#include "DevicePropertyKey.h"
#include "DeviceManagerUtils.h"

DevicePropertyKey::DevicePropertyKey(DEVINST deviceInstance, const DEVPROPKEY& propKey) : m_propertyDevice(deviceInstance), m_propertyKey(propKey) {}

static std::wstring getNameFallback(const DEVPROPKEY& propKey) {
    RPC_WSTR rpcStr{};
    DeviceManagerUtils::RPCCheck(UuidToString(&propKey.fmtid, &rpcStr));
    auto nameStr = std::wstring(reinterpret_cast<const wchar_t*>(rpcStr)) + L"," + std::to_wstring(propKey.pid);
    DeviceManagerUtils::RPCCheck(RpcStringFree(&rpcStr));
    return nameStr;
}

std::wstring DevicePropertyKey::getPropertyName() {
    auto deviceName = getDevicePropertyKeyName(m_propertyKey);
    if (!deviceName) {
        return getNameFallback(m_propertyKey);
    }
    return *deviceName;
}

DevicePropertyData DevicePropertyKey::getPropertyValue() {
    return getDevicePropertyData(m_propertyDevice, m_propertyKey);
}
