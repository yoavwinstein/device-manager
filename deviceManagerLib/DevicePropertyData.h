#pragma once

#include <variant>
#include <functional>
#include <Windows.h>
#include <cfgmgr32.h>

// Can't find SBYTE's definition so declaring it by myself :shrug:
using SBYTE = int8_t;

struct FailureType {};

using DevicePropertyData = std::variant<std::monostate, SBYTE, BYTE, SHORT, USHORT, LONG, ULONG, LONG64, ULONG64,
    FLOAT, DOUBLE, DECIMAL, GUID, CURRENCY, DATE, FILETIME, DEVPROP_BOOLEAN, std::wstring, std::vector<std::wstring>, std::vector<BYTE>, FailureType>;


using FuncProto = DevicePropertyData(void*, size_t);

DevicePropertyData getDevicePropertyData(DEVINST deviceInstance, DEVPROPKEY propertyKey);
