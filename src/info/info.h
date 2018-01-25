
#ifndef PROJECT_INFO_H
#define PROJECT_INFO_H

#pragma once

#include <boost/filesystem.hpp>
#include <cstdint>
#include <string>
#include <vector>
#include <functional>
//#include "boost/filesystem.hpp"

namespace info {
namespace cpu {
//enum class architecture_t {
//  x64,
//  arm,
//  itanium,
//  x86,
//  unknown,
//};

enum class endianness_t {
  little,
  big,
};

enum class instruction_set_t {
  s3d_now,
  s3d_now_extended,
  mmx,
  mmx_extended,
  sse,
  sse2,
  sse3,
  ssse3,
  sse4a,
  sse41,
  sse42,
  aes,

  avx,
  avx2,

  avx_512,
  avx_512_f,
  avx_512_cd,
  avx_512_pf,
  avx_512_er,
  avx_512_vl,
  avx_512_bw,
  avx_512_bq,
  avx_512_dq,
  avx_512_ifma,
  avx_512_vbmi,

  hle,

  bmi1,
  bmi2,
  adx,
  mpx,
  sha,
  prefetch_wt1,

  fma3,
  fma4,

  xop,

  rd_rand,

  x64,
  x87_fpu,
};

enum class cache_type_t {
  unified,
  instruction,
  data,
  trace,
};

struct quantities_t {
  /// Hyperthreads.
  std::uint32_t logical;
  /// Physical "cores".
  std::string physical;
  /// Physical CPU units/packages/sockets.
  std::uint32_t packages;
};

struct cache_t {
  std::size_t size;
  std::size_t line_size;
  std::uint8_t associativity;
  cache_type_t type;
};

typedef struct CPUData {
  std::string cpu;
  size_t times[10];
} CPUData;

enum CPUStates {
  S_USER = 0,
  S_NICE,
  S_SYSTEM,
  S_IDLE,
  S_IOWAIT,
  S_IRQ,
  S_SOFTIRQ,
  S_STEAL,
  S_GUEST,
  S_GUEST_NICE
};

size_t GetActiveTime(const CPUData &e);

size_t GetIdleTime(const CPUData &e);

std::vector<info::cpu::CPUData> ReadStatsCPU();

/// Returns the usage percentage of total CPU
std::string get_cpu_usage();

/// Returns the quantity of CPU at various gradation.
quantities_t get_cpu_quantities();

std::string get_cpu_cores();

std::string get_cpu_temp();

/// Get CPU's cache properties.
///
/// `level` is the cache level (3 -> L3 cache).
cache_t cache(unsigned int level);

/// Returns product information of the device
std::string get_product_name();

/// Returns the architecture of the current CPU.
std::string get_architecture();

/// Returns the current frequency of the current CPU in Hz.
std::string get_frequency();

/// Returns the current endianness of the current CPU.
endianness_t endianness();

/// Returns the CPU's vendor.
std::string get_vendor();

/// Returns the CPU's vendor according to the CPUID instruction
std::string vendor_id();

/// Returns the CPU's model name.
std::string get_model_name();

/// Returns whether an instruction set is supported by the current CPU.
///
/// `noexcept` on Windows
bool get_instruction_set_support(instruction_set_t set);

/// Retrieve all of the instruction sets this hardware supports
std::vector<instruction_set_t> supported_instruction_sets();
}

namespace system {
struct memory_t {
  std::string _physical_usage;
  std::string _physical_used;
  std::string _physical_available;
  std::string _physical_free;
  std::string _physical_total;
};

struct diskspace_t {
  std::string _disk_usage;
  std::string _diskspace_used;
  std::string _diskspace_available;
  std::string _diskspace_free;
  std::string _diskspace_total;
};

enum class kernel_t {
  windows_nt,
  linux_,
  darwin,
  unknown,
};

struct kernel_info_t {
  std::string variant;
  std::uint64_t major;
  std::uint64_t minor;
  std::uint64_t patch;
  std::uint64_t build_number;
};

struct OS_info_t {
  std::string name;
  std::string full_name;
  std::uint64_t major;
  std::uint64_t minor;
  std::uint64_t patch;
  std::uint64_t build_number;
};

struct display_t {
  std::uint32_t width;
  std::uint32_t height;
  std::uint32_t dpi;
  /// Bits Per Pixel a.k.a. depth
  std::uint32_t bpp;
};

/// Get amount of connected mice.
std::size_t mouse_amount();

/// Get amount of connected keyboards.
///
/// Always returns 0 on Linuxish kernels, as it can not be detected there.
std::size_t keyboard_amount();

/// Get amount of other connected HIDs.
///
/// Always returns 0 on Linuxish kernels, as it can not be detected there.
std::size_t other_HID_amount();

/// Get RAM statistics.
memory_t get_memory_info();

/// Get total RAM statistics.
std::string get_memory_usage();

std::string get_total_memory();

std::string get_used_memory();

std::string get_free_memory();

/// Get Disk statistics.
diskspace_t get_diskspace_info();

std::string get_disk_usage();

std::string get_total_disk_space();

std::string get_used_disk_space();

std::string get_free_disk_space();

/// Get System Time
std::string get_system_time();

/// Get Hostname
std::string get_hostname();

/// Get kernel information.
kernel_info_t get_kernel_info();

std::string get_kernel_variant();

/// Get kernel information.
OS_info_t get_OS_info();

std::string get_os_full_name();

std::string get_open_files();

std::string get_process_count();

/// Get information about displays.
std::vector<display_t> displays();
}
}

#endif // PROJECT_INFO_H
