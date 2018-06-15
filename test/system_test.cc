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
    const char *argv[] = {"./testResp", "--broker", "ds://127.0.0.1:4125",
                          "-l", "info", "--thread",
                          "4", "--server-port", "4185"};
    int argc = 9;
    auto linkResp = make_ref_<DsLink>(argc, argv, "mydslink", "1.0.0", app);

    // filter log for unit test
    static_cast<ConsoleLogger &>(Logger::_()).filter =
            Logger::FATAL_ | Logger::ERROR_ | Logger::WARN__;

    ref_<InfoDsLinkNode> _info_node = make_ref_<InfoDsLinkNode>(
            linkResp->strand->get_ref(), std::move(process));

    linkResp->init_responder(std::move(_info_node));

    //  linkResp->init_responder<InfoDsLinkNode>();
    linkResp->connect([&](const shared_ptr_<Connection> connection, ref_<DsLinkRequester> link_req) {});

    // Create link
    std::string address = std::string("127.0.0.1:") + std::to_string(4185);
    const char *argv2[] = {"./test", "-b", address.c_str()};
    int argc2 = 3;
    auto link = make_ref_<DsLink>(argc2, argv2, "mydslink", "1.0.0", app);

    // filter log for unit test
    static_cast<ConsoleLogger &>(Logger::_()).filter =
            Logger::FATAL_ | Logger::ERROR_ | Logger::WARN__;

    bool listed_1 = false;
    bool subscribed_1 = false;
    bool subscribed_2 = false;
    bool invoke_1 = false;

    std::vector<string_> list_result;

    std::vector<std::string> messages;
    // connection
    bool is_connected = false;
    link->connect(
            [&](const shared_ptr_<Connection> connection, ref_<DsLinkRequester> link_req) {

                if (!listed_1) {
                    // List test
                    link_req->list("", [&](IncomingListCache &cache,
                                           const std::vector<string_> &str) {
                        if (!listed_1) {
                            list_result = str;
                            listed_1 = true;
                        }
                    });
                }
                if (!subscribed_1) {
                    link_req->subscribe(
                            "Main/Processor_Temp", [&](IncomingSubscribeCache &cache,
                                                       ref_<const SubscribeResponseMessage> message) {
                                messages.push_back(message->get_value().value.get_string());
                                cout << "CPU Temp: " << message->get_value().value.get_string()
                                     << "\n";
                                subscribed_1 = true;

                                EXPECT_TRUE(messages.size() > 0);
                            });
                }
                if (!subscribed_2) {
                    link_req->subscribe(
                            "Main/Cpu_Frequency", [&](IncomingSubscribeCache &cache,
                                                      ref_<const SubscribeResponseMessage> message) {
                                messages.push_back(message->get_value().value.get_string());
                                cout << "CPU Frequency: " << message->get_value().value.get_string()
                                     << "\n";
                                subscribed_2 = true;

                                EXPECT_TRUE(messages.size() > 0);
                            });
                }

                auto first_request = make_ref_<InvokeRequestMessage>();
                first_request->set_target_path("Main/Execute_Stream_Command");
                first_request->set_value(Var{{"cmd", Var("ping google.com")}});

                auto invoke_stream = link_req->invoke(
                        [&](IncomingInvokeStream &stream,
                            ref_<const InvokeResponseMessage> &&msg) {
                            if ((msg != nullptr) && msg->get_value().is_map())
                                std::cout
                                        << std::endl
                                        << msg->get_value().get_map()["output"].to_string()
                                        << std::endl;
                            EXPECT_TRUE((msg != nullptr) && msg->get_value().is_map());
                            invoke_1 = true;
                        },
                        copy_ref_(first_request));

                is_connected = true;

            });

    WAIT_EXPECT_TRUE(5000,
                     [&]() -> bool { return (is_connected && listed_1 && subscribed_1 && subscribed_2 && invoke_1); });

    // Cleaning test
    //  destroy_dslink_in_strand(linkResp);
    //  destroy_dslink_in_strand(link);


    link->strand->dispatch([link]() { link->destroy(); });
    WAIT_EXPECT_TRUE(3000, [&]() -> bool { return link->is_destroyed(); });
    linkResp->strand->dispatch([linkResp]() { linkResp->destroy(); });
    WAIT_EXPECT_TRUE(3000, [&]() -> bool { return linkResp->is_destroyed(); });

    app->close();
    WAIT_EXPECT_TRUE(1000, [&]() -> bool { return app->is_stopped(); });

    if (!app->is_stopped()) {
        app->force_stop();
    }
    app->wait();
}
