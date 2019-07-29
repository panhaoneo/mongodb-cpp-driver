#include "Mongo.h"

CNFMongo::CNFMongo()
{
	_bConnect = false;
	_pMongoPoolptr = nullptr;
	_instance = nullptr;
}

CNFMongo::~CNFMongo()
{
	if(_pMongoPoolptr)
	{
		delete _pMongoPoolptr;
		_pMongoPoolptr = nullptr;
	}
}

CNFMongo::CNFMongo(const string &hostip, const string &port, const string &username, const string& password)
{
	Init(hostip, port, username, password);
}

CNFMongo::CNFMongo(const NF_MongoDBConf& nfDbConf)
{
	_dbConf = nfDbConf;
}

void CNFMongo::Init(const string &hostip, const string &port, const string &username, const string& password)
{
	_dbConf.init(hostip, port, username, password);
}

void CNFMongo::Init(const NF_MongoDBConf& nfDbConf)
{
	_dbConf = nfDbConf;
}

void CNFMongo::connect()
{
	try
	{
		_instance = std::move(std::unique_ptr<mongocxx::instance>(new mongocxx::instance()));
		if(!_instance)
		{
			LOG_ERROR << "create mongocxx::instance{} failed." << endl;
			return ;
		}

		string strMongoAddr = "";
		if(_dbConf._user.empty() || _dbConf._password.empty())
		{
			strMongoAddr =  "mongodb://" + _dbConf._host + ":" +_dbConf._port;
		}
		else
		{
			strMongoAddr = "mongodb://" + _dbConf._user + ":" + _dbConf._password + "@" + _dbConf._host + ":" + _dbConf._port;
		}

		mongocxx::uri uri = mongocxx::uri(strMongoAddr);
		_pMongoPoolptr = new mongocxx::pool(uri);
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


bool CNFMongo::get_client(mongocxx::pool::entry& dbClient)
{
	if(!_bConnect)
	{
		connect();
	}

	if(_pMongoPoolptr)
	{
		dbClient = std::move(_pMongoPoolptr->acquire());
		return true;
	}
	else
	{
		return false;
	}
}

bool CNFMongo::try_get_client(mongocxx::stdx::optional<mongocxx::pool::entry>& dbClient)
{
	if(!_bConnect)
	{
		connect();
	}

	if(_pMongoPoolptr)
	{
		dbClient = std::move(_pMongoPoolptr->try_acquire());
		return true;
	}
	else
	{
		return false;
	}
}


void CNFMongo::buildDoc(const MONGODB_RECORD_DATA& mapData,  bsoncxx::builder::basic::document &doc)
{
	MONGODB_RECORD_DATA::iterator itEnd = mapData.end();
	for(MONGODB_RECORD_DATA::iterator it=mapData.begin(); it != itEnd; it++)
	{
		if(it->second.first == MONGODB_INT32)
		{ 
			doc.append(kvp(it->first, bsoncxx::types::b_int32{boost::get<int32_t>(it->second.second)} ));
		}
		else if(it->second.first == MONGODB_INT64)
		{
			doc.append(kvp(it->first, bsoncxx::types::b_int64{boost::get<int64_t>(it->second.second)} ));
		}
		else if(it->second.first == MONGODB_DOUBLE)
		{ 
			doc.append(kvp(it->first, bsoncxx::types::b_double{boost::get<double>(it->second.second)} ));
		}
		else if(it->second.first == MONGODB_STRING)
		{ 
			doc.append(kvp(it->first, boost::get<string>(it->second.second)));
		}
	}
}

int CNFMongo::UpdateRecord(const string &strAreaDb, const string &collection, const MONGODB_RECORD_DATA &mapData, const MONGODB_RECORD_DATA &mapFilter)
{
	try
	{
		mongocxx::stdx::optional<mongocxx::pool::entry> client;
		bool ret = try_get_client(client);
		if(!ret || !client)
		{
			LOG_ERROR << "get client failed " << endl;
			return -1;
		}

		mongocxx::client &conn = **client;
		mongocxx::collection coll = conn[strAreaDb][collection];
		bsoncxx::builder::basic::document doc,docFilter;

		using bsoncxx::builder::basic::kvp;
		buildDoc(mapData, doc);  
		buildDoc(mapFilter, docFilter);

		mongocxx::options::update opUpdate;
		opUpdate.upsert(true); 
		auto result = coll.update_one(docFilter.view(), doc.view(), opUpdate);
		if(!result)
		{
			LOG_ERROR << "update failed | docvalue=" <<  bsoncxx::to_json(doc.view()) << endl;
			return -1;
		} 
	}
	catch(mongocxx::exception &e)
	{
		LOG_ERROR << e.what() << endl;
		return -1;
	}
	catch (exception &ex)
	{
		LOG_ERROR << "[exception]:" << ex.what() << endl;
		return -1;
	}
	catch(...)
	{
		LOG_ERROR << "unknown exception" << endl;
		return -1;
	}

	return 0;
}


int CNFMongo::InsertRecord(const string &strAreaDb, const string &collection, const MONGODB_RECORD_DATA &mapData)
{
	try
	{
		mongocxx::stdx::optional<mongocxx::pool::entry> client;
		bool ret = try_get_client(client);
		if(!ret || !client)
		{
			LOG_ERROR << "get client failed " << endl;
			return -1;
		}

		mongocxx::client &conn = **client;
		mongocxx::collection coll = conn[strAreaDb][collection];
		bsoncxx::builder::basic::document doc;

		using bsoncxx::builder::basic::kvp;
		buildDoc(mapData, doc);
		auto result = coll.insert_one(doc.view());
		if(! result)
		{
			LOG_ERROR << "insert failed | docvalue=" <<  bsoncxx::to_json(doc.view()) << endl;
			return -1;
		}
	}
	catch(mongocxx::exception &e)
	{
		LOG_ERROR << e.what() << '\n';
		return -1;
	}
	catch (exception &ex)
	{
		LOG_ERROR << "[exception]:" << ex.what() << endl;
		return -1;
	}
	catch(...)
	{
		LOG_ERROR << "unknown exception" << endl;
		return -1;
	}
	return 0;
}


int CNFMongo::BuildSingleIndex(const string &strAreaDb, const string &collection, const MONGODB_RECORD_DATA &mapData)
{
	try
	{
		mongocxx::stdx::optional<mongocxx::pool::entry> client;
		bool ret = try_get_client(client);
		if(!ret || !client)
		{
			LOG_ERROR << "get client failed " << endl;
			return -1;
		}

		mongocxx::client &conn = **client;
		mongocxx::collection coll = conn[strAreaDb][collection];

		using bsoncxx::builder::basic::kvp;
		mongocxx::options::index opIndex;
		opIndex.background(true);

		MONGODB_RECORD_DATA::iterator itEnd = mapData.end();
		for(MONGODB_RECORD_DATA::iterator it=mapData.begin(); it != itEnd; it++)
		{
			bsoncxx::builder::basic::document doc;
			doc.append(kvp(it->first, -1));
			coll.create_index(doc.view(), opIndex);
		}	
	}
	catch(mongocxx::exception &e)
	{
		LOG_ERROR << e.what() << '\n';
		return -1;
	}
	catch (exception &ex)
	{
		LOG_ERROR << "[exception]:" << ex.what() << endl;
		return -1;
	}
	catch(...)
	{
		LOG_ERROR << "unknown exception" << endl;
		return -1;
	}
	return 0;
}

int CNFMongo::FindRecord(const string &strAreaDb, const string &collection, const MONGODB_RECORD_DATA &mapData ,mongocxx::cursor &cursor)
{
	try
	{
		mongocxx::stdx::optional<mongocxx::pool::entry> client;
		bool ret = try_get_client(client);
		if(!ret || !client)
		{
			LOG_ERROR << "get client failed " << endl;
			return -1;
		}

		mongocxx::client &conn = **client;
		mongocxx::collection coll = conn[strAreaDb][collection];

		bsoncxx::builder::basic::document doc;
		buildDoc(mapData, doc);
		cursor = coll.find(doc.view());
	}
	catch(mongocxx::exception &e)
	{
		LOG_ERROR << e.what() << '\n';
		return -1;
	}
	catch (exception &ex)
	{
		LOG_ERROR << "[exception]:" << ex.what() << endl;
		return -1;
	}
	catch(...)
	{
		LOG_ERROR << "unknown exception" << endl;
		return -1;
	}
	return 0;
}

