#include "Mongo.h"

CNFMongo::CNFMongo()
{

}

CNFMongo::CNFMongo(const string &hostip, const string &port, const string &username, const string& password, const string &dbname)
{
    init(hostip, port, username, password, dbname);
}

int CNFMongo::init(const string &hostip, const string &port, const string &username, const string& password, const string &dbname)
{
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
        mongocxx::pool *pool = new mongocxx::pool{uri};
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
