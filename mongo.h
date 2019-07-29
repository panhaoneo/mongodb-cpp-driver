#ifndef _MONGO_H_
#define _MONGO_H_

#include "util/tc_thread.h"
#include "util/tc_monitor.h"
#include "util/tc_thread_queue.h"
#include "util/tc_config.h"
#include "Common.h"
#include "boost/variant.hpp"


#include <iostream>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/exception/exception.hpp>



using namespace::std;
using std::string;
using bsoncxx::builder::basic::kvp;

struct NF_MongoDBConf
{
    string _host;
    string _user;
    string _password;
    string _port;

    /**
    * @brief 构造函数
    */
    NF_MongoDBConf()
    {
    }

    void init(const string &hostip, const string &port, const string &username, const string& password)
    {
        _host = hostip;
        _user = username;
        _password = password;
        _port = port;
    }

    /**
    * @brief 读取数据库配置. 
    * 
    * @param mpParam 存放数据库配置的map 
    *        dbhost: 主机地址
    *        dbuser:用户名
    *        dbpass:密码
    *        dbport:端口
    */
    void loadFromMap(const map<string, string> &mpParam)
    {
        map<string, string> mpTmp = mpParam;

        _host        = mpTmp["dbhost"];
        _user        = mpTmp["dbuser"];
        _password    = mpTmp["dbpass"];
        _port        = mpTmp["dbport"];

        if(mpTmp["dbport"] == "")
        {
            _port = "27017";
        }
    }

};


enum ValueType
{
	MONGODB_INT32,
	MONGODB_INT64,
	MONGODB_DOUBLE,
	MONGODB_STRING,
};

typedef  boost::variant<int32_t,int64_t,double,std::string> ValueVariant;
typedef map<string, pair<ValueType, ValueVariant> > MONGODB_RECORD_DATA;

class CNFMongo 
{
public:
	CNFMongo();
	CNFMongo(const string &hostip, const string &port, const string &username, const string& password);
    CNFMongo(const NF_MongoDBConf& nfDbConf);
	~CNFMongo();
    void Init(const string &hostip, const string &port, const string &username, const string& password);
    void Init(const NF_MongoDBConf& nfDbConf);

    

    int UpdateRecord(const string &strAreaDb, const string &collection, const MONGODB_RECORD_DATA &mapData, const MONGODB_RECORD_DATA &mapFilter);

    int InsertRecord(const string &strAreaDb, const string &collection, const MONGODB_RECORD_DATA &mapData);

private:
	void connect();
	bool get_client(mongocxx::pool::entry& dbClient);
	bool try_get_client(mongocxx::stdx::optional<mongocxx::pool::entry>& dbClient);

	void buildDoc(MONGODB_RECORD_DATA mapData,  bsoncxx::builder::basic::document &doc);
	void buildIndexDoc(MONGODB_RECORD_DATA mapData,  bsoncxx::builder::basic::document &doc);

private:
    std::unique_ptr<mongocxx::instance> _instance;
    bool _bConnect;
    mongocxx::pool *_pMongoPoolptr;
    NF_MongoDBConf _dbConf;
};


#endif

