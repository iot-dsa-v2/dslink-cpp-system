#ifdef __linux__
#include "info.h"
#include <cstring>
#include <sys/utsname.h>
#include <fstream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <boost/filesystem.hpp>
#include <sstream>
#include <chrono>
#include <thread>
#include <iterator>
#include <ctime>

std::string info::cpu::get_architecture() {
  utsname buf;

  if(uname(&buf) == -1)
    return "Unknown";
  else
    return buf.machine;

//  if(!strcmp(buf.machine, "x86_64"))
//    return "x86_64";
//  else if(strstr(buf.machine, "arm") == buf.machine)
//    return "arm";
//  else if(!strcmp(buf.machine, "ia64") || !strcmp(buf.machine, "IA64"))
//    return "IA64";
//  else if(!strcmp(buf.machine, "i686"))
//    return "i686";
//  else
//    return "Unknown";
}


std::string info::cpu::get_frequency() {
  std::ifstream cpuinfo("/proc/cpuinfo");

  if(!cpuinfo.is_open() || !cpuinfo)
    return 0;

  for(std::string line; std::getline(cpuinfo, line);)
    if(line.find("cpu MHz") == 0) {
      const auto colon_id = line.find_first_of(':');
      std::string frequency = line.c_str() + colon_id + 1;
      return (frequency + " MHz");
    }

  return 0;
}

info::cpu::quantities_t info::cpu::get_cpu_quantities() {
  info::cpu::quantities_t ret{};
  //ret.logical = sysconf(_SC_NPROCESSORS_ONLN);

  std::ifstream cpuinfo("/proc/cpuinfo");

  if(!cpuinfo.is_open() || !cpuinfo)
    return ret;

  std::vector<unsigned int> package_ids;
  for(std::string line; std::getline(cpuinfo, line);)
    if(line.find("physical id") == 0)
      package_ids.emplace_back(std::strtoul(line.c_str() + line.find_first_of("1234567890"), nullptr, 10));
    else if(line.find("processor") == 0)
      ++ret.logical;
    else if(line.find("cpu cores") == 0)
      ret.physical = line.c_str() + line.find_first_of("1234567890");


  std::sort(package_ids.begin(), package_ids.end());
  package_ids.erase(std::unique(package_ids.begin(), package_ids.end()), package_ids.end());
  ret.packages = package_ids.size();

  return ret;
}

std::string info::cpu::get_product_name() {

  std::string sLine;
  std::ifstream file("/sys/devices/virtual/dmi/id/product_name");

  getline(file, sLine);

  if (sLine == "To be filled by O.E.M.") {
    std::ifstream file("/sys/devices/virtual/dmi/id/product_name");
    getline(file, sLine);
  }
  return (sLine);
}

static std::string cpuinfo_value(const char* key) {
  std::ifstream cpuinfo("/proc/cpuinfo");

  if(!cpuinfo.is_open() || !cpuinfo)
    return 0;

  for(std::string line; std::getline(cpuinfo, line);)
    if(line.find(key) == 0) {
      const auto colon_id    = line.find_first_of(':');
      const auto nonspace_id = line.find_first_not_of(" \t", colon_id) + 1;
      return line.c_str() + nonspace_id;
    }

  return {};
}


std::string info::cpu::get_vendor() {
  return cpuinfo_value("vendor");
}

std::string info::cpu::get_model_name() {
  return cpuinfo_value("model name");
}

info::system::OS_info_t info::system::get_OS_info() {
  std::ifstream release("/etc/lsb-release");

  if(!release.is_open() || !release)
    return {};

  info::system::OS_info_t ret{};

  for(std::string line; std::getline(release, line);)
    if(line.find("DISTRIB_ID") == 0)
      ret.name = line.substr(line.find('=') + 1);
    else if(line.find("DISTRIB_RELEASE") == 0) {
      char* marker     = &line[line.find('=') + 1];
      ret.major        = std::strtoul(marker, &marker, 10);
      ret.minor        = std::strtoul(marker + 1, &marker, 10);
      ret.patch        = std::strtoul(marker + 1, &marker, 10);
      ret.build_number = std::strtoul(marker + 1, nullptr, 10);
    } else if(line.find("DISTRIB_DESCRIPTION") == 0) {
      const auto start_idx = line.find('"') + 1;
      const auto end_idx   = line.size() - 1;
      ret.full_name        = line.substr(start_idx, end_idx - start_idx);
    }

  return ret;
}

info::system::kernel_info_t info::system::get_kernel_info() {
  utsname uts;
  uname(&uts);

  char* marker                    = uts.release;
  const unsigned int major        = std::strtoul(marker, &marker, 10);
  const unsigned int minor        = std::strtoul(marker + 1, &marker, 10);
  const unsigned int patch        = std::strtoul(marker + 1, &marker, 10);
  const unsigned int build_number = std::strtoul(marker + 1, nullptr, 10);

  std::string kernel = "Unknown";
  if(!std::strcmp(uts.sysname, "Linux"))
    kernel = "Linux";
  else if(!std::strcmp(uts.sysname, "Darwin"))
    kernel = "Darwin";

  return {kernel, major, minor, patch, build_number};
}

