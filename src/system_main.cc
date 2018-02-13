#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "dslink.h"
#include "util/app.h"

#include "module/default/console_logger.h"

#include "info/info_dslink_node.h"

using namespace dsa;
using namespace dslink_info;

ref_<DsLink> system_main;
ref_<info::ProcessHandler> process;

void connect_dslink(int argc, char const *argv[], std::shared_ptr<App> app) {

  system_main = make_ref_<DsLink>(argc, argv, "sys-dslink", "1.0.0", app);

  process = make_ref_<info::ProcessHandler>("", "", "", app);

  ref_<InfoDsLinkNode> _info_node = make_ref_<InfoDsLinkNode>(
      std::move(system_main->strand->get_ref()), std::move(process->get_ref()));

  system_main->init_responder(std::move(_info_node));

  //	static_cast<ConsoleLogger &>(system_main->strand->logger()).filter =
  //		Logger::WARN__ | Logger::ERROR_ | Logger::FATAL_;

  system_main->connect([&](const shared_ptr_<Connection> connection) {
    LOG_DEBUG(Logger::_(),
              LOG << "system-dslink is connected");
  });
}

int main(int argc, char const *argv[]) {

  auto app = std::make_shared<App>();

  // Connect to the broker
  connect_dslink(argc, argv, app);

  //  auto cmdline = CmdLine(app, system_main);
  //  cmdline.run();

  app->close();
  app->wait();

  return 0;
}