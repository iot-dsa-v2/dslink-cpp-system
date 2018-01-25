#ifndef PROJECT_INFO_DSLINK_NODE_H
#define PROJECT_INFO_DSLINK_NODE_H

#include "dsa/message.h"
#include "dsa/network.h"
#include "dsa/responder.h"
#include "dsa/stream.h"
#include "dslink.h"
#include "info.h"
#include "module/logger.h"

#include "process/process_handler.h"

#include <iostream>

using namespace std;

using namespace dsa;

namespace dslink_info {

class InfoDsLinkNodeBase : public NodeModel {
  bool _subscribe_state;

 protected:
  bool _dynamic = true;
  bool _is_updated = false;

public:
  typedef std::function<void(void)> SubsCallback;
  SubsCallback _subs_callback;
  void set_subs_callback(SubsCallback &&cb);
  explicit InfoDsLinkNodeBase(LinkStrandRef &&strand);

  void on_subscribe(const SubscribeOptions &options,
                    bool first_request) override {
    if (_subs_callback)
      _subs_callback();
    _subscribe_state = true;
  }

  void on_unsubscribe() override { _subscribe_state = false; }
  bool get_subs_state() { return _subscribe_state; }

  virtual void update_value() {}
  void handle_value();
};

template <class T> class InfoDsLinkNodeTemplate : public InfoDsLinkNodeBase {
  T _f_ptr;

public:
  explicit InfoDsLinkNodeTemplate(LinkStrandRef &&strand, T f_ptr)
      : InfoDsLinkNodeBase(std::move(strand)) {
    _f_ptr = f_ptr;
  }
  void update_value() override { set_value(Var((_f_ptr)())); };
};

class InfoDsLinkNode : public NodeModel {
  ref_<StrandTimer> _timer;
  ref_<StrandTimer> _command_timer;

public:
  std::map<string, ref_<InfoDsLinkNodeBase>> nodes;
  explicit InfoDsLinkNode(LinkStrandRef &&strand,
                          ref_<info::ProcessHandler> &&process);
};
}
#endif // PROJECT_INFO_DSLINK_NODE_H