info::system::memory_t info::system::get_memory_info() {
  std::uint64_t physical_used;
  std::uint64_t physical_available;
  std::uint64_t physical_free;
  std::uint64_t physical_total;
  std::ifstream meminfo("/proc/meminfo");

  if(!meminfo.is_open() || !meminfo)
    return {};

  info::system::memory_t ret;
  for(std::string line; std::getline(meminfo, line);) {

    const auto colon_id = line.find_first_of(':');
    const auto value    = std::strtoul(line.c_str() + colon_id + 1, nullptr, 10) * 1024;

    if(line.find("MemTotal") == 0)
      physical_total = value;
    else if(line.find("MemFree") == 0)
      physical_free = value;
    else if(line.find("MemAvailable") == 0)
      physical_available = value;
    else if(line.find("Cached") == 0)
      physical_used = physical_total - physical_free - value;

  }

  ret._physical_total = std::to_string(physical_total/1024/1024) + " mb";
  ret._physical_available = std::to_string(physical_available/1024/1024) + " mb";
  ret._physical_free = std::to_string(physical_free/1024/1024) + " mb";
  ret._physical_used = std::to_string(physical_used/1024/1024) + " mb";

  return ret;
}

info::system::diskspace_t info::system::get_diskspace_info() {

  info::system::diskspace_t ret;
  boost::filesystem::space_info _space_info = boost::filesystem::space(".");

  ret._disk_usage = std::to_string(_space_info.capacity/1024/1024) + " MB";
  ret._diskspace_total = std::to_string(_space_info.capacity/1024/1024) + " MB";
  ret._diskspace_available = std::to_string(_space_info.available/1024/1024) + " MB";
  ret._diskspace_free = std::to_string(_space_info.free/1024/1024) + " MB";
  ret._diskspace_used = std::to_string((_space_info.capacity - _space_info.available)/1024/1024) + " MB";

  return ret;
}

std::string info::system::get_system_time() {

  time_t now = time(0);

  return ctime(&now);
}

std::string info::system::get_hostname() {

  char hostname[1024];
  gethostname(hostname, 1024);

  return hostname;
}

std::vector<info::cpu::CPUData> info::cpu::ReadStatsCPU() {

  std::ifstream fileStat("/proc/stat");

  std::string line;

  std::vector<info::cpu::CPUData> entries;

  const std::string STR_CPU("cpu");
  const std::size_t LEN_STR_CPU = STR_CPU.size();
  const std::string STR_TOT("tot");

  while(std::getline(fileStat, line))
  {
    // cpu stats line found
    if(!line.compare(0, LEN_STR_CPU, STR_CPU))
    {
      std::istringstream ss(line);

      // store entry
      entries.emplace_back(info::cpu::CPUData());
      info::cpu::CPUData & entry = entries.back();

      // read cpu label
      ss >> entry.cpu;

      if(entry.cpu.size() > LEN_STR_CPU)
        entry.cpu.erase(0, LEN_STR_CPU);
      else
        entry.cpu = STR_TOT;

      // read times
      for(int i = 0; i < 10; ++i)
        ss >> entry.times[i];
    }
  }



  return entries;
}

size_t info::cpu::GetActiveTime(const CPUData & e) {
  return  e.times[S_USER] +
      e.times[S_NICE] +
      e.times[S_SYSTEM] +
      e.times[S_IRQ] +
      e.times[S_SOFTIRQ] +
      e.times[S_STEAL] +
      e.times[S_GUEST] +
      e.times[S_GUEST_NICE];
}

size_t info::cpu::GetIdleTime(const CPUData & e) {
  return	e.times[S_IDLE] +
      e.times[S_IOWAIT];
}

std::string info::cpu::get_cpu_usage() {

  // snapshot 1
  std::vector<info::cpu::CPUData> entries1 = info::cpu::ReadStatsCPU();

  // 100ms pause
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // snapshot 2
  std::vector<info::cpu::CPUData> entries2 = info::cpu::ReadStatsCPU();

  // get times for total cpu usage (entries1[0],entries2[0])
  const float ACTIVE_TIME = static_cast<float>(info::cpu::GetActiveTime(entries2[0]) - info::cpu::GetActiveTime(entries1[0]));

  const float IDLE_TIME	= static_cast<float>(info::cpu::GetIdleTime(entries2[0]) - info::cpu::GetIdleTime(entries1[0]));

  const float TOTAL_TIME = ACTIVE_TIME + IDLE_TIME;

  return (std::to_string(100.f * ACTIVE_TIME / TOTAL_TIME) + "%");
}
#endif