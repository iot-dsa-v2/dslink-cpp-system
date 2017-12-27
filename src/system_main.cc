#include "info/info_dslink_node.h"
#include "dslink.h"
#include <iostream>
#include "module/default/console_logger.h"

using namespace dsa;
using namespace std;
using namespace dslink_info;

int main() {
  shared_ptr<App> app = make_shared<App>();

  const char *argv[] = {"./testResp", "--broker", "ds://127.0.0.1:4120",
                        "-l", "info", "--thread",
                        "4", "--server-port", "4121"};
  int argc = 9;
  auto linkResp = make_ref_<DsLink>(argc, argv, "mydslink", "1.0.0");

  // filter log for unit test
  static_cast<ConsoleLogger &>(linkResp->strand->logger()).filter =
      Logger::FATAL_ | Logger::ERROR_ | Logger::WARN__;

  linkResp->init_responder<InfoDsLinkNode>();
  linkResp->connect([&](const shared_ptr_<Connection> connection) {});

  // Create link
  std::string address = std::string("127.0.0.1:") + std::to_string(4121);
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

//  ASYNC_EXPECT_TRUE(500, *link->strand, [&]() { return is_connected; });
  std::vector<string_> list_result;
  // List test
  link->list("", [&](IncomingListCache &cache,
                     const std::vector<string_> &str) { list_result = str;
  });

  std::vector<std::string> messages;
  link->subscribe("main/CPU_Usage",
                  [&](IncomingSubscribeCache &cache,
                      ref_<const SubscribeResponseMessage> message) {
                    messages.push_back(message->get_value().value.get_string());
                    cout << "CPU Usage: " << message->get_value().value.get_string() << "\n";
                  });

  link->subscribe("main/Free_Memory",
                  [&](IncomingSubscribeCache &cache,
                      ref_<const SubscribeResponseMessage> message) {
                    messages.push_back(message->get_value().value.get_string());
                    cout << "Free Memory: " << message->get_value().value.get_string() << "\n";
                  });

  link->subscribe("main/Used_Memory",
                  [&](IncomingSubscribeCache &cache,
                      ref_<const SubscribeResponseMessage> message) {
                    messages.push_back(message->get_value().value.get_string());
                    cout << "Used Memory: " << message->get_value().value.get_string() << "\n";
                  });

  link->subscribe("main/Total_Memory",
                  [&](IncomingSubscribeCache &cache,
                      ref_<const SubscribeResponseMessage> message) {
                    messages.push_back(message->get_value().value.get_string());
                    cout << "Total Memory: " << message->get_value().value.get_string() << "\n";
                  });

  link->subscribe("main/Hardware_Model",
                  [&](IncomingSubscribeCache &cache,
                      ref_<const SubscribeResponseMessage> message) {
                    messages.push_back(message->get_value().value.get_string());
                    cout << "CPU Name: " << message->get_value().value.get_string() << "\n";
                  });

  link->subscribe("main/Model_Name",
                  [&](IncomingSubscribeCache &cache,
                      ref_<const SubscribeResponseMessage> message) {
                    messages.push_back(message->get_value().value.get_string());
                    cout << "Model Name: " << message->get_value().value.get_string() << "\n";
                  });

  link->subscribe("main/CPU_Frequency",
                  [&](IncomingSubscribeCache &cache,
                      ref_<const SubscribeResponseMessage> message) {
                    messages.push_back(message->get_value().value.get_string());
                    cout << "CPU Frequency: " << message->get_value().value.get_string() << "\n";
                  });


  // Cleaning test
//  destroy_dslink_in_strand(linkResp);
//  destroy_dslink_in_strand(link);

//  app->close();
////  WAIT_EXPECT_TRUE(500, [&]() -> bool { return app->is_stopped(); });
//
//  if (!app->is_stopped()) {
//    app->force_stop();
//  }
  app->wait();

  return 0;
}
