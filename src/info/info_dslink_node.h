#ifndef PROJECT_INFO_DSLINK_NODE_H
#define PROJECT_INFO_DSLINK_NODE_H

#include "dslink.h"
#include "dsa/message.h"
#include "dsa/stream.h"
#include "dsa/network.h"
#include "module/logger.h"
#include "dsa/responder.h"

using namespace dsa;

namespace dslink_info {

class InfoDsLinkNodeCPUUsage : public NodeModel {
 public:
  explicit InfoDsLinkNodeCPUUsage(LinkStrandRef strand);
};

class InfoDsLinkNodeFreeMemory : public NodeModel {
 public:
  explicit InfoDsLinkNodeFreeMemory(LinkStrandRef strand);
};

class InfoDsLinkNodeUsedMemory : public NodeModel {
 public:
  explicit InfoDsLinkNodeUsedMemory(LinkStrandRef strand);
};

class InfoDsLinkNodeTotalMemory : public NodeModel {
 public:
  explicit InfoDsLinkNodeTotalMemory(LinkStrandRef strand);
};

class InfoDsLinkNodeHardwareModel : public NodeModel {
 public:
  explicit InfoDsLinkNodeHardwareModel(LinkStrandRef strand);
};

class InfoDsLinkNodeModelName : public NodeModel {
 public:
  explicit InfoDsLinkNodeModelName(LinkStrandRef strand);
};

class InfoDsLinkNodeCPUFreq : public NodeModel {
 public:
  explicit InfoDsLinkNodeCPUFreq(LinkStrandRef strand);
};

class InfoDsLinkNode : public NodeModel {
 public:
  explicit InfoDsLinkNode(LinkStrandRef strand);
};

}
#endif //PROJECT_INFO_DSLINK_NODE_H
