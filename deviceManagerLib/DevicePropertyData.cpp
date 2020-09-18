#include "DevicePropertyData.h"
#include "DeviceManagerUtils.h"
#include <map>

auto NOTHING_LAMBDA = [](void*, size_t) { return std::monostate{}; };
template <typename T>
struct SimpleTypeFunctor {
    DevicePropertyData operator()(void* buf, size_t size) {
        if (size != sizeof(T)) {
            throw std::exception("Invalid size");
        }
        return *reinterpret_cast<T*>(buf);
    }
};

static std::map<DEVPROPTYPE, std::function<FuncProto>> typeToDataConverterMap = {
    { DEVPROP_TYPE_EMPTY, NOTHING_LAMBDA },
    { DEVPROP_TYPE_NULL, NOTHING_LAMBDA },
    { DEVPROP_TYPE_SBYTE, SimpleTypeFunctor<SBYTE>() },
    { DEVPROP_TYPE_BYTE, SimpleTypeFunctor<BYTE>() },
    { DEVPROP_TYPE_INT16, SimpleTypeFunctor<SHORT>() },
    { DEVPROP_TYPE_UINT16, SimpleTypeFunctor<USHORT>() },
    { DEVPROP_TYPE_INT32, SimpleTypeFunctor<LONG>() },
    { DEVPROP_TYPE_UINT32, SimpleTypeFunctor<ULONG>() },
    { DEVPROP_TYPE_INT64, SimpleTypeFunctor<LONG64>() },
    { DEVPROP_TYPE_UINT64, SimpleTypeFunctor<ULONG64>() },
    { DEVPROP_TYPE_FLOAT, SimpleTypeFunctor<FLOAT>() },
    // { DEVPROP_TYPE_DOUBLE, SimpleTypeFunctor<DOUBLE>() },
    { DEVPROP_TYPE_DECIMAL, SimpleTypeFunctor<DECIMAL>() },
    { DEVPROP_TYPE_GUID, SimpleTypeFunctor<GUID>() },
    { DEVPROP_TYPE_CURRENCY, SimpleTypeFunctor<CURRENCY>() },
    // { DEVPROP_TYPE_DATE, SimpleTypeFunctor<DATE>() },
    { DEVPROP_TYPE_FILETIME, SimpleTypeFunctor<FILETIME>() },
    { DEVPROP_TYPE_BOOLEAN, SimpleTypeFunctor<DEVPROP_BOOLEAN>() },
    { DEVPROP_TYPE_STRING, [](void* buf, size_t size) { return std::wstring(static_cast<wchar_t*>(buf), size / sizeof(wchar_t)); } },
    { DEVPROP_TYPE_STRING_LIST, [](void* buf, size_t size) {
        std::wstring str(static_cast<wchar_t*>(buf), size / sizeof(wchar_t));
        return DeviceManagerUtils::split(str, std::wstring(L"\x00", 1));
    } },
    { DEVPROP_TYPE_BINARY, [](void* buf, size_t size) {
        return std::vector<BYTE>(static_cast<BYTE*>(buf), static_cast<BYTE*>(buf) + size);
    } },
};

DevicePropertyData getDevicePropertyData(DEVINST deviceInstance, DEVPROPKEY propertyKey)
{
    DEVPROPTYPE type;
    ULONG size{};
    CONFIGRET result = CM_Get_DevNode_Property(deviceInstance, &propertyKey, &type, nullptr, &size, 0);
    if (result != CR_BUFFER_SMALL) {
        throw DeviceManagerUtils::CMException(result);
    }
    auto typeToDataFuncIter = typeToDataConverterMap.find(type);
    if (typeToDataFuncIter == typeToDataConverterMap.end()) {
        return FailureType{};
    }
    std::vector<BYTE> data(size);
    DeviceManagerUtils::CMCheck(CM_Get_DevNode_Property(deviceInstance, &propertyKey, &type, data.data(), &size, 0));
    return typeToDataFuncIter->second(data.data(), data.size());
}
