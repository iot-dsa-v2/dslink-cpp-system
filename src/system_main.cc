#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "dslink.h"
#include "util/app.h"

#include <module/default/console_logger.h>

#include "info/info_dslink_node.h"


using namespace dsa;
using namespace dslink_info;

ref_<DsLink> system_main;

bool connect_dslink(int argc, char const *argv[], std::shared_ptr<App> app) {
  system_main = make_ref_<DsLink>(argc, argv, "sys-dslink", "1.0.0", app);
  system_main->init_responder<InfoDsLinkNode>();

  static_cast<ConsoleLogger &>(system_main->strand->logger()).filter =
      Logger::WARN__ | Logger::ERROR_ | Logger::FATAL_;

  bool is_connected = false;
  system_main->connect([&](const shared_ptr_<Connection> connection) {
    is_connected = true;
  });

  boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
  if(!is_connected){
    std::cout<<"cannot connect to the broker!"<<std::endl;
    return false;
  }

  std::cout<<"\n system-dslink is connected, you can go for it!\n"<<std::endl;

  return true;
}

int main(int argc, char const *argv[]) {

  auto app = std::make_shared<App>();

  // Connect to the broker
  if(!connect_dslink(argc, argv, app)) return 1;

//  auto cmdline = CmdLine(app, system_main);
//  cmdline.run();

  app->close();
  app->wait();

  return 0;
}