#ifdef __APPLE__
#include "dsa_common.h"
#include "info.h"
#include <algorithm>
#include <arpa/inet.h>
#include <boost/filesystem.hpp>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <ifaddrs.h>
#include <iostream>
#include <iterator>
#include <map>
#include <netdb.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <thread>
#include <unistd.h>
#include <vector>

std::string get_macos_info(std::string input) {
    char *line = NULL;
    ssize_t chars;
    size_t len = 0;
    try {
        FILE *data = popen(input.c_str(), "r");
        if (!data)
            return "NA";

        if ((chars = getline(&line, &len, data)) != -1){
          std::string str = std::string(line, chars);
          str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
            return str;
        }
        else
            return "NA";
    } catch (int e) {
        return "NA";
    }
}

std::string info::cpu::get_architecture() {
  utsname buf;

  if (uname(&buf) == -1)
    return "Unknown";
  else
    return buf.machine;
}

std::string info::cpu::get_frequency() {
    return std::to_string(std::strtoul(get_macos_info("sysctl -n hw.cpufrequency").c_str(), nullptr, 10) / 1000000) + " MHz";
}

std::string info::cpu::get_cpu_cores() {
  return get_macos_info("sysctl -n hw.ncpu");
}

std::string info::cpu::get_product_name() {
    return get_macos_info("sw_vers -productName");
}

static std::string cpuinfo_value(const char *key) {
  std::ifstream cpuinfo("/proc/cpuinfo");

  if (!cpuinfo.is_open() || !cpuinfo)
    return 0;

  for (std::string line; std::getline(cpuinfo, line);)
    if (line.find(key) == 0) {
      const auto colon_id = line.find_first_of(':');
      const auto nonspace_id = line.find_first_not_of(" \t", colon_id) + 1;
      return line.c_str() + nonspace_id;
    }

  return {};
}

std::string info::cpu::get_vendor() { return get_macos_info("sysctl -n machdep.cpu.vendor"); }

std::string info::cpu::get_model_name() { return get_macos_info("sysctl -n machdep.cpu.brand_string"); }

std::string info::system::get_os_full_name() { return get_macos_info("sw_vers -productName") + get_macos_info("sw_vers -productVersion"); }

info::system::kernel_info_t info::system::get_kernel_info() {
  utsname uts;
  uname(&uts);

  char *marker = uts.release;
  const unsigned int major = std::strtoul(marker, &marker, 10);
  const unsigned int minor = std::strtoul(marker + 1, &marker, 10);
  const unsigned int patch = std::strtoul(marker + 1, &marker, 10);
  const unsigned int build_number = std::strtoul(marker + 1, nullptr, 10);

  std::string kernel = "Unknown";
  if (!std::strcmp(uts.sysname, "Linux"))
    kernel = "Linux";
  else if (!std::strcmp(uts.sysname, "Darwin"))
    kernel = "Darwin";

  return {kernel, major, minor, patch, build_number};
}

std::string info::system::get_kernel_variant() {
  return get_kernel_info().variant;
}

std::string info::system::get_memory_usage() {
    std::string memory_usage_percentage(16, '\0');
    auto percentage = std::snprintf(
            &memory_usage_percentage[0], memory_usage_percentage.size(), "%.2f",
            (((float)(std::strtoul(get_used_memory().c_str(), nullptr, 10)) / (float)(std::strtoul(get_total_memory().c_str(), nullptr, 10))) * 100));
    memory_usage_percentage.resize(percentage);
    return memory_usage_percentage + "%";
}

std::string info::system::get_total_memory() {
  return std::to_string(std::strtoul(get_macos_info("sysctl -n hw.memsize").c_str(), nullptr, 10) / 1024 /1024) + " MByte";
}

std::string info::system::get_used_memory() {
  return std::to_string((std::strtoul(get_macos_info("sysctl -n hw.memsize").c_str(), nullptr, 10) / 1024 /1024) - std::strtoul(get_free_memory().c_str(), nullptr, 10)) + " MByte";
}

