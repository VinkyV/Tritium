#pragma once

#include "windows.h"
#include "psapi.h"
#include <string>
#include <sstream>
#include <iomanip>

namespace SystemProfilier {

void init();
std::string getCpuUsage();
double getVirtualMemory();
double getVirtualMemoryUsed();
double getPhysicalMemory();
double getPhysicalMemoryUsed();
}