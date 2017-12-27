#include "info_dslink_node.h"
#include "info.h"

namespace dslink_info {

  InfoDsLinkNodeCPUUsage::InfoDsLinkNodeCPUUsage(LinkStrandRef strand) : NodeModel(std::move(strand)) {
    set_value(Var(info::cpu::get_cpu_usage()));
  };

  InfoDsLinkNodeFreeMemory::InfoDsLinkNodeFreeMemory(LinkStrandRef strand) : NodeModel(std::move(strand)) {
    set_value(Var(info::system::get_memory_info()._physical_free));
  };

  InfoDsLinkNodeUsedMemory::InfoDsLinkNodeUsedMemory(LinkStrandRef strand) : NodeModel(std::move(strand)) {
    set_value(Var(info::system::get_memory_info()._physical_used));
  };

  InfoDsLinkNodeTotalMemory::InfoDsLinkNodeTotalMemory(LinkStrandRef strand) : NodeModel(std::move(strand)) {
    set_value(Var(info::system::get_memory_info()._physical_total));
  };

  InfoDsLinkNodeHardwareModel::InfoDsLinkNodeHardwareModel(LinkStrandRef strand) : NodeModel(std::move(strand)) {
    set_value(Var(info::cpu::get_product_name()));
  };

  InfoDsLinkNodeModelName::InfoDsLinkNodeModelName(LinkStrandRef strand) : NodeModel(std::move(strand)) {
    set_value(Var(info::cpu::get_model_name()));
  };

  InfoDsLinkNodeCPUFreq::InfoDsLinkNodeCPUFreq(LinkStrandRef strand) : NodeModel(std::move(strand)) {
    set_value(Var(info::cpu::get_frequency()));
  };


  InfoDsLinkNode::InfoDsLinkNode(LinkStrandRef strand) : NodeModel(std::move(strand)){
    add_list_child("CPU_Usage", new InfoDsLinkNodeCPUUsage(_strand));
    add_list_child("Free_Memory", new InfoDsLinkNodeFreeMemory(_strand));
    add_list_child("Used_Memory", new InfoDsLinkNodeUsedMemory(_strand));
    add_list_child("Total_Memory", new InfoDsLinkNodeTotalMemory(_strand));
    add_list_child("Hardware_Model", new InfoDsLinkNodeHardwareModel(_strand));
    add_list_child("Model_Name", new InfoDsLinkNodeModelName(_strand));
    add_list_child("CPU_Frequency", new InfoDsLinkNodeCPUFreq(_strand));
  }

}

