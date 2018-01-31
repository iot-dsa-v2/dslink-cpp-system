#ifdef _WIN32
#include "info.h"
#include <WinSock2.h>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <winver.h>
//#include <unistd.h>
#include <BaseTsd.h>
#include <Pdh.h>
#include <VersionHelpers.h>
#include <bitset>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <chrono>
#include <functional>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
typedef SSIZE_T ssize_t;
#include <Wbemidl.h>
#include <comdef.h>

#define WIN32_LEAN_AND_MEAN
#include <wbemidl.h>
#include <windows.h>

#include <winsock2.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <string>

#pragma comment(lib, "iphlpapi.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x)) 
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

using namespace std;

std::string info::cpu::get_architecture() {
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);

  switch (sysinfo.wProcessorArchitecture) {
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
std::string info::system::get_hostname() {

  char hostname[1024];
  gethostname(hostname, 1024);

  return hostname;
}
std::string info::system::get_system_time() {

  time_t now = time(0);

  return ctime(&now);
}

static std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> cpuinfo_buffer() {
  std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer;

  DWORD byte_count = 0;
  GetLogicalProcessorInformation(nullptr, &byte_count);
  buffer.resize(byte_count / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
  GetLogicalProcessorInformation(buffer.data(), &byte_count);

  return buffer;
}

std::string info::cpu::get_frequency() {
  LARGE_INTEGER freq;
  QueryPerformanceFrequency(&freq);
  return std::to_string(static_cast<int64_t>(freq.QuadPart / 1000)) + " MHz";
}

info::cpu::quantities_t info::cpu::get_cpu_quantities() {
  int physical_cpu_count = 0;
  info::cpu::quantities_t ret{};
  for (auto &&info : cpuinfo_buffer())
    switch (info.Relationship) {
    case RelationProcessorCore:
      ++physical_cpu_count;
      // A hyperthreaded core supplies more than one logical processor.
      ret.logical += static_cast<std::uint32_t>(
          std::bitset<sizeof(ULONG_PTR) * 8>(info.ProcessorMask).count());
      break;

    case RelationProcessorPackage:
      // Logical processors share a physical package.
      ++ret.packages;
      break;

    default:
      break;
    }
  ret.physical = std::to_string(physical_cpu_count);
  return ret;
}
std::string info::cpu::get_cpu_cores() { return get_cpu_quantities().physical; }
std::string info::cpu::get_product_name() {
  return boost::asio::ip::host_name();
}
static std::string cpuinfo_value(const char *key) {

  SYSTEM_INFO siSysInfo;

  // Copy the hardware information to the SYSTEM_INFO structure.
  GetSystemInfo(&siSysInfo);

  // Display the contents of the SYSTEM_INFO structure.

  printf("Hardware information: \n");
  printf("  OEM ID: %u\n", siSysInfo.dwOemId);
  printf("  Number of processors: %u\n", siSysInfo.dwNumberOfProcessors);
  printf("  Page size: %u\n", siSysInfo.dwPageSize);
  printf("  Processor type: %u\n", siSysInfo.dwProcessorType);
  printf("  Minimum application address: %lx\n",
         siSysInfo.lpMinimumApplicationAddress);
  printf("  Maximum application address: %lx\n",
         siSysInfo.lpMaximumApplicationAddress);
  printf("  Active processor mask: %u\n", siSysInfo.dwActiveProcessorMask);

  return "";
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

template <unsigned int IdentLen>
static std::string central_processor_subkey(const char *key) {
  HKEY hkey;
  if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                    R"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)", 0,
                    KEY_READ, &hkey))
    return {};

  char identifier[IdentLen];
  DWORD identifier_len = sizeof identifier;
  LPBYTE lpdata = static_cast<LPBYTE>(static_cast<void *>(&identifier[0]));
  if (RegQueryValueExA(hkey, key, nullptr, nullptr, lpdata, &identifier_len))
    return {};

  return identifier;
}

std::string info::cpu::get_vendor() {
  // TODO retrieve vendor information from system
  return central_processor_subkey<13>("VendorIdentifier");
}

std::string info::cpu::get_model_name() {
  // Get extended ids.
  int CPUInfo[4] = {-1};
  __cpuid(CPUInfo, 0x80000000);
  unsigned int nExIds = CPUInfo[0];

  // Get the information associated with each extended ID.
  char CPUBrandString[0x40] = {0};
  for (unsigned int i = 0x80000000; i <= nExIds; ++i) {
    __cpuid(CPUInfo, i);

    // Interpret CPU brand string and cache information.
    if (i == 0x80000002) {
      memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
    } else if (i == 0x80000003) {
      memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
    } else if (i == 0x80000004) {
      memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
    }
  }
  return CPUBrandString;
}

BOOL EqualsMajorVersion(DWORD majorVersion) {
  OSVERSIONINFOEX osVersionInfo;
  ::ZeroMemory(&osVersionInfo, sizeof(OSVERSIONINFOEX));
  osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  osVersionInfo.dwMajorVersion = majorVersion;
  ULONGLONG maskCondition =
      ::VerSetConditionMask(0, VER_MAJORVERSION, VER_EQUAL);
  return ::VerifyVersionInfo(&osVersionInfo, VER_MAJORVERSION, maskCondition);
}
BOOL EqualsMinorVersion(DWORD minorVersion) {
  OSVERSIONINFOEX osVersionInfo;
  ::ZeroMemory(&osVersionInfo, sizeof(OSVERSIONINFOEX));
  osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  osVersionInfo.dwMinorVersion = minorVersion;
  ULONGLONG maskCondition =
      ::VerSetConditionMask(0, VER_MINORVERSION, VER_EQUAL);
  return ::VerifyVersionInfo(&osVersionInfo, VER_MINORVERSION, maskCondition);
}

std::string info::system::get_os_full_name() { return get_OS_info().full_name; }

info::system::OS_info_t info::system::get_OS_info() {

  const auto kernel_version = info::system::get_kernel_info();
  std::string version = "";
  if (IsWindows10OrGreater())
    version = "Windows 10";
  else if (IsWindows8Point1OrGreater())
    version = "Windows 8.1";
  else if (IsWindows8OrGreater())
    version = "Windows 8";
  else if (IsWindows7SP1OrGreater())
    version = "Windows 7 SP1";
  else if (IsWindows7OrGreater())
    version = "Windows 7";
  else if (IsWindowsVistaSP2OrGreater())
    version = "Windows Vista SP2";
  else if (IsWindowsVistaSP1OrGreater())
    version = "Windows Vista SP1";
  else if (IsWindowsVistaOrGreater())
    version = "Windows Vista";
  else if (IsWindowsXPSP3OrGreater())
    version = "Windows XP SP3";
  else if (IsWindowsXPSP2OrGreater())
    version = "Windows XP SP2";
  else if (IsWindowsXPSP1OrGreater())
    version = "Windows XP SP1";
  else if (IsWindowsXPOrGreater())
    version = "Windows XP";

  // if (IsWindowsServer())
  //{
  //	printf("Server\n");
  //}
  // else
  //{
  //	printf("Client\n");
  //}

  return {"Windows NT",         version,
          kernel_version.major, kernel_version.minor,
          kernel_version.patch, kernel_version.build_number};
}

info::system::kernel_info_t info::system::get_kernel_info() {

  // std::string path;
  // path.resize(GetSystemDirectoryA(nullptr, 0) - 1);
  // GetSystemDirectoryA(&path[0], static_cast<UINT>(path.size() + 1));
  // path += "\\kernel32.dll";

  // const auto ver_info_len = GetFileVersionInfoSizeA(path.c_str(), nullptr);
  // auto ver_info = std::make_unique<std::uint8_t[]>(ver_info_len);
  // GetFileVersionInfoA(path.c_str(), 0, ver_info_len, ver_info.get());

  // VS_FIXEDFILEINFO* file_version;
  // unsigned int file_version_len;
  // VerQueryValueA(ver_info.get(), "", reinterpret_cast<void**>(&file_version),
  // &file_version_len);

  // return { "windows_nt", HIWORD(file_version->dwProductVersionMS),
  // LOWORD(file_version->dwProductVersionMS),
  // HIWORD(file_version->dwProductVersionLS),
  // LOWORD(file_version->dwProductVersionLS) };

  DWORD dwVersion = 0;
  DWORD dwMajorVersion = 0;
  DWORD dwMinorVersion = 0;
  DWORD dwBuild = 0;

  dwVersion = GetVersion();

  // Get the Windows version.

  dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
  dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

  // Get the build number.

  if (dwVersion < 0x80000000)
    dwBuild = (DWORD)(HIWORD(dwVersion));
  return {"windows_nt", dwMajorVersion, dwMinorVersion, 0, dwBuild};
}

std::string info::system::get_kernel_variant() {
  return get_kernel_info().variant;
}

info::system::memory_t info::system::get_memory_info() {

  MEMORYSTATUSEX mem;
  mem.dwLength = sizeof(mem);
  if (!GlobalMemoryStatusEx(&mem))
    return {};
  info::system::memory_t ret;
  ret._physical_total = std::to_string(mem.ullTotalPhys / 1024 / 1024) + " mb";
  ret._physical_available =
      std::to_string(mem.ullAvailPhys / 1024 / 1024) + " mb";
  ret._physical_free = std::to_string(mem.ullAvailPhys / 1024 / 1024) + " mb";
  ret._physical_used =
      std::to_string((mem.ullTotalPhys - mem.ullAvailPhys) / 1024 / 1024) +
      " mb";

  std::string memory_usage_percentage(16, '\0');
  auto percentage = std::snprintf(
      &memory_usage_percentage[0], memory_usage_percentage.size(), "%.2f",
      ((float)((mem.ullTotalPhys - mem.ullAvailPhys)) /
       (float)mem.ullTotalPhys) *
          100);
  memory_usage_percentage.resize(percentage);

  ret._physical_usage = memory_usage_percentage + "%";

  return ret;
}

std::string info::system::get_memory_usage() {
  return get_memory_info()._physical_usage;
}

std::string info::system::get_total_memory() {
  return get_memory_info()._physical_total;
}

std::string info::system::get_used_memory() {
  return get_memory_info()._physical_used;
}

std::string info::system::get_free_memory() {
  return get_memory_info()._physical_free;
}

std::string info::system::get_process_count() {
  char line[128];
  size_t len = 0;
  ssize_t read;
  int count = 0;
  try {
    FILE *data = _popen("wmic PROCESS LIST BRIEF", "r");
    if (data == nullptr) {
      data = nullptr;
      return "0";
    }
    while (fgets(line, 128, data) != nullptr) {
      // TODO: don't count empty lines and header line
      count++;
    }
    return std::to_string(count);
  } catch (int e) {
    return "0";
  }
}

std::vector<info::cpu::CPUData> info::cpu::ReadStatsCPU() {

  std::ifstream fileStat("/proc/stat");

  std::string line;

  std::vector<info::cpu::CPUData> entries;

  const std::string STR_CPU("cpu");
  const std::size_t LEN_STR_CPU = STR_CPU.size();
  const std::string STR_TOT("tot");

  while (std::getline(fileStat, line)) {
    // cpu stats line found
    if (!line.compare(0, LEN_STR_CPU, STR_CPU)) {
      std::istringstream ss(line);

      // store entry
      entries.emplace_back(info::cpu::CPUData());
      info::cpu::CPUData &entry = entries.back();

      // read cpu label
      ss >> entry.cpu;

      if (entry.cpu.size() > LEN_STR_CPU)
        entry.cpu.erase(0, LEN_STR_CPU);
      else
        entry.cpu = STR_TOT;

      // read times
      for (int i = 0; i < 10; ++i)
        ss >> entry.times[i];
    }
  }

  return entries;
}

size_t info::cpu::GetActiveTime(const CPUData &e) {
  return e.times[S_USER] + e.times[S_NICE] + e.times[S_SYSTEM] +
         e.times[S_IRQ] + e.times[S_SOFTIRQ] + e.times[S_STEAL] +
         e.times[S_GUEST] + e.times[S_GUEST_NICE];
}

size_t info::cpu::GetIdleTime(const CPUData &e) {
  return e.times[S_IDLE] + e.times[S_IOWAIT];
}

static float CalculateCPULoad(unsigned long long idleTicks,
                              unsigned long long totalTicks) {
  static unsigned long long _previousTotalTicks = 0;
  static unsigned long long _previousIdleTicks = 0;

  unsigned long long totalTicksSinceLastTime = totalTicks - _previousTotalTicks;
  unsigned long long idleTicksSinceLastTime = idleTicks - _previousIdleTicks;

  float ret =
      1.0f - ((totalTicksSinceLastTime > 0)
                  ? ((float)idleTicksSinceLastTime) / totalTicksSinceLastTime
                  : 0);

  _previousTotalTicks = totalTicks;
  _previousIdleTicks = idleTicks;
  return ret;
}

static unsigned long long FileTimeToInt64(const FILETIME &ft) {
  return (((unsigned long long)(ft.dwHighDateTime)) << 32) |
         ((unsigned long long)ft.dwLowDateTime);
}

std::string info::cpu::get_cpu_usage() {
  FILETIME idleTime, kernelTime, userTime;
  return GetSystemTimes(&idleTime, &kernelTime, &userTime)
             ? std::to_string(
                   (CalculateCPULoad(FileTimeToInt64(idleTime),
                                     FileTimeToInt64(kernelTime) +
                                         FileTimeToInt64(userTime))) *
                   100) +
                   "%"
             : "NA";
}

std::string info::cpu::get_cpu_temp() {

  HRESULT hres;

  // Step 1: --------------------------------------------------
  // Initialize COM. ------------------------------------------

  hres = CoInitializeEx(0, COINIT_MULTITHREADED);
  if (FAILED(hres)) {
    cout << "Failed to initialize COM library. Error code = 0x" << hex << hres
         << endl;
    return "1"; // Program has failed.
  }

  // Step 2: --------------------------------------------------
  // Set general COM security levels --------------------------

  hres =
      CoInitializeSecurity(NULL,
                           -1,                        // COM authentication
                           NULL,                      // Authentication services
                           NULL,                      // Reserved
                           RPC_C_AUTHN_LEVEL_DEFAULT, // Default authentication
                           RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
                           NULL,                        // Authentication info
                           EOAC_NONE, // Additional capabilities
                           NULL       // Reserved
                           );

  if (FAILED(hres)) {
    cout << "Failed to initialize security. Error code = 0x" << hex << hres
         << endl;
    CoUninitialize();
    return "1"; // Program has failed.
  }

  // Step 3: ---------------------------------------------------
  // Obtain the initial locator to WMI -------------------------

  IWbemLocator *pLoc = NULL;

  hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
                          IID_IWbemLocator, (LPVOID *)&pLoc);

  if (FAILED(hres)) {
    cout << "Failed to create IWbemLocator object."
         << " Err code = 0x" << hex << hres << endl;
    CoUninitialize();
    return "1"; // Program has failed.
  }

  // Step 4: -----------------------------------------------------
  // Connect to WMI through the IWbemLocator::ConnectServer method

  IWbemServices *pSvc = NULL;

  // Connect to the root\cimv2 namespace with
  // the current user and obtain pointer pSvc
  // to make IWbemServices calls.
  hres = pLoc->ConnectServer(
      _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
      NULL,                    // User name. NULL = current user
      NULL,                    // User password. NULL = current
      0,                       // Locale. NULL indicates current
      NULL,                    // Security flags.
      0,                       // Authority (for example, Kerberos)
      0,                       // Context object
      &pSvc                    // pointer to IWbemServices proxy
      );

  if (FAILED(hres)) {
    cout << "Could not connect. Error code = 0x" << hex << hres << endl;
    pLoc->Release();
    CoUninitialize();
    return "1"; // Program has failed.
  }

  cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;

  // Step 5: --------------------------------------------------
  // Set security levels on the proxy -------------------------

  hres = CoSetProxyBlanket(pSvc,                   // Indicates the proxy to set
                           RPC_C_AUTHN_WINNT,      // RPC_C_AUTHN_xxx
                           RPC_C_AUTHZ_NONE,       // RPC_C_AUTHZ_xxx
                           NULL,                   // Server principal name
                           RPC_C_AUTHN_LEVEL_CALL, // RPC_C_AUTHN_LEVEL_xxx
                           RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
                           NULL,                        // client identity
                           EOAC_NONE                    // proxy capabilities
                           );

  if (FAILED(hres)) {
    cout << "Could not set proxy blanket. Error code = 0x" << hex << hres
         << endl;
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();
    return "1"; // Program has failed.
  }

  // Step 6: --------------------------------------------------
  // Use the IWbemServices pointer to make requests of WMI ----

  // For example, get the name of the operating system
  IEnumWbemClassObject *pEnumerator = NULL;
  hres = pSvc->ExecQuery(bstr_t("WQL"),
                         bstr_t("SELECT * FROM Win32_TemperatureProbe"),
                         WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                         NULL, &pEnumerator);

  if (FAILED(hres)) {
    cout << "Query for operating system name failed."
         << " Error code = 0x" << hex << hres << endl;
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();
    return "1"; // Program has failed.
  }

  // Step 7: -------------------------------------------------
  // Get the data from the query in step 6 -------------------

  IWbemClassObject *pclsObj = NULL;
  ULONG uReturn = 0;

  while (pEnumerator) {
    HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

    if (0 == uReturn) {
      break;
    }

    VARIANT vtProp;

    // Get the value of the Name property
    hr = pclsObj->Get(L"NominalReading", 0, &vtProp, 0, 0);
    wcout << " OS Name : " << vtProp.bstrVal << endl;
    VariantClear(&vtProp);

    pclsObj->Release();
  }

  // Cleanup
  // ========

  pSvc->Release();
  pLoc->Release();
  pEnumerator->Release();
  CoUninitialize();

  return "0.0°C";
}

std::map<std::string, std::string> info::system::get_network_interfaces() {
	std::map<std::string, std::string> interfaces;

	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;

	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL) {
		printf("Error allocating memory needed to call GetAdaptersinfo\n");
	}

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		FREE(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
		if (pAdapterInfo == NULL) {
			printf("Error allocating memory needed to call GetAdaptersinfo\n");
		}
	}

	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
		pAdapter = pAdapterInfo;
		while (pAdapter) {

			interfaces[to_string(pAdapter->Index)] = string(pAdapter->Description) + ":" + string(pAdapter->IpAddressList.IpAddress.String);

			pAdapter = pAdapter->Next;
			printf("\n");
		}
	}
	else {
		printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);

	}
	if (pAdapterInfo)
		FREE(pAdapterInfo);

	return (interfaces);
}
#endif