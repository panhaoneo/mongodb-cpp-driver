#include "Mongo.h"
#include <iostream>


#define KEY_CODE		"code"
#define KEY_DATE		"date"
#define KEY_FIELDNAME		"fieldname"
#define KEY_VALUE		"value"

int getRand()
{
	srand((unsigned)time(NULL));  
	return rand();
}


struct FactorMessage{
	string _strArea;
	string _strCollection;
	string _strCode;
	int32_t _iDate;
	string _strfieldname;
	int32_t _iDecimal;
	int64_t _Value;
	int64_t _iSerialNum;
};

int main()
{
	string strConfigFile =  "Mongo.conf";
	cout  << "init config: " << strConfigFile << endl;
	TC_Config config;
	config.parseFile(strConfigFile);

	NF_MongoDBConf dbConf;
	dbConf.loadFromMap(config.getDomainMap("/conf/db_mongo"));

	std::shared_ptr<CNFMongo> _pMongoSharedptr = std::make_shared<CNFMongo>(dbConf);

	FactorMessage msg;
	msg._iDate = 20190724;
	msg._iDecimal = 1;
	msg._strArea = string("TestDB");
	msg._strCode = string(I2S(getRand()));
	msg._strCollection = string("testCollection");
	msg._strfieldname = string("open") ;
	msg._Value = getRand();

	MONGODB_RECORD_DATA rc;
	rc[KEY_CODE] = make_pair(MONGODB_STRING, msg._strCode);
	rc[KEY_DATE] = make_pair(MONGODB_INT32, msg._iDate);
	rc[KEY_FIELDNAME] = make_pair(MONGODB_STRING, msg._strfieldname);
	if(msg._iDecimal == 1)
	{
		rc[KEY_VALUE] = make_pair(MONGODB_INT64, msg._Value);
	}
	else
	{
		rc[KEY_VALUE] = make_pair(MONGODB_DOUBLE, (double)msg._Value/msg._iDecimal);
	}

	int ret = _pMongoSharedptr->InsertRecord(msg._strArea, msg._strCollection, rc);
	if(ret !=  0)
	{
		cout << "insert failed" << endl;
	}

	return 0;
}
