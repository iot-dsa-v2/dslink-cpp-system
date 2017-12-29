#ifndef PROJECT_INFO_DSLINK_NODE_H
#define PROJECT_INFO_DSLINK_NODE_H

#include "dslink.h"
#include "dsa/message.h"
#include "dsa/stream.h"
#include "dsa/network.h"
#include "module/logger.h"
#include "dsa/responder.h"
#include "info.h"

#include <iostream>

using namespace std;

using namespace dsa;

namespace dslink_info {

class InfoDsLinkNodeCPUUsage : public NodeModel {
 public:
  std::unique_ptr<SubscribeOptions> first_subscribe_options;
  std::unique_ptr<SubscribeOptions> second_subscribe_options;
  explicit InfoDsLinkNodeCPUUsage(LinkStrandRef strand);

  void on_subscribe(const SubscribeOptions &options,
                    bool first_request) override {
    first_subscribe_options.reset(new SubscribeOptions(options));
    if (first_request) {
      _strand->add_timer(1000, [ this, keep_ref = get_ref() ](bool canceled) {
        set_value(Var(info::cpu::get_cpu_usage()));
        return true;
      });
    } else {
      set_value(Var("world"));
    }
  }
};

class InfoDsLinkNodeFreeMemory : public NodeModel {
 public:
  std::unique_ptr<SubscribeOptions> first_subscribe_options;
  std::unique_ptr<SubscribeOptions> second_subscribe_options;
  explicit InfoDsLinkNodeFreeMemory(LinkStrandRef strand);

  void on_subscribe(const SubscribeOptions &options,
                    bool first_request) override {
    first_subscribe_options.reset(new SubscribeOptions(options));
    if (first_request) {
      _strand->add_timer(1000, [ this, keep_ref = get_ref() ](bool canceled) {
        set_value(Var(info::system::get_memory_info()._physical_free));
        return true;
      });
    } else {
      set_value(Var("world"));
    }
  }
};

class InfoDsLinkNodeUsedMemory : public NodeModel {
 public:
  std::unique_ptr<SubscribeOptions> first_subscribe_options;
  std::unique_ptr<SubscribeOptions> second_subscribe_options;
  explicit InfoDsLinkNodeUsedMemory(LinkStrandRef strand);

  void on_subscribe(const SubscribeOptions &options,
                    bool first_request) override {
    first_subscribe_options.reset(new SubscribeOptions(options));
    if (first_request) {
      _strand->add_timer(1000, [ this, keep_ref = get_ref() ](bool canceled) {
        set_value(Var(info::system::get_memory_info()._physical_used));
        return true;
      });
    } else {
      set_value(Var("world"));
    }
  }
};

class InfoDsLinkNodeTotalMemory : public NodeModel {
 public:
  std::unique_ptr<SubscribeOptions> first_subscribe_options;
  std::unique_ptr<SubscribeOptions> second_subscribe_options;
  explicit InfoDsLinkNodeTotalMemory(LinkStrandRef strand);

  void on_subscribe(const SubscribeOptions &options,
                    bool first_request) override {
    first_subscribe_options.reset(new SubscribeOptions(options));
    if (first_request) {
      _strand->add_timer(1000, [ this, keep_ref = get_ref() ](bool canceled) {
        set_value(Var(info::system::get_memory_info()._physical_total));
        return true;
      });
    } else {
      set_value(Var("world"));
    }
  }
};

class InfoDsLinkNodeDiskUsage : public NodeModel {
 public:
  std::unique_ptr<SubscribeOptions> first_subscribe_options;
  std::unique_ptr<SubscribeOptions> second_subscribe_options;
  explicit InfoDsLinkNodeDiskUsage(LinkStrandRef strand);

  void on_subscribe(const SubscribeOptions &options,
                    bool first_request) override {
    first_subscribe_options.reset(new SubscribeOptions(options));
    if (first_request) {
      _strand->add_timer(1000, [ this, keep_ref = get_ref() ](bool canceled) {
        set_value(Var(info::system::get_diskspace_info()._disk_usage));
        return true;
      });
    } else {
      set_value(Var("world"));
    }
  }
};

class InfoDsLinkNodeFreeDiskSpace : public NodeModel {
 public:
  std::unique_ptr<SubscribeOptions> first_subscribe_options;
  std::unique_ptr<SubscribeOptions> second_subscribe_options;
  explicit InfoDsLinkNodeFreeDiskSpace(LinkStrandRef strand);

