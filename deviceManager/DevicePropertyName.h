#pragma once

#include <string>
#include <optional>
#include <Windows.h>
#include <cfgmgr32.h>

std::optional<std::wstring> getDevicePropertyKeyName(const DEVPROPKEY& propertyKey);
