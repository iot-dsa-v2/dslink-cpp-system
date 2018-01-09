#include "process_handler.h"
#include "boost/asio/strand.hpp"
#include "module/logger.h"
#include "process_handler.h"
#include <boost/asio/buffers_iterator.hpp>

using namespace dsa;
namespace info {

ProcessHandler::ProcessHandler(fs::path executable_path, string_ arguments,
                               fs::path log_path, std::shared_ptr<App> app) {
  this->exec_path = fs::canonical(executable_path);
  this->arguments = arguments;
  this->log_path = log_path;
  this->app = app;
  // this->env = boost::this_process::environment();

  if (this->app == nullptr)
    this->app = make_shared_<App>();

  this->handler.parent = this;
};

void ProcessHandler::wait(int seconds) {
  if (seconds == 0)
    this->process.wait();
  else
    this->process.wait_for(std::chrono::seconds(seconds));
}

void ProcessHandler::start(string_ args) {
  if (running()) {
    LOG_WARN(Logger::_(), LOG << "It is already running, ignoring start()...");
    return;
  }
  // if(args != "") { this->arguments = args; }
  auto exec_str = this->exec_path.string();
  if (args != "") {
    exec_str = args;
  }
  if (arguments != "")
    exec_str = exec_str + " " + arguments;
  this->process = bp::child(
      exec_str, // executable
      bp::std_out > output,
      bp::start_dir =
          this->working_directory(), // Work on with executable directory
      handler, app->io_service());
}

bool ProcessHandler::running() { return this->process.running(); }

std::string ProcessHandler::getOutput() {
  using boost::asio::buffers_begin;

  if(output.size() == 0)
    return "";
  auto bufs = output.data();
  std::string result(buffers_begin(bufs), buffers_begin(bufs) + output.size());
  output.consume(output.size());
  return result;
}

void ProcessHandler::terminate() {
//  this->process.terminate();
//  this->process.wait();
// TODO: make os independent
  kill(this->process.id(), SIGKILL);
}

void ProcessHandler::schedule_terminate(int seconds) {
  if (termination_timer != nullptr) {
    LOG_WARN(Logger::_(),
             LOG << "Only one termination can be scheculed! Ignoring..."
                 << exec_path);
  }
  termination_timer =
      make_shared_<boost::asio::deadline_timer>(app->io_service());

  termination_timer->expires_from_now(boost::posix_time::seconds(seconds));

  termination_timer->async_wait([&](const boost::system::error_code &error) {
    if (this->running()) {
      LOG_DEBUG(Logger::_(),
                LOG << "Process couldn't be closed cleanly: " << exec_path);
      this->terminate();
    } else {
      LOG_DEBUG(Logger::_(), LOG << "Process closed CLEANLY: " << exec_path);
    }
  });
}

int ProcessHandler::exit_code() { return this->process.exit_code(); }

fs::path ProcessHandler::working_directory() { return exec_path.parent_path(); }
}