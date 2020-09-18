#include "SerializationCommon.h"
#include "DeviceManagerUtils.h"
#include <iomanip>
#include <sstream>

std::map<std::wstring, DevicePropertyData> getPropertyMap(std::vector<DevicePropertyKey>& properties) {

    std::map<std::wstring, DevicePropertyData> returnValue;
    for (auto& prop : properties) {
        auto propName = prop.getPropertyName();
        auto propValue = prop.getPropertyValue();

        returnValue[propName] = propValue;
    }
    return returnValue;
}

static std::vector<std::wstring> s_excludedDevicePropertyKeys = {
    L"DEVPKEY_Device_Children",
    L"DEVPKEY_Device_Siblings",
    L"DEVPKEY_Device_BusRelations"
};

bool isExcludedDevicePropertyKey(const std::wstring& devicePropertyKey) {
    return std::find(s_excludedDevicePropertyKeys.begin(),
                     s_excludedDevicePropertyKeys.end(),
                     devicePropertyKey) != s_excludedDevicePropertyKeys.end();
}

std::wstring guidToString(const GUID& guid) {
    RPC_WSTR rpcStr{};
    DeviceManagerUtils::RPCCheck(UuidToString(&guid, &rpcStr));
    std::wstring nameStr = reinterpret_cast<wchar_t*>(rpcStr);
    DeviceManagerUtils::RPCCheck(RpcStringFree(&rpcStr));
    return L"{" + nameStr + L"}";
}

std::wstring getTimeFormatted(const FILETIME& fileTime) {
    SYSTEMTIME sysTime{};
    DeviceManagerUtils::WIN32Check(FileTimeToSystemTime(&fileTime, &sysTime));
    std::wostringstream sstr;
    auto width2 = std::setw(2);
    // Format is as following: "yyyy/mm/dd hh:mm::ss"
    sstr << std::setfill(L'0') << width2 << sysTime.wMonth << L"/" << width2 << sysTime.wDay << L"/" << sysTime.wYear;
    sstr << L" ";
    sstr << width2 << sysTime.wHour << L":" << width2 << sysTime.wMinute << L":" << width2 << sysTime.wSecond;
    return sstr.str();
}
