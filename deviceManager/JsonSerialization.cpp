#include "JsonSerialization.h"
#include "SerializationCommon.h"
#include <iomanip>
#include <sstream>

static std::wstring hexify(const void* base, size_t size) {
    auto begin = reinterpret_cast<const BYTE*>(base);
    auto end = reinterpret_cast<const BYTE*>(base) + size;
    std::wostringstream sstr;
    for (auto ptr = begin; ptr < end; ++ptr) {
        sstr << std::hex << std::setfill(L'0') << std::setw(2) << static_cast<uint16_t>(*ptr);
    }
    return sstr.str();
}

template <typename T>
static std::enable_if_t<std::is_pod_v<T>, std::wstring> hexify(const T& value) {
    return hexify(&value, sizeof(T));
}

template <typename T>
static std::enable_if_t<std::is_integral_v<T>> json_serialize(web::json::value& obj, T value) {
    obj = web::json::value(static_cast<uint64_t>(value));
}

static void json_serialize(web::json::value& obj, const std::wstring& value) {
    obj = web::json::value(value);
}

template <typename T>
static void json_serialize(web::json::value& obj, const std::vector<T>& value) {
    std::vector<web::json::value> serializedArray(value.size());
    for (size_t i = 0; i < value.size(); ++i) {
        json_serialize(serializedArray[i], value[i]);
    }
    obj = web::json::value::array(serializedArray);
}

static void json_serialize(web::json::value& obj, const std::vector<BYTE>& value) {
    obj = web::json::value(hexify(value.data(), value.size()));
}

static void json_serialize(web::json::value& obj, const GUID& value) {
    obj = web::json::value(guidToString(value));
}

static void json_serialize(web::json::value& obj, const std::monostate&) {
    obj = web::json::value();
}

static void json_serialize(web::json::value& obj, const DECIMAL& value) {
    obj = web::json::value(hexify(value));
}

static void json_serialize(web::json::value& obj, const CURRENCY& value) {
    obj = web::json::value(hexify(value));
}

static void json_serialize(web::json::value& obj, const FLOAT& value) {
    obj = web::json::value(hexify(value));
}

static void json_serialize(web::json::value& obj, const DOUBLE& value) {
    obj = web::json::value(hexify(value));
}

static void json_serialize(web::json::value& obj, const FILETIME& value) {
    // TODO: Format time
    obj = web::json::value(hexify(value));
}

static void json_serialize(web::json::value& obj, const FailureType& value) {
    obj = web::json::value(L"Failure");
}

template <typename ...Args>
static void json_serialize(web::json::value& obj, const std::variant<Args...>& value) {
    std::visit([&](const auto& val) { json_serialize(obj, val); }, value);
}

static void json_serialize(web::json::value& obj,
                           const std::map<std::wstring, DevicePropertyData>& value) {
    
    for (const auto& val : value) {
        if (isExcludedDevicePropertyKey(val.first)) {
            continue;
        }
        json_serialize(obj[val.first], val.second);
    }
}

void json_serialize(web::json::value& obj, const DeviceNode& devNode) {
	auto properties = devNode.properties();
	auto propertiesMap = getPropertyMap(properties);
    auto [status, problem] = devNode.statusAndProblem();
    json_serialize(obj[L"deviceName"], propertiesMap[L"DEVPKEY_Device_DeviceDesc"]);
    json_serialize(obj[L"status"], status);
    json_serialize(obj[L"problem"], problem);
    json_serialize(obj[L"properties"], propertiesMap);
    json_serialize(obj[L"children"], devNode.children());
}
