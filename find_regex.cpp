//记录下mongo driver的坑，正则表达式的组装

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
	mongocxx::uri uri = mongocxx::uri("mongodb://root:******@127.0.0.1:27017");
	mongocxx::pool *_pMongoPoolptr = new mongocxx::pool(uri);

	mongocxx::pool::entry pentry = _pMongoPoolptr->acquire();
	mongocxx::client & client = *pentry;
	mongocxx::collection coll = client["db"]["collection'];

	try {
		bsoncxx::builder::stream::document doc,docfilter;
		bsoncxx::document::value q(doc.view());
		using bsoncxx::builder::basic::kvp;
	
		std::string query = " { \"$not\" : { \"$regex\" : \"上海\" }  }";
		q = bsoncxx::from_json(query);
		std::cout << bsoncxx::to_json(q.view()) << std::endl;	
		docfilter << "fieldname" << "ZHUCEDIZHI" << "value" << concatenate(q.view());
		//2
		std::cout << bsoncxx::to_json(docfilter) << std::endl;
		
		auto result = coll.find( docfilter.view());
		for (auto&& it : result) {
			std::cout << bsoncxx::to_json(it) << std::endl;
		}

	} catch (const mongocxx::exception& e) {
		std::cout << "An exception occurred: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

