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
#include <mongocxx/options/update.hpp>
#include <mongocxx/options/insert.hpp>

using namespace::std;
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

//mongo操作的数据  嵌套map key：存入mongodb的键  value：pair<ValueType, ValueVariant> 值的类型和数值
typedef map<string, pair<ValueType, ValueVariant> > MONGODB_RECORD_DATA;

class CNFMongo : public TC_Singleton<CNFMongo>
{
public:
	CNFMongo();
	CNFMongo(const string &hostip, const string &port, const string &username, const string& password);
    CNFMongo(const NF_MongoDBConf& nfDbConf);
	~CNFMongo();
    void Init(const string &hostip, const string &port, const string &username, const string& password);
    void Init(const NF_MongoDBConf& nfDbConf);

    /**
    * @brief 更新mongo单条数据记录 
    * 
    * @param strAreaDb  所属db
    *        collection 所属collection
    *        mapData    map<string, pair<ValueType, ValueVariant> >  存储的数据
    *        mapFilter  map<string, pair<ValueType, ValueVariant> >  需要更新的项
    * 
    * @return 0-正常  -1-异常
    */
    int UpdateRecord(const string &strAreaDb, const string &collection, MONGODB_RECORD_DATA &mapData, MONGODB_RECORD_DATA &mapFilter);

    /**
    * @brief 插入mongo单条数据记录 
    * 
    * @param strAreaDb  所属db
    *        collection 所属collection
    *        mapData    map<string, pair<ValueType, ValueVariant> >  存储的数据
    * 
    * @return 0-正常  -1-异常
    */
    int InsertRecord(const string &strAreaDb, const string &collection, MONGODB_RECORD_DATA &mapData);

    /**
    * @brief 创建单字段索引 
    * 
    * @param strAreaDb  所属db
    *        collection 所属collection
    *        mapData    map<string, pair<ValueType, ValueVariant> >  需要创建索引的列表
    * 
    * @return 0-正常  -1-异常
    */
    int BuildSingleIndex(const string &strAreaDb, const string &collection, MONGODB_RECORD_DATA &mapData);

      /**
    * @brief 简单doc查找
    * 
    * @param strAreaDb  所属db
    *        collection 所属collection
    *        mapData    map<string, pair<ValueType, ValueVariant> >  需要查找的条件
    *        cursor     查找到的迭代器集合
    * @return 0-正常  -1-异常
    */
    int FindRecord(const string &strAreaDb, const string &collection, MONGODB_RECORD_DATA &mapData ,mongocxx::cursor &cursor);

private:
	void connect();
	bool get_client(mongocxx::pool::entry& dbClient);
	bool try_get_client(mongocxx::stdx::optional<mongocxx::pool::entry>& dbClient);

	void buildDoc(MONGODB_RECORD_DATA& mapData,  bsoncxx::builder::basic::document &doc);
private:
    std::unique_ptr<mongocxx::instance> _instance;
    bool _bConnect;
    mongocxx::pool *_pMongoPoolptr;
    NF_MongoDBConf _dbConf;
};

#define MongoHandle CNFMongo::getInstance()

#endif

