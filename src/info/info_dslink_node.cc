#include "info_dslink_node.h"

namespace dslink_info {

InfoDsLinkNodeBase::InfoDsLinkNodeBase(LinkStrandRef &&strand)
    : NodeModel(std::move(strand)) {
  _subscribe_state = false;
};

void InfoDsLinkNodeCPUUsage::update_value() {
  set_value(Var(info::cpu::get_cpu_usage()));
}

void InfoDsLinkNodeFreeMemory::update_value() {
  set_value(Var(info::system::get_memory_info()._physical_free));
};

void InfoDsLinkNodeUsedMemory::update_value() {
  set_value(Var(info::system::get_memory_info()._physical_used));
};

void InfoDsLinkNodeTotalMemory::update_value() {
  set_value(Var(info::system::get_memory_info()._physical_total));
};

void InfoDsLinkNodeDiskUsage::update_value() {
  set_value(Var(info::system::get_diskspace_info()._disk_usage));
};

void InfoDsLinkNodeFreeDiskSpace::update_value() {
  set_value(Var(info::system::get_diskspace_info()._diskspace_free));
};

void InfoDsLinkNodeUsedDiskSpace::update_value() {
  set_value(Var(info::system::get_diskspace_info()._diskspace_used));
};

void InfoDsLinkNodeTotalDiskSpace::update_value() {
  set_value(Var(info::system::get_diskspace_info()._diskspace_total));
};

void InfoDsLinkNodeHardwareModel::update_value() {
  set_value(Var(info::cpu::get_product_name()));
};

void InfoDsLinkNodeModelName::update_value() {
  set_value(Var(info::cpu::get_model_name()));
};

void InfoDsLinkNodeCPUFreq::update_value() {
  set_value(Var(info::cpu::get_frequency()));
};

InfoDsLinkNode::InfoDsLinkNode(LinkStrandRef strand)
    : NodeModel(std::move(strand)) {

  nodes["CPU_Usage"] = add_list_child(
      "CPU_Usage", make_ref_<InfoDsLinkNodeCPUUsage>(_strand->get_ref()));
  nodes["Free_Memory"] = add_list_child(
      "Free_Memory", make_ref_<InfoDsLinkNodeFreeMemory>(_strand->get_ref()));
  nodes["Used_Memory"] = add_list_child(
      "Used_Memory", make_ref_<InfoDsLinkNodeUsedMemory>(_strand->get_ref()));
  nodes["Total_Memory"] = add_list_child(
      "Total_Memory", make_ref_<InfoDsLinkNodeTotalMemory>(_strand->get_ref()));
  nodes["Disk_Usage"] = add_list_child(
      "Disk_Usage", make_ref_<InfoDsLinkNodeDiskUsage>(_strand->get_ref()));
  nodes["Free_Disk_Space"] = add_list_child(
      "Free_Disk_Space",
      make_ref_<InfoDsLinkNodeFreeDiskSpace>(_strand->get_ref()));
  nodes["Used_Disk_Space"] = add_list_child(
      "Used_Disk_Space",
      make_ref_<InfoDsLinkNodeUsedDiskSpace>(_strand->get_ref()));
  nodes["Total_Disk_Space"] = add_list_child(
      "Total_Disk_Space",
      make_ref_<InfoDsLinkNodeTotalDiskSpace>(_strand->get_ref()));
  nodes["Hardware_Model"] = add_list_child(
      "Hardware_Model",
      make_ref_<InfoDsLinkNodeHardwareModel>(_strand->get_ref()));
  nodes["Model_Name"] = add_list_child(
      "Model_Name", make_ref_<InfoDsLinkNodeModelName>(_strand->get_ref()));
  nodes["CPU_Frequency"] = add_list_child(
      "CPU_Frequency", make_ref_<InfoDsLinkNodeCPUFreq>(_strand->get_ref()));

  _strand->add_timer(1000, [ this, keep_ref = get_ref() ](bool canceled) {
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
      if (it->second->get_subs_state())
        it->second->update_value();
    }
    return true;
  });
}
}
