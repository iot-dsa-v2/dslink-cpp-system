#include "info_dslink_node.h"

namespace dslink_info {

  InfoDsLinkNodeCPUUsage::InfoDsLinkNodeCPUUsage(LinkStrandRef strand) : NodeModel(std::move(strand)) {
//    set_value(Var(info::cpu::get_cpu_usage()));
  };

  InfoDsLinkNodeFreeMemory::InfoDsLinkNodeFreeMemory(LinkStrandRef strand) : NodeModel(std::move(strand)) {
//    set_value(Var(info::system::get_memory_info()._physical_free));
  };

  InfoDsLinkNodeUsedMemory::InfoDsLinkNodeUsedMemory(LinkStrandRef strand) : NodeModel(std::move(strand)) {
//    set_value(Var(info::system::get_memory_info()._physical_used));
  };

  InfoDsLinkNodeTotalMemory::InfoDsLinkNodeTotalMemory(LinkStrandRef strand) : NodeModel(std::move(strand)) {
//    set_value(Var(info::system::get_memory_info()._physical_total));
  };

  InfoDsLinkNodeDiskUsage::InfoDsLinkNodeDiskUsage(LinkStrandRef strand) : NodeModel(std::move(strand)) {
//    set_value(Var(info::system::get_diskspace_info()._disk_usage));
  };

  InfoDsLinkNodeFreeDiskSpace::InfoDsLinkNodeFreeDiskSpace(LinkStrandRef strand) : NodeModel(std::move(strand)) {
//    set_value(Var(info::system::get_diskspace_info()._diskspace_free));
  };

  InfoDsLinkNodeUsedDiskSpace::InfoDsLinkNodeUsedDiskSpace(LinkStrandRef strand) : NodeModel(std::move(strand)) {
//    set_value(Var(info::system::get_diskspace_info()._diskspace_used));
  };

  InfoDsLinkNodeTotalDiskSpace::InfoDsLinkNodeTotalDiskSpace(LinkStrandRef strand) : NodeModel(std::move(strand)) {
//    set_value(Var(info::system::get_diskspace_info()._diskspace_total));
  };

  InfoDsLinkNodeHardwareModel::InfoDsLinkNodeHardwareModel(LinkStrandRef strand) : NodeModel(std::move(strand)) {
//    set_value(Var(info::cpu::get_product_name()));
  };

  InfoDsLinkNodeModelName::InfoDsLinkNodeModelName(LinkStrandRef strand) : NodeModel(std::move(strand)) {
//    set_value(Var(info::cpu::get_model_name()));
  };

  InfoDsLinkNodeCPUFreq::InfoDsLinkNodeCPUFreq(LinkStrandRef strand) : NodeModel(std::move(strand)) {
//    set_value(Var(info::cpu::get_frequency()));
  };

  InfoDsLinkNode::InfoDsLinkNode(LinkStrandRef strand) : NodeModel(std::move(strand)){
    add_list_child("CPU_Usage", make_ref_<InfoDsLinkNodeCPUUsage>(_strand->get_ref()));
    add_list_child("Free_Memory", make_ref_<InfoDsLinkNodeFreeMemory>(_strand->get_ref()));
    add_list_child("Used_Memory", make_ref_<InfoDsLinkNodeUsedMemory>(_strand->get_ref()));
    add_list_child("Total_Memory", make_ref_<InfoDsLinkNodeTotalMemory>(_strand->get_ref()));
    add_list_child("Disk_Usage", make_ref_<InfoDsLinkNodeDiskUsage>(_strand->get_ref()));
    add_list_child("Free_Disk_Space", make_ref_<InfoDsLinkNodeFreeDiskSpace>(_strand->get_ref()));
    add_list_child("Used_Disk_Space", make_ref_<InfoDsLinkNodeUsedDiskSpace>(_strand->get_ref()));
    add_list_child("Total_Disk_Space", make_ref_<InfoDsLinkNodeTotalDiskSpace>(_strand->get_ref()));
    add_list_child("Hardware_Model", make_ref_<InfoDsLinkNodeHardwareModel>(_strand->get_ref()));
    add_list_child("Model_Name", make_ref_<InfoDsLinkNodeModelName>(_strand->get_ref()));
    add_list_child("CPU_Frequency", make_ref_<InfoDsLinkNodeCPUFreq>(_strand->get_ref()));
  }

}

