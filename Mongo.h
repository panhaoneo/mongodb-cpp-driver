#ifndef _MONGO_H_
#define _MONGO_H_

#include "util/tc_thread.h"
#include "util/tc_monitor.h"
#include "util/tc_thread_queue.h"
#include "util/tc_config.h"
#include "Common.h"

#include <string>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>

using namespace::std;
using bsoncxx::builder::basic::kvp;

struct NF_MongoDBConf
{
  
    string _host;
    string _user;
    string _password;
    string _port;
    string _database;

    /**
    * @brief 构造函数
    */
    NF_MongoDBConf()
    {
    }

    /**
    * @brief 读取数据库配置. 
    * 
    * @param mpParam 存放数据库配置的map 
    *        dbhost: 主机地址
    *        dbuser:用户名
    *        dbpass:密码
    *        dbport:端口
    *        dbname:数据库名称
    */
    void loadFromMap(const map<string, string> &mpParam)
    {
        map<string, string> mpTmp = mpParam;

        _host        = mpTmp["dbhost"];
        _user        = mpTmp["dbuser"];
        _password    = mpTmp["dbpass"];
        _port        = mpTmp["dbport"];
        _database    = mpTmp["dbname"];

        if(mpTmp["dbport"] == "")
        {
            _port = "27017";
        }
    }

};


class CNFMongo
{
public:
	CNFMongo();
	CNFMongo(const string &hostip, const string &port, const string &username, const string& password, const string &dbname);
    CNFMongo(const NF_MongoDBConf& nfDbConf);
	~CNFMongo();
    int init(const string &hostip, const string &port, const string &username, const string& password, const string &dbname);
    int init(const NF_MongoDBConf& nfDbConf);

	void connect();
	void disconnect();

    enum ValueType
    {
        MONGODB_INT64,
        MONGODB_DOUBLE,
    };
    typedef map<string, pair<ValueType, string>> MONGODB_RECORD_DATA;

    size_t UpdateRecord(const string &strAreaDb, const string &collection, const map<string, pair<ValueType, string> > &mapData);

private:
    std::unique_ptr<mongocxx::instance> _instance = nullptr;
    mongocxx::pool::entry _MongoClient;
};

#endif

