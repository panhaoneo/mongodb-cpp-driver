#include "Mongo.h"

CNFMongo::CNFMongo()
{
    _isConnect = false;
    _pMongoPoolptr = nullptr;
     _instance = nullptr;
}

CNFMongo::CNFMongo(const string &hostip, const string &port, const string &username, const string& password, const string &dbname)
{
    init(hostip, port, username, password, dbname);
}

CNFMongo::CNFMongo(const NF_MongoDBConf& nfDbConf)
{
    _dbConf = nfDbConf;
    init(nfDbConf);
}

void CNFMongo::init(const string &hostip, const string &port, const string &username, const string& password, const string &dbname)
{
    _dbConf.init(hostip, port, username, password, dbname);
    try
    {
        _instance = std::move(std::unique_ptr<mongocxx::instance>(new mongocxx::instance{}));
        if(!_instance)
        {
            LOG_ERROR << "create mongocxx::instance{} failed." << endl;
            return -1;
        }

        string strMongoAddr = "";
        if(username.empty() || password.empty())
        {
            strMongoAddr =  "mongodb://" + hostip + ":" +port;
        }
        else
        {
            strMongoAddr = "mongodb://" + username + ":" + password + "@" + hostip + ":" +port;
        }
    
        mongocxx::uri uri = mongocxx::uri{ strMongoAddr };
        _pMongoPoolptr = new mongocxx::pool{uri};
        _bConnect = true;
    }
    catch(mongocxx::exception &e)
    {
        LOG_ERROR << e.what() << '\n';
    }
    catch(...)
    {
        LOG_ERROR << "unkown exception" << endl;
    }
    
}

void init(const NF_MongoDBConf& nfDbConf)
{
    init(nfDbConf._host, nfDbConf._port, nfDbConf._user, nfDbConf._password, nfDbConf._database);
}

void CNFMongo::Connect()
{
    init(_dbConf);
}


#define DOC_APPEND_DATA(doc,mapData)  MONGODB_RECORD_DATA::iterator itEnd = mapData.end()\
            for(MONGODB_RECORD_DATA::iterator it=mapData.begin(); it != itEnd; it++)\
            {\
                if(it->second.first == MONGODB_INT64)\
                {\
                    doc.append(kvp(it->first, types::b_int64(it->second.second)));\
                }\
                else if(it->second.first == MONGODB_DOUBLE)\
                {\
                    doc.append(kvp(it->first, types::b_double(it->second.second)));\
                }\
                else if(it->second.first == MONGODB_STRING)\
                {\
                    doc.append(kvp(it->first, types::b_symbol(it->second.second)))\
                }\
            }\

#define DOC_APPEND_INDEX(doc,mapData)  MONGODB_RECORD_DATA::iterator itEnd = mapData.end()\
            for(MONGODB_RECORD_DATA::iterator it=mapData.begin(); it != itEnd; it++)\
            {\
                doc.append(kvp(it->first, 1));\
            }\

int CNFMongo::UpdateRecord(const string &strAreaDb, const string &collection, const MONGODB_RECORD_DATA &mapData, const MONGODB_RECORD_DATA &mapFilter)
{
    if(!_bConnect)
    {
        Connect();
    }

    try
    {
        mongocxx::pool::entry client = _pMongoPoolptr->acquire();
        auto coll = client[strAreaDb][collection];
        auto doc = builder::basic::document{};
        auto docFilter = builder::basic::document{};
        auto docIndex = builder::basic::document{};

        using bsoncxx::builder::basic::kvp;
        {
            DOC_APPEND_DATA(doc,mapData);
        }
        {    
            DOC_APPEND_DATA(docFilter,mapFilter);
        }
  
        options::update opUpdate;
        opUpdate.upsert(true); 
        auto result = coll.update_one(docFilter.view(), doc.view(), opUpdate);
        if(!result)
        {
            LOG_ERROR << "update failed | docvalue=" <<  bsoncxx::to_json(doc.view()) << endl;
            return -1;
        } 
        DOC_APPEND_INDEX(docIndex, mapData);
        options::index_view opIndex;
        opIndex.background(true);
        coll.create_index(docIndex.view(), opIndex);
    }
    catch(mongocxx::exception &e)
    {
        LOG_ERROR << e.what() << endl;
        return -1;
    }
    catch(...)
    {
        LOG_ERROR << "unknown exception" << endl;
        return -1;
    }
    
    return 0;
}


int InsertRecord(const string &strAreaDb, const string &collection, const MONGODB_RECORD_DATA &mapData)
{
    if(!_bConnect)
    {
        Connect();
    }

    try
    {
        mongocxx::pool::entry client = _pMongoPoolptr->acquire();
        auto coll = client[strAreaDb][collection];
        auto doc = builder::basic::document{};
        auto docIndex = builder::basic::document{};

        using bsoncxx::builder::basic::kvp;
        DOC_APPEND_DATA(doc,mapData);
        auto result = coll.insert_one(doc);
        if(! result)
        {
            LOG_ERROR << "insert failed | docvalue=" <<  bsoncxx::to_json(doc.view()) << endl;
            return -1;
        }

        DOC_APPEND_INDEX(docIndex, mapData);
        options::index_view opIndex;
        opIndex.background(true);
        coll.create_index(docIndex.view(), opIndex);
    }
    catch(mongocxx::exception &e)
    {
        LOG_ERROR << e.what() << '\n';
    }
    catch(...)
    {
        LOG_ERROR << "unknown exception" << endl;
    }
    return 0;
}
