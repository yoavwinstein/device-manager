#include "SerializationCommon.h"
#include "DeviceManagerUtils.h"

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
