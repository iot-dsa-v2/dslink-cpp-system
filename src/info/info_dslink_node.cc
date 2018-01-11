#include "info_dslink_node.h"
#include "process/process_handler.h"

namespace dslink_info {

InfoDsLinkNodeBase::InfoDsLinkNodeBase(LinkStrandRef &&strand)
    : NodeModel(std::move(strand)) {
  _subscribe_state = false;
};

void InfoDsLinkNodeBase::set_subs_callback(SubsCallback &&cb) {
  _subs_callback = std::move(cb);
}
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

void InfoDsLinkNodeSystemTime::update_value() {
  set_value(Var(info::system::get_system_time()));
};

void InfoDsLinkNodeOperatingSystem::update_value() {
  set_value(Var(info::system::get_OS_info().full_name));
};

void InfoDsLinkNodePlatform::update_value() {
  set_value(Var(info::system::get_kernel_info().variant));
};

void InfoDsLinkNodeHostname::update_value() {
  set_value(Var(info::system::get_hostname()));
};

void InfoDsLinkNodeArchitecture::update_value() {
  set_value(Var(info::cpu::get_architecture()));
};

void InfoDsLinkNodeProcessorCount::update_value() {
  set_value(Var(info::cpu::get_cpu_quantities().physical));
};

InfoDsLinkNode::InfoDsLinkNode(LinkStrandRef &&strand,
                               ref_<info::ProcessHandler> &&process)
    : NodeModel(std::move(strand)) {

  nodes["cpu_usage"] = add_list_child(
      "cpu_usage", make_ref_<InfoDsLinkNodeCPUUsage>(_strand->get_ref()));
  nodes["cpu_usage"]->update_property("$name", Var("CPU Usage"));
  nodes["cpu_usage"]->update_property("$type", Var("string"));

  nodes["free_memory"] = add_list_child(
      "free_memory", make_ref_<InfoDsLinkNodeFreeMemory>(_strand->get_ref()));
  nodes["free_memory"]->update_property("$name", Var("Free Memory"));
  nodes["free_memory"]->update_property("$type", Var("string"));

  nodes["used_memory"] = add_list_child(
      "used_memory", make_ref_<InfoDsLinkNodeUsedMemory>(_strand->get_ref()));
  nodes["used_memory"]->update_property("$name", Var("Used Memory"));
  nodes["used_memory"]->update_property("$type", Var("string"));

  nodes["total_memory"] = add_list_child(
      "total_memory", make_ref_<InfoDsLinkNodeTotalMemory>(_strand->get_ref()));
  nodes["total_memory"]->update_property("$name", Var("Total Memory"));
  nodes["total_memory"]->update_property("$type", Var("string"));

  nodes["disk_usage"] = add_list_child(
      "disk_usage", make_ref_<InfoDsLinkNodeDiskUsage>(_strand->get_ref()));
  nodes["disk_usage"]->update_property("$name", Var("Disk Usage"));
  nodes["disk_usage"]->update_property("$type", Var("string"));

  nodes["free_disk_space"] = add_list_child(
      "free_disk_space",
      make_ref_<InfoDsLinkNodeFreeDiskSpace>(_strand->get_ref()));
  nodes["free_disk_space"]->update_property("$name", Var("Free Disk Space"));
  nodes["free_disk_space"]->update_property("$type", Var("string"));

  nodes["used_disk_space"] = add_list_child(
      "used_disk_space",
      make_ref_<InfoDsLinkNodeUsedDiskSpace>(_strand->get_ref()));
  nodes["used_disk_space"]->update_property("$name", Var("Used Disk Space"));
  nodes["used_disk_space"]->update_property("$type", Var("string"));

  nodes["total_disk_space"] = add_list_child(
      "total_disk_space",
      make_ref_<InfoDsLinkNodeTotalDiskSpace>(_strand->get_ref()));
  nodes["total_disk_space"]->update_property("$name", Var("Total Disk Space"));
  nodes["total_disk_space"]->update_property("$type", Var("string"));

  nodes["hardware_model"] = add_list_child(
      "hardware_model",
      make_ref_<InfoDsLinkNodeHardwareModel>(_strand->get_ref()));
  nodes["hardware_model"]->update_property("$name", Var("Hardware Model"));
  nodes["hardware_model"]->update_property("$type", Var("string"));

  nodes["model_name"] = add_list_child(
      "model_name", make_ref_<InfoDsLinkNodeModelName>(_strand->get_ref()));
  nodes["model_name"]->update_property("$name", Var("Model Name"));
  nodes["model_name"]->update_property("$type", Var("string"));

  nodes["cpu_frequency"] = add_list_child(
      "cpu_frequency", make_ref_<InfoDsLinkNodeCPUFreq>(_strand->get_ref()));
  nodes["cpu_frequency"]->update_property("$name", Var("CPU Frequency"));
  nodes["cpu_frequency"]->update_property("$type", Var("string"));

  nodes["system_time"] = add_list_child(
      "system_time", make_ref_<InfoDsLinkNodeSystemTime>(_strand->get_ref()));
  nodes["system_time"]->update_property("$name", Var("System Time"));
  nodes["system_time"]->update_property("$type", Var("string"));

  nodes["operating_system"] = add_list_child(
      "operating_system",
      make_ref_<InfoDsLinkNodeOperatingSystem>(_strand->get_ref()));
  nodes["operating_system"]->update_property("$name", Var("Operating System"));
  nodes["operating_system"]->update_property("$type", Var("string"));

  nodes["platform"] = add_list_child(
      "platform", make_ref_<InfoDsLinkNodePlatform>(_strand->get_ref()));
  nodes["platform"]->update_property("$name", Var("Platform Info"));
  nodes["platform"]->update_property("$type", Var("string"));

  nodes["hostname"] = add_list_child(
      "hostname", make_ref_<InfoDsLinkNodeHostname>(_strand->get_ref()));
  nodes["hostname"]->update_property("$name", Var("Hostname"));
  nodes["hostname"]->update_property("$type", Var("string"));

  nodes["architecture"] =
      add_list_child("architecture",
                     make_ref_<InfoDsLinkNodeArchitecture>(_strand->get_ref()));
  nodes["architecture"]->update_property("$name", Var("Architecture"));
  nodes["architecture"]->update_property("$type", Var("string"));

  nodes["processor_count"] = add_list_child(
      "processor_count",
      make_ref_<InfoDsLinkNodeProcessorCount>(_strand->get_ref()));
  nodes["processor_count"]->update_property("$name", Var("Processor Count"));
  nodes["processor_count"]->update_property("$type", Var("string"));

  ref_<NodeModel> exe_cmd_node = add_list_child(
      "execute_command",
      make_ref_<SimpleInvokeNode>(
          _strand->get_ref(),
          [=](Var &&v) {
            string_ ret_str = "";
            if (v.is_map() && (v.get_map().find("cmd") != v.get_map().end())) {
              if (v["cmd"].get_type() == Var::STRING) {
                process->start(v["cmd"].get_string());
                process->wait();
                ret_str = process->getOutput();
              }
            }
            return Var{{"output", Var(ret_str)}};
          },
          PermissionLevel::CONFIG));
  exe_cmd_node->update_property("$name", Var("Execute command"));
  exe_cmd_node->update_property(
      "$params", Var{{{"$name", Var("cmd")}, {"$type", Var("string")}}});
  exe_cmd_node->update_property("$columns",
                                Var{{{"$name", Var("output")},
                                     {"$type", Var("string")},
                                     {"$editor", Var("textarea")}}});

  ref_<NodeModel> exe_stream_cmd_node = add_list_child(
      "execute_stream_command",
      make_ref_<SimpleInvokeNode>(
          _strand->get_ref(),
          [=](Var &&v, SimpleInvokeNode &node, OutgoingInvokeStream &stream) {

            if (v.is_map() && (v.get_map().find("cmd") != v.get_map().end())) {
              if (v["cmd"].get_type() == Var::STRING) {
                process->start(v["cmd"].get_string());

                _command_timer = _strand->add_timer(
                    1000, [&, this, keep_ref = get_ref() ](bool canceled) {

                      if (!stream.is_destroyed()) {

                        std::string ret_str = "";
                        ret_str = process->getOutput();
                        if (!ret_str.empty()) {
                          auto response = make_ref_<InvokeResponseMessage>();
                          response->set_value(Var{{"output", Var(ret_str)}});
                          stream.send_response(std::move(response));
                        }
                        return true;
                      } else {
                        return false;
                      }
                    });
              }
            } else if (v.is_null() && stream.is_destroyed()) {
              // stream closed
              process->terminate();
              _command_timer->destroy();
            }
          },
          PermissionLevel::CONFIG));
  exe_stream_cmd_node->update_property("$name", Var("Execute command stream"));
  exe_stream_cmd_node->update_property(
      "$params", Var{{{"$name", Var("cmd")}, {"$type", Var("string")}}});
  exe_stream_cmd_node->update_property("$columns",
                                       Var{{{"$name", Var("output")},
                                            {"$type", Var("string")},
                                            {"$editor", Var("table")}}});
  // TODO: how to send/config table output for stream response?

  for (auto &node : nodes) {
    node.second->set_subs_callback([&]() {
      //      _timer->restart(1000);

      if (_timer.get() == nullptr ||
          (_timer.get() != nullptr && !_timer->is_running())) {
        if (_timer.get() != nullptr)
          _timer->destroy();
        _timer = _strand->add_timer(
            1000, [ this, keep_ref = get_ref() ](bool canceled) {

              bool any_subs = false;
              for (auto it = nodes.begin(); it != nodes.end(); ++it) {
                if (it->second->get_subs_state()) {
                  it->second->update_value();
                  any_subs = true;
                }
              }

              return any_subs;
            });
      } else {
        // timer already running
      }
    });
  }
}
}
