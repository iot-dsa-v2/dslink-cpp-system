#ifndef PROJECT_INFO_DSLINK_NODE_H
#define PROJECT_INFO_DSLINK_NODE_H

#include "dsa/message.h"
#include "dsa/network.h"
#include "dsa/responder.h"
#include "dsa/stream.h"
#include "dslink.h"
#include "info.h"
#include "module/logger.h"

#include <iostream>

using namespace std;

using namespace dsa;

namespace dslink_info {

class InfoDsLinkNodeBase : public NodeModel {
  bool _subscribe_state;

public:
  explicit InfoDsLinkNodeBase(LinkStrandRef &&strand);

  void on_subscribe(const SubscribeOptions &options,
                    bool first_request) override {
    _subscribe_state = true;
  }

  void on_unsubscribe() override { _subscribe_state = false; }
  bool get_subs_state() { return _subscribe_state; }

  virtual void update_value() {}
};

class InfoDsLinkNodeCPUUsage : public InfoDsLinkNodeBase {
public:
  explicit InfoDsLinkNodeCPUUsage(LinkStrandRef &&strand)
      : InfoDsLinkNodeBase(std::move(strand)){};
  void update_value() override;
};
class InfoDsLinkNodeFreeMemory : public InfoDsLinkNodeBase {
public:
  explicit InfoDsLinkNodeFreeMemory(LinkStrandRef &&strand)
      : InfoDsLinkNodeBase(std::move(strand)){};
  void update_value() override;
};
class InfoDsLinkNodeUsedMemory : public InfoDsLinkNodeBase {
public:
  explicit InfoDsLinkNodeUsedMemory(LinkStrandRef &&strand)
      : InfoDsLinkNodeBase(std::move(strand)){};
  void update_value() override;
};
class InfoDsLinkNodeTotalMemory : public InfoDsLinkNodeBase {
public:
  explicit InfoDsLinkNodeTotalMemory(LinkStrandRef &&strand)
      : InfoDsLinkNodeBase(std::move(strand)){};
  void update_value() override;
};
class InfoDsLinkNodeDiskUsage : public InfoDsLinkNodeBase {
public:
  explicit InfoDsLinkNodeDiskUsage(LinkStrandRef &&strand)
      : InfoDsLinkNodeBase(std::move(strand)){};
  void update_value() override;
};
class InfoDsLinkNodeFreeDiskSpace : public InfoDsLinkNodeBase {
public:
  explicit InfoDsLinkNodeFreeDiskSpace(LinkStrandRef &&strand)
      : InfoDsLinkNodeBase(std::move(strand)){};
  void update_value() override;
};
class InfoDsLinkNodeUsedDiskSpace : public InfoDsLinkNodeBase {
public:
  explicit InfoDsLinkNodeUsedDiskSpace(LinkStrandRef &&strand)
      : InfoDsLinkNodeBase(std::move(strand)){};
  void update_value() override;
};
class InfoDsLinkNodeTotalDiskSpace : public InfoDsLinkNodeBase {
public:
  explicit InfoDsLinkNodeTotalDiskSpace(LinkStrandRef &&strand)
      : InfoDsLinkNodeBase(std::move(strand)){};
  void update_value() override;
};
class InfoDsLinkNodeHardwareModel : public InfoDsLinkNodeBase {
public:
  explicit InfoDsLinkNodeHardwareModel(LinkStrandRef &&strand)
      : InfoDsLinkNodeBase(std::move(strand)){};
  void update_value() override;
};
class InfoDsLinkNodeModelName : public InfoDsLinkNodeBase {
public:
  explicit InfoDsLinkNodeModelName(LinkStrandRef &&strand)
      : InfoDsLinkNodeBase(std::move(strand)){};
  void update_value() override;
};
class InfoDsLinkNodeCPUFreq : public InfoDsLinkNodeBase {
public:
  explicit InfoDsLinkNodeCPUFreq(LinkStrandRef &&strand)
      : InfoDsLinkNodeBase(std::move(strand)){};
  void update_value() override;
};

class InfoDsLinkNode : public NodeModel {
public:
  std::map<string, ref_<InfoDsLinkNodeBase>> nodes;
  explicit InfoDsLinkNode(LinkStrandRef strand);
};
}
#endif // PROJECT_INFO_DSLINK_NODE_H