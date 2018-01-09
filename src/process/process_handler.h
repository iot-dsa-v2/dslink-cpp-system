#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/filesystem/path.hpp>

#include <boost/process.hpp>
#include <boost/process/extend.hpp>
#include <boost/process/start_dir.hpp>

#include "util/app.h"
#include "dsa/util.h"

#include <iostream>
#include <string>


#ifndef PROJECT_PROCESS_HANDLER_H
#define PROJECT_PROCESS_HANDLER_H

namespace b = boost;
namespace bp = boost::process;
namespace fs = boost::filesystem;
namespace asio = boost::asio;
namespace ex = boost::process::extend;

using namespace dsa;
namespace info {

class ProcessHandler;

struct process_callbacks : ex::handler {
  ProcessHandler *parent;

  template<typename Executor>
  void on_setup(Executor &exec) const {
  }

  template<typename Executor>
  void on_success(Executor &exec) const {
//    std::error_code ec{42, std::system_category()};
//    exec.set_error(ec, "a fake error");
  }

  template<typename Executor>
  void on_exec_error(Executor &exec, const std::error_code &error_code) {
//    std::cout << "on_error" << std::endl;
  }
};

class ProcessHandler: public EnableRef<ProcessHandler> {
 private:
  fs::path exec_path;
  fs::path log_path;
  string_ arguments;
  //bp::environment env;
  boost::asio::streambuf output;

  std::shared_ptr<App> app;
  bp::child process;
  struct process_callbacks handler;

  std::shared_ptr<boost::asio::deadline_timer> termination_timer;

 public:
  ProcessHandler(fs::path executable_path = "", string_ arguments = "", fs::path log_path = "", std::shared_ptr<App> app = nullptr);
  void set_executable_path(fs::path &path) { exec_path = fs::canonical(path); }
  void set_log_path(fs::path &path) { log_path = path; }
  std::string getOutput();

  void wait(int seconds = 0);

  virtual void start(string_ args = "");
  bool running();

  void terminate();
  void schedule_terminate(int seconds);
  int exit_code();

  fs::path working_directory();

};

}

#endif //PROJECT_PROCESS_HANDLER_H
