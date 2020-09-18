#pragma once
#include "DevicePropertyData.h"
#include "DevicePropertyKey.h"
#include <map>
#include <string>

std::map<std::wstring, DevicePropertyData> getPropertyMap(std::vector<DevicePropertyKey>& properties);

bool isExcludedDevicePropertyKey(const std::wstring& devicePropertyKey);
std::wstring guidToString(const GUID& guid);
std::wstring getTimeFormatted(const FILETIME& fileTime);