std::string info::system::get_free_memory() {
  char *line = NULL;
  long free_pages;
  long speculative_pages;
  size_t len = 0;
  ssize_t read;
  try {
    FILE *data = popen("vm_stat", "r");
    if (!data) {
      data = nullptr;
      return "NA";
    }

    while ((getline(&line, &len, data)) != -1) {
      std::string test(line);
      const auto colon_id = test.find_first_of(':');
      const auto value = std::strtoul(test.c_str() + colon_id + 1, nullptr, 10);

      if (test.find("Pages free") == 0) {
          free_pages = value;
      }
        else if (test.find("Pages speculative") == 0){
          speculative_pages = value;
      }
    }
      return std::to_string((free_pages + speculative_pages) * 4096 / 1024 /1024) + " MByte";
  } catch (int e) {
    return "0.0°C";
  }
  return "0.0°C";
}

info::system::diskspace_t info::system::get_diskspace_info() {

  info::system::diskspace_t ret;
  boost::filesystem::space_info _space_info = boost::filesystem::space(".");

  ret._diskspace_total =
      std::to_string(_space_info.capacity / 1024 / 1024) + " MB";
  ret._diskspace_available =
      std::to_string(_space_info.available / 1024 / 1024) + " MB";
  ret._diskspace_free = std::to_string(_space_info.free / 1024 / 1024) + " MB";
  ret._diskspace_used =
      std::to_string((_space_info.capacity - _space_info.available) / 1024 /
                     1024) +
      " MB";

  std::string disk_usage_percentage(16, '\0');
  auto percentage = std::snprintf(
      &disk_usage_percentage[0], disk_usage_percentage.size(), "%.2f",
      ((float)(_space_info.capacity - _space_info.available) /
       (float)_space_info.capacity) *
          100);
  disk_usage_percentage.resize(percentage);

  ret._disk_usage = disk_usage_percentage + "%";

  return ret;
}

std::string info::system::get_disk_usage() {
  return get_diskspace_info()._disk_usage;
}

std::string info::system::get_total_disk_space() {
  return get_diskspace_info()._diskspace_total;
}

std::string info::system::get_used_disk_space() {
  return get_diskspace_info()._diskspace_used;
}

std::string info::system::get_free_disk_space() {
  return get_diskspace_info()._diskspace_free;
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

std::string info::system::get_open_files() {
    return get_macos_info("sysctl -n kern.num_files");
}

std::string info::system::get_process_count() {
  return get_macos_info("ps axo state | wc -l");
}

std::string info::cpu::get_cpu_usage() {
 return get_macos_info("ps -A -o %cpu | awk '{s+=$1} END {print s \"%\"}'");
}

std::string info::cpu::get_cpu_temp() {
  return get_macos_info("iStats cpu temp --value-only") + "°C";
}

std::map<std::string, std::string> info::system::get_network_interfaces() {
  struct ifaddrs *ifaddr, *ifa;
  int family, s, n;
  char host[NI_MAXHOST];
  std::map<std::string, std::string> interfaces;

  if (getifaddrs(&ifaddr) == -1) {
    perror("getifaddrs");
    exit(EXIT_FAILURE);
  }

  interfaces.clear();

  /* Walk through linked list, maintaining head pointer so we
     can free list later */

  for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {

    family = ifa->ifa_addr->sa_family;

    if (family == AF_INET || family == AF_INET6) {

      s = getnameinfo(ifa->ifa_addr,
                      (family == AF_INET) ? sizeof(struct sockaddr_in)
                                          : sizeof(struct sockaddr_in6),
                      host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
      if (s != 0) {
        printf("getnameinfo() failed: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
      }

      interfaces[ifa->ifa_name] += host;
      interfaces[ifa->ifa_name] += " ";
    }
  }

  for (std::map<std::string, std::string>::iterator it = interfaces.begin();
       it != interfaces.end(); ++it) {
    std::cout << it->first << " :: " << it->second << std::endl;
  }

  freeifaddrs(ifaddr);
  return (interfaces);
}
#endif