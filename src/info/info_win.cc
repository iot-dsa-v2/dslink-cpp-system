#ifdef _WIN32
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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

std::string info::cpu::get_architecture() {
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);

  switch(sysinfo.wProcessorArchitecture) {
      case PROCESSOR_ARCHITECTURE_AMD64:
          return "x86_64";
      case PROCESSOR_ARCHITECTURE_ARM:
          return "arm";
      case PROCESSOR_ARCHITECTURE_IA64:
          return "IA64";
      case PROCESSOR_ARCHITECTURE_INTEL:
          return "i686";
      default:
          return "Unknown";
  }
}


std::int64_t info::cpu::get_frequency() {
  LARGE_INTEGER freq;
  QueryPerformanceFrequency(&freq);
  return static_cast<int64_t>(freq.QuadPart * 1000);
}

info::cpu::quantities_t info::cpu::get_cpu_quantities() {
  info::cpu::quantities_t ret{};
  	for(auto&& info : cpuinfo_buffer())
		switch(info.Relationship) {
			case RelationProcessorCore:
				++ret.physical;
				// A hyperthreaded core supplies more than one logical processor.
				ret.logical += static_cast<std::uint32_t>(std::bitset<sizeof(ULONG_PTR) * 8>(info.ProcessorMask).count());
				break;

			case RelationProcessorPackage:
				// Logical processors share a physical package.
				++ret.packages;
				break;

			default:
				break;
		}

  return ret;
}

static std::string cpuinfo_value(const char* key) {

  SYSTEM_INFO siSysInfo;

  // Copy the hardware information to the SYSTEM_INFO structure.
  GetSystemInfo(&siSysInfo);

  // Display the contents of the SYSTEM_INFO structure.

   printf("Hardware information: \n");
   printf("  OEM ID: %u\n", siSysInfo.dwOemId);
   printf("  Number of processors: %u\n",
      siSysInfo.dwNumberOfProcessors);
   printf("  Page size: %u\n", siSysInfo.dwPageSize);
   printf("  Processor type: %u\n", siSysInfo.dwProcessorType);
   printf("  Minimum application address: %lx\n",
      siSysInfo.lpMinimumApplicationAddress);
   printf("  Maximum application address: %lx\n",
      siSysInfo.lpMaximumApplicationAddress);
   printf("  Active processor mask: %u\n",
      siSysInfo.dwActiveProcessorMask);

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

  auto kernel = info::system::kernel_t::unknown;
  if(!std::strcmp(uts.sysname, "Linux"))
    kernel = info::system::kernel_t::linux_;
  else if(!std::strcmp(uts.sysname, "Darwin"))
    kernel = info::system::kernel_t::darwin;

  return {kernel, major, minor, patch, build_number};
}

info::system::memory_t info::system::get_memory_info() {
  std::ifstream meminfo("/proc/meminfo");

  if(!meminfo.is_open() || !meminfo)
    return {};

  info::system::memory_t ret;
  for(std::string line; std::getline(meminfo, line);) {
    const auto colon_id = line.find_first_of(':');
    const auto value    = std::strtoul(line.c_str() + colon_id + 1, nullptr, 10) * 1024;

    if(line.find("MemTotal") == 0)
      ret.physical_total = value;
    else if(line.find("MemAvailable") == 0)
      ret.physical_available = value;
    else if(line.find("VmallocTotal") == 0)
      ret.virtual_total = value;
    else if(line.find("VmallocUsed") == 0)
      ret.virtual_available = ret.virtual_total - value;
  }

  return ret;
}

std::vector<info::system::CPUData> info::system::ReadStatsCPU() {

  std::ifstream fileStat("/proc/stat");

  std::string line;

  std::vector<info::system::CPUData> entries;

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
      entries.emplace_back(info::system::CPUData());
      info::system::CPUData & entry = entries.back();

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

size_t info::system::GetActiveTime(const CPUData & e) {
  return  e.times[S_USER] +
      e.times[S_NICE] +
      e.times[S_SYSTEM] +
      e.times[S_IRQ] +
      e.times[S_SOFTIRQ] +
      e.times[S_STEAL] +
      e.times[S_GUEST] +
      e.times[S_GUEST_NICE];
}

size_t info::system::GetIdleTime(const CPUData & e) {
  return	e.times[S_IDLE] +
      e.times[S_IOWAIT];
}

std::int64_t info::system::get_cpu_usage() {

  // snapshot 1
  std::vector<info::system::CPUData> entries1 = info::system::ReadStatsCPU();

  // 100ms pause
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // snapshot 2
  std::vector<info::system::CPUData> entries2 = info::system::ReadStatsCPU();

  // get times for total cpu usage (entries1[0],entries2[0])
  const float ACTIVE_TIME = static_cast<float>(info::system::GetActiveTime(entries2[0]) - info::system::GetActiveTime(entries1[0]));

  const float IDLE_TIME	= static_cast<float>(info::system::GetIdleTime(entries2[0]) - info::system::GetIdleTime(entries1[0]));

  const float TOTAL_TIME = ACTIVE_TIME + IDLE_TIME;

  return (100.f * ACTIVE_TIME / TOTAL_TIME);
}
#endif