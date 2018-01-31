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
  virtual void handle_value();
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

class InfoDsLinkNodePollRate : public NodeModel {
public:
	typedef std::function<void(void)> SetCallback;
	SetCallback _set_callback;
	explicit InfoDsLinkNodePollRate(
		LinkStrandRef strand,
		SetCallback &&set_callback) // allows set value with write permission
		: NodeModel(std::move(strand), PermissionLevel::WRITE) {
		update_property("$name", Var("Poll Rate"));
		update_property("$type", Var("int"));
		set_value(Var(1));
		_set_callback = std::move(set_callback);
	};

	MessageStatus on_set_attribute(const string_ &field, Var &&value) override {
		update_property(field, std::move(value));
		return MessageStatus::CLOSED;
	}

	int get_value() { return (int)get_cached_value().value.get_int() * 1000; }
	MessageStatus on_set_value(MessageValue &&value) override {
		set_value(std::move(value));
		if (_set_callback != nullptr)
			_set_callback();
		return MessageStatus::CLOSED;
	}
};

class InfoDsLinkNodeNetworkInterfaces : public InfoDsLinkNodeBase {
	std::string _interface;

public:
	explicit InfoDsLinkNodeNetworkInterfaces(LinkStrandRef &&strand)
		: InfoDsLinkNodeBase(std::move(strand)) {}
	void update_value() override { set_value(Var(_interface)); };
	void set_interface_value(std::string interface) { _interface = interface; };
};

class InfoDsLinkNodeNetwork : public NodeModel {
	bool _subscribe_state;
	ref_<StrandTimer> network_timer;
	bool _first_call = true;
	std::map<std::string, std::string> interfaces;

protected:
	bool _dynamic = true;
	bool _is_updated = false;

public:
	std::map<string, ref_<InfoDsLinkNodeNetworkInterfaces>> network_nodes;
	typedef std::function<void(void)> SubsCallback;
	SubsCallback _subs_callback;
	void set_subs_callback(SubsCallback &&cb);
	explicit InfoDsLinkNodeNetwork(LinkStrandRef &&strand);

	void on_subscribe(const SubscribeOptions &options,
		bool first_request) override;

	void on_unsubscribe() override { _subscribe_state = false; }
	bool get_subs_state() { return _subscribe_state; }

	virtual void update_value();
	void handle_value();
};

class InfoDsLinkNode : public NodeModel {
  ref_<StrandTimer> _timer;
  ref_<StrandTimer> _command_timer;
  ref_<InfoDsLinkNodePollRate> poll_rate;
  ref_<InfoDsLinkNodeNetwork> network_interfaces;

public:
  std::map<string, ref_<InfoDsLinkNodeBase>> nodes;
  explicit InfoDsLinkNode(LinkStrandRef &&strand,
                          ref_<info::ProcessHandler> &&process);
};

}
#endif // PROJECT_INFO_DSLINK_NODE_H
