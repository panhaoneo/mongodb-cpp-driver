#include <iostream>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/document/element.hpp>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/builder/concatenate.hpp>

using namespace::bsoncxx;
using bsoncxx::builder::basic::kvp;
using builder::stream::open_document;
using builder::stream::close_document;
using bsoncxx::builder::concatenate;


int main(int, char**) {
	// The mongocxx::instance constructor and destructor initialize and shut down the driver,
	// respectively. Therefore, a mongocxx::instance must be created before using the driver and
	// must remain alive for as long as the driver is in use.
	mongocxx::instance inst{};
	mongocxx::uri uri = mongocxx::uri("mongodb://");
	mongocxx::pool *_pMongoPoolptr = new mongocxx::pool(uri);

	mongocxx::pool::entry pentry = _pMongoPoolptr->acquire();
	mongocxx::client & client = *pentry;
	mongocxx::collection coll = client["test"]["testCollection"];

	try {
		bsoncxx::builder::basic::document doc,docfilter;
		bsoncxx::document::value q(doc.view());
		using bsoncxx::builder::basic::kvp;
		using bsoncxx::builder::basic::sub_document;
		using bsoncxx::builder::basic::sub_array;

		doc.append(kvp("code", "0101600000"), kvp("date", 20190819), kvp("update", 20190819));
		doc.append(kvp("dataArray", [](sub_array subarr){
					subarr.append(
						[](sub_document subdoc){
						subdoc.append(kvp("fieldname", "yingyeting"), kvp("value", "shanghai"));
						});
					subarr.append(
						[](sub_document subdoc){
						subdoc.append(kvp("fieldname", "yingyee"), kvp("value", 1234354.90));});
					}) );

		std::cout << bsoncxx::to_json(doc.view()) << std::endl;	
		//2
		coll.insert_one(doc.view());
	} catch (const mongocxx::exception& e) {
		std::cout << "An exception occurred: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