  void on_subscribe(const SubscribeOptions &options,
                    bool first_request) override {
    first_subscribe_options.reset(new SubscribeOptions(options));
    if (first_request) {
      _strand->add_timer(1000, [ this, keep_ref = get_ref() ](bool canceled) {
        set_value(Var(info::system::get_diskspace_info()._diskspace_free));
        return true;
      });
    } else {
      set_value(Var("world"));
    }
  }
};

class InfoDsLinkNodeUsedDiskSpace : public NodeModel {
 public:
  std::unique_ptr<SubscribeOptions> first_subscribe_options;
  std::unique_ptr<SubscribeOptions> second_subscribe_options;
  explicit InfoDsLinkNodeUsedDiskSpace(LinkStrandRef strand);

  void on_subscribe(const SubscribeOptions &options,
                    bool first_request) override {
    first_subscribe_options.reset(new SubscribeOptions(options));
    if (first_request) {
      _strand->add_timer(1000, [ this, keep_ref = get_ref() ](bool canceled) {
        set_value(Var(info::system::get_diskspace_info()._diskspace_used));
        return true;
      });
    } else {
      set_value(Var("world"));
    }
  }
};

class InfoDsLinkNodeTotalDiskSpace: public NodeModel {
 public:
  std::unique_ptr<SubscribeOptions> first_subscribe_options;
  std::unique_ptr<SubscribeOptions> second_subscribe_options;
  explicit InfoDsLinkNodeTotalDiskSpace(LinkStrandRef strand);

  void on_subscribe(const SubscribeOptions &options,
                    bool first_request) override {
    first_subscribe_options.reset(new SubscribeOptions(options));
    if (first_request) {
      _strand->add_timer(1000, [ this, keep_ref = get_ref() ](bool canceled) {
        set_value(Var(info::system::get_diskspace_info()._diskspace_total));
        return true;
      });
    } else {
      set_value(Var("world"));
    }
  }
};

class InfoDsLinkNodeHardwareModel : public NodeModel {
 public:
  std::unique_ptr<SubscribeOptions> first_subscribe_options;
  std::unique_ptr<SubscribeOptions> second_subscribe_options;
  explicit InfoDsLinkNodeHardwareModel(LinkStrandRef strand);

  void on_subscribe(const SubscribeOptions &options,
                    bool first_request) override {
    first_subscribe_options.reset(new SubscribeOptions(options));
    if (first_request) {
      _strand->add_timer(1000, [ this, keep_ref = get_ref() ](bool canceled) {
        set_value(Var(info::cpu::get_product_name()));
        return true;
      });
    } else {
      set_value(Var("world"));
    }
  }
};

class InfoDsLinkNodeModelName : public NodeModel {
 public:
  std::unique_ptr<SubscribeOptions> first_subscribe_options;
  std::unique_ptr<SubscribeOptions> second_subscribe_options;
  explicit InfoDsLinkNodeModelName(LinkStrandRef strand);

  void on_subscribe(const SubscribeOptions &options,
                    bool first_request) override {
    first_subscribe_options.reset(new SubscribeOptions(options));
    if (first_request) {
      _strand->add_timer(1000, [ this, keep_ref = get_ref() ](bool canceled) {
        set_value(Var(info::cpu::get_model_name()));
        return true;
      });
    } else {
      set_value(Var("world"));
    }
  }
};

class InfoDsLinkNodeCPUFreq : public NodeModel {
 public:
  std::unique_ptr<SubscribeOptions> first_subscribe_options;
  std::unique_ptr<SubscribeOptions> second_subscribe_options;
  explicit InfoDsLinkNodeCPUFreq(LinkStrandRef strand);

  void on_subscribe(const SubscribeOptions &options,
                    bool first_request) override {
    first_subscribe_options.reset(new SubscribeOptions(options));
    if (first_request) {
      _strand->add_timer(1000, [ this, keep_ref = get_ref() ](bool canceled) {
        set_value(Var(info::cpu::get_frequency()));
        return true;
      });
    } else {
      set_value(Var("world"));
    }
  }
};

class InfoDsLinkNode : public NodeModel {
 public:
  explicit InfoDsLinkNode(LinkStrandRef strand);
};

}
#endif //PROJECT_INFO_DSLINK_NODE_H
