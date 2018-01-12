#include "../src/info/info_dslink_node.h"
#include "dslink.h"
#include <iostream>

#include "../deps/sdk-dslink-cpp/test/sdk/async_test.h"
#include "../deps/sdk-dslink-cpp/test/sdk/test_config.h"
#include "module/default/console_logger.h"
#include "gtest/gtest.h"

using namespace dsa;
using namespace std;
using namespace dslink_info;

TEST(SystemLinkTest, SSubscribeTest) {
  shared_ptr<App> app = make_shared<App>();
  auto process = make_ref_<info::ProcessHandler>("", "", "");
  const char *argv[] = {"./testResp", "--broker",      "ds://127.0.0.1:4125",
                        "-l",         "info",          "--thread",
                        "4",          "--server-port", "4161"};
  int argc = 9;
  auto linkResp = make_ref_<DsLink>(argc, argv, "mydslink", "1.0.0", app);

  // filter log for unit test
  static_cast<ConsoleLogger &>(linkResp->strand->logger()).filter =
      Logger::FATAL_ | Logger::ERROR_ | Logger::WARN__;

  ref_<InfoDsLinkNode> _info_node = make_ref_<InfoDsLinkNode>(
      linkResp->strand->get_ref(), std::move(process));

  linkResp->init_responder(std::move(_info_node));

  //  linkResp->init_responder<InfoDsLinkNode>();
  linkResp->connect([&](const shared_ptr_<Connection> connection) {});

  // Create link
  std::string address = std::string("127.0.0.1:") + std::to_string(4161);
  const char *argv2[] = {"./test", "-b", address.c_str()};
  int argc2 = 3;
  auto link = make_ref_<DsLink>(argc2, argv2, "mydslink", "1.0.0", app);

  // filter log for unit test
  static_cast<ConsoleLogger &>(link->strand->logger()).filter =
      Logger::FATAL_ | Logger::ERROR_ | Logger::WARN__;

  // connection
  bool is_connected = false;
  link->connect(
      [&](const shared_ptr_<Connection> connection) { is_connected = true; });

  ASYNC_EXPECT_TRUE(2000, *link->strand, [&]() { return is_connected; });

  std::vector<string_> list_result;
  // List test
  link->list("", [&](IncomingListCache &cache,
                     const std::vector<string_> &str) { list_result = str; });

  std::vector<std::string> messages;

	//  auto subs_cache = link->subscribe("main/cpu_usage", [&](IncomingSubscribeCache &cache,
	//	  ref_<const SubscribeResponseMessage> message) {
	//	  messages.push_back(message->get_value().value.to_string());
	//	  cout << "CPU Usage: " << message->get_value().value.to_string()
	//		  << "\n";
	//  });
	//  WAIT_EXPECT_TRUE(2000, [&]() { return messages.size() > 0; });
	//  WAIT(2000);
	//  subs_cache->close();
	//  WAIT(1000);

	//  messages.clear();

	//  auto subs_cache2 = link->subscribe("main/free_memory",
	//	  [&](IncomingSubscribeCache &cache,
	//		  ref_<const SubscribeResponseMessage> message) {
	//	  messages.push_back(message->get_value().value.to_string());
	//	  cout << "Free Memory: " <<
	//		  message->get_value().value.to_string() << "\n";
	//  });
	//  WAIT_EXPECT_TRUE(2000, [&]() { return messages.size() > 0; });
	//  WAIT(2000);
	//  subs_cache2->close();
	//  WAIT(1000);

	//  messages.clear();

	//  auto subs_cache3 = link->subscribe("main/used_memory",
	//	  [&](IncomingSubscribeCache &cache,
	//		  ref_<const SubscribeResponseMessage> message) {
	//	  messages.push_back(message->get_value().value.to_string());
	//	  cout << "Used Memory: " <<
	//		  message->get_value().value.to_string() << "\n";
	//  });
	//  WAIT_EXPECT_TRUE(2000, [&]() { return messages.size() > 0; });
	//  WAIT(2000);
	//  subs_cache3->close();
	//  WAIT(1000);

	//  messages.clear();

	//  auto subs_cache4 = link->subscribe("main/total_memory",
	//	  [&](IncomingSubscribeCache &cache,
	//		  ref_<const SubscribeResponseMessage> message) {
	//	  messages.push_back(message->get_value().value.to_string());
	//	  cout << "Total Memory: " <<
	//		  message->get_value().value.to_string() << "\n";
	//  });
	//  WAIT_EXPECT_TRUE(2000, [&]() { return messages.size() > 0; });
	//  WAIT(2000);
	//  subs_cache4->close();
	//  WAIT(1000);

	//  messages.clear();

	//  auto subs_cache5 = link->subscribe("main/free_disk_space",
	//	  [&](IncomingSubscribeCache &cache,
	//		  ref_<const SubscribeResponseMessage> message) {
	//	  messages.push_back(message->get_value().value.to_string());
	//	  cout << "Free Disk Space: " <<
	//		  message->get_value().value.to_string() << "\n";
	//  });
	//  WAIT_EXPECT_TRUE(2000, [&]() { return messages.size() > 0; });
	//  WAIT(2000);
	//  subs_cache5->close();
	//  WAIT(1000);

	//  messages.clear();

	//  auto subs_cache6 = link->subscribe("main/used_disk_space",
	//	  [&](IncomingSubscribeCache &cache,
	//		  ref_<const SubscribeResponseMessage> message) {
	//	  messages.push_back(message->get_value().value.to_string());
	//	  cout << "Used Disk Space: " <<
	//		  message->get_value().value.to_string() << "\n";
	//  });
	//  WAIT_EXPECT_TRUE(2000, [&]() { return messages.size() > 0; });
	//  WAIT(2000);
	//  subs_cache6->close();
	//  WAIT(1000);

	//  messages.clear();

	//  auto subs_cache7 = link->subscribe("main/total_disk_space",
	//	  [&](IncomingSubscribeCache &cache,
	//		  ref_<const SubscribeResponseMessage> message) {
	//	  messages.push_back(message->get_value().value.to_string());
	//	  cout << "Total Disk Space: " <<
	//		  message->get_value().value.to_string() << "\n";
	//  });
	//  WAIT_EXPECT_TRUE(2000, [&]() { return messages.size() > 0; });
	//  WAIT(2000);
	//  subs_cache7->close();
	//  WAIT(1000);

	//  messages.clear();

	//  auto subs_cache8 = link->subscribe("main/disk_usage",
	//	  [&](IncomingSubscribeCache &cache,
	//		  ref_<const SubscribeResponseMessage> message) {
	//	  messages.push_back(message->get_value().value.to_string());
	//	  cout << "Disk Usage: " <<
	//		  message->get_value().value.to_string() << "\n";
	//  });
	//  WAIT_EXPECT_TRUE(2000, [&]() { return messages.size() > 0; });
	//  WAIT(2000);
	//  subs_cache8->close();
	//  WAIT(1000);

	//  messages.clear();

	//	auto subs_cache9 = link->subscribe("main/hardware_model",
	//		[&](IncomingSubscribeCache &cache,
	//			ref_<const SubscribeResponseMessage> message) {
	//		messages.push_back(message->get_value().value.to_string());
	//		cout << "Device Name: " <<
	//			message->get_value().value.to_string() << "\n";
	//	});
	//	WAIT_EXPECT_TRUE(2000, [&]() { return messages.size() > 0; });
	//	WAIT(2000);
	//	subs_cache9->close();
	//	WAIT(1000);

	//	messages.clear();

	//	auto subs_cache10 = link->subscribe("main/model_name",
	//		[&](IncomingSubscribeCache &cache,
	//			ref_<const SubscribeResponseMessage> message) {
	//		messages.push_back(message->get_value().value.to_string());
	//		cout << "Model Name: " <<
	//			message->get_value().value.to_string() << "\n";
	//	});
	//	WAIT_EXPECT_TRUE(2000, [&]() { return messages.size() > 0; });
	//	WAIT(2000);
	//	subs_cache10->close();
	//	WAIT(1000);

	//	messages.clear();

	auto subs_cache11 = link->subscribe("main/hostname",
		[&](IncomingSubscribeCache &cache,
			ref_<const SubscribeResponseMessage> message) {
		messages.push_back(message->get_value().value.to_string());
		cout << "Model Name: " <<
			message->get_value().value.to_string() << "\n";
	});
	WAIT_EXPECT_TRUE(2000, [&]() { return messages.size() > 0; });
	WAIT(2000);
	subs_cache11->close();
	WAIT(1000);

	messages.clear();

	auto subs_cache12 = link->subscribe(
		"main/cpu_frequency", [&](IncomingSubscribeCache &cache,
			ref_<const SubscribeResponseMessage> message) {
		messages.push_back(message->get_value().value.to_string());
		cout << "CPU Frequency: " << message->get_value().value.to_string()
			<< "\n";
	});
	WAIT_EXPECT_TRUE(2000, [&]() { return messages.size() > 0; });
	WAIT(2000);
	subs_cache12->close();
	WAIT(1000);

	messages.clear();

  auto first_request = make_ref_<InvokeRequestMessage>();
  first_request->set_target_path("main/execute_stream_command");
  first_request->set_value(Var{{"cmd", Var("ping google.com")}});

  ref_<const InvokeResponseMessage> last_response;

  auto invoke_stream = link->invoke(
      [&](IncomingInvokeStream &stream,
          ref_<const InvokeResponseMessage> &&msg) {
        last_response = std::move(msg);
        if ((last_response != nullptr) && last_response->get_value().is_map())
          std::cout
              << std::endl
              << last_response->get_value().get_map()["output"].to_string()
              << std::endl;
      },
      copy_ref_(first_request));

  ASYNC_EXPECT_TRUE(2000, *link->strand, [&]() -> bool {
    return ((last_response != nullptr) && last_response->get_value().is_map());
  });

  WAIT(5000);
  invoke_stream->close();

  WAIT_EXPECT_TRUE(2000, [&]() -> bool { return invoke_stream->is_closed(); });
  WAIT(4000);

  // Cleaning test
  //  destroy_dslink_in_strand(linkResp);
  //  destroy_dslink_in_strand(link);

  linkResp->strand->dispatch([linkResp]() { linkResp->destroy(); });
  WAIT_EXPECT_TRUE(2000, [&]() -> bool { return linkResp->is_destroyed(); });
  link->strand->dispatch([link]() { link->destroy(); });
  WAIT_EXPECT_TRUE(2000, [&]() -> bool { return link->is_destroyed(); });

  app->close();
  WAIT_EXPECT_TRUE(500, [&]() -> bool { return app->is_stopped(); });

  if (!app->is_stopped()) {
    app->force_stop();
  }
  app->wait();
}
