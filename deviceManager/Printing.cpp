#include "Printing.h"
#include "DevicePropertyData.h"
#include "DevicePropertyKey.h"
#include "DeviceManagerUtils.h"
#include <iostream>
#include <string>
#include <map>
#include <iomanip>

static size_t s_indent = 0;

IndentGuard::IndentGuard() {
    s_indent += 2;
    std::cout << "  ";
}

IndentGuard::IndentGuard(const IndentGuard&) { s_indent += 2; }

IndentGuard::~IndentGuard() {
    s_indent -= 2;
    std::cout << "\r" << std::string(s_indent, ' ');
}

template <typename T>
struct str_trait { constexpr static char space = ' '; };

template <>
struct str_trait<wchar_t> { constexpr static wchar_t space = ' '; };

template <typename Char>
static std::basic_string<Char> indent() {
    return std::basic_string<Char>(s_indent, str_trait<Char>::space);
}

template <class _Elem, class _Traits>
static std::basic_ostream<_Elem, _Traits>& endlind(
    std::basic_ostream<_Elem, _Traits>& _Ostr) {
    return _Ostr << std::endl << indent<_Elem>();
}

static std::map<std::wstring, DevicePropertyData> getPropertyMap(std::vector<DevicePropertyKey>& properties) {

    std::map<std::wstring, DevicePropertyData> returnValue;
    for (auto& prop : properties) {
        auto propName = prop.getPropertyName();
        auto propValue = prop.getPropertyValue();

        returnValue[propName] = propValue;
    }
    return returnValue;
}

static void hex_print(const void* base, size_t size) {
    auto begin = reinterpret_cast<const BYTE*>(base);
    auto end = reinterpret_cast<const BYTE*>(base) + size;

    for (auto ptr = begin; ptr < end; ++ptr) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(*ptr);
    }
}

template <typename T>
static void hex_print(const T& value) {
    hex_print(&value, sizeof(T));
}

template <typename T>
static std::enable_if_t<std::is_integral_v<T>> free_printer(T value) {

    std::cout << std::hex << value;
}

static void free_printer(const std::wstring& value) {
    std::wcout << value;
}

template <typename T>
static void free_printer(const std::vector<T>& value) {
    IndentGuard g;
    std::cout << endlind;
    for (const auto& val : value) {
        free_printer(val);
    }
}

static void free_printer(const std::vector<BYTE>& value) {
    hex_print(value.data(), value.size());
}

static void free_printer(const GUID& value) {
    RPC_WSTR rpcStr{};
    DeviceManagerUtils::RPCCheck(UuidToString(&value, &rpcStr));
    std::wstring nameStr = reinterpret_cast<wchar_t*>(rpcStr);
    DeviceManagerUtils::RPCCheck(RpcStringFree(&rpcStr));
    free_printer(L"{" + nameStr + L"}");
}

static void free_printer(const std::monostate&) {
    std::cout << "None";
}

static void free_printer(const DECIMAL& value) {
    hex_print(value);
}

static void free_printer(const CURRENCY& value) {
    hex_print(value);
}

static void free_printer(const FLOAT& value) {
    hex_print(value);
}

static void free_printer(const DOUBLE& value) {
    hex_print(value);
}

static void free_printer(const FILETIME& value) {
    // TODO: Format time
    hex_print(value);
}

static void free_printer(const FailureType& value) {
    std::cout << "Failure";
}

template <typename ...Args>
static void free_printer(const std::variant<Args...>& value) {
    std::visit([](const auto& val) { free_printer(val); }, value);
}

static std::vector<std::wstring> s_excludedDevicePropertyKeys = {
    L"DEVPKEY_Device_Children",
    L"DEVPKEY_Device_Siblings",
};

static void free_printer(const std::map<std::wstring, DevicePropertyData>& value) {
    IndentGuard g;
    for (const auto& val : value) {
        if (std::find(s_excludedDevicePropertyKeys.begin(),
            s_excludedDevicePropertyKeys.end(),
            val.first) != s_excludedDevicePropertyKeys.end()) {
            continue;
        }
        std::wcout << val.first << ": ";
        free_printer(val.second);
        std::cout << endlind;
    }
}

void free_printer(DeviceNode& devNode) {
    auto deviceProperties = devNode.properties();
    auto devicePropetriesMap = getPropertyMap(deviceProperties);
    auto [status, problem] = devNode.statusAndProblem();

    std::cout << "\"";
    free_printer(devicePropetriesMap[L"DEVPKEY_Device_DeviceDesc"]);
    std::cout << "\" status " << std::hex << status;
    if (problem) {
        std::cout << " Reported problem " << problem;
    }
    std::cout << endlind << "Properties:" << endlind;
    IndentGuard g;
    free_printer(devicePropetriesMap);
    std::cout << endlind;
}
