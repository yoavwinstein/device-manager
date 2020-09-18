#include "DeviceNode.h"
#include "DeviceManagerUtils.h"
#include <iostream>
#include <map>
#include <stack>
#include <type_traits>
#include <iomanip>


size_t s_indent = 0;
class IndentGuard {
public:
    IndentGuard() { s_indent += 2; }
    IndentGuard(const IndentGuard&) { s_indent += 2; }
    ~IndentGuard() { s_indent -= 2; }
};

template <typename T>
struct str_trait { constexpr static char space = ' '; };

template <>
struct str_trait<wchar_t> { constexpr static wchar_t space = ' '; };

template <typename Char>
std::basic_string<Char> indent() {
    return std::basic_string<Char>(s_indent, str_trait<Char>::space);
}

template <class _Elem, class _Traits>
std::basic_ostream<_Elem, _Traits>& __CLRCALL_OR_CDECL endlind(
    std::basic_ostream<_Elem, _Traits>& _Ostr) {
    return _Ostr << std::endl << indent<_Elem>();
}

std::map<std::wstring, DevicePropertyData> getPropertyMap(std::vector<DevicePropertyKey>& properties) {

    std::map<std::wstring, DevicePropertyData> returnValue;
    for (auto& prop : properties) {
        auto propName = prop.getPropertyName();
        auto propValue = prop.getPropertyValue();

        returnValue[propName] = propValue;
    }
    return returnValue;
}

void hex_print(const void* base, size_t size) {
    auto begin = reinterpret_cast<const BYTE*>(base);
    auto end = reinterpret_cast<const BYTE*>(base) + size;

    for (auto ptr = begin; ptr < end; ++ptr) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(*ptr);
    }
}

template <typename T>
void hex_print(const T& value) {
    hex_print(&value, sizeof(T));
}

template <typename T>
std::enable_if_t<std::is_integral_v<T>> free_printer(T value) {
    
    std::cout << std::hex << value;
}

void free_printer(const std::wstring& value) {
    std::wcout << value;
}

template <typename T>
void free_printer(const std::vector<T>& value) {
    IndentGuard g;
    std::cout << endlind;
    for (const auto& val : value) {
        free_printer(val);
    }
}

void free_printer(const std::vector<BYTE>& value) {
    hex_print(value.data(), value.size());
}

void free_printer(const GUID& value) {
    RPC_WSTR rpcStr{};
    DeviceManagerUtils::RPCCheck(UuidToString(&value, &rpcStr));
    std::wstring nameStr = reinterpret_cast<wchar_t*>(rpcStr);
    DeviceManagerUtils::RPCCheck(RpcStringFree(&rpcStr));
    free_printer(L"{" + nameStr + L"}");
}

void free_printer(const std::monostate&) {
    std::cout << "None";
}

void free_printer(const DECIMAL& value) {
    hex_print(value);
}

void free_printer(const CURRENCY& value) {
    hex_print(value);
}

void free_printer(const FLOAT& value) {
    hex_print(value);
}

void free_printer(const DOUBLE& value) {
    hex_print(value);
}

void free_printer(const FILETIME& value) {
    // TODO: Format time
    hex_print(value);
}

void free_printer(const FailureType& value) {
    std::cout << "Failure";
}

template <typename ...Args>
void free_printer(const std::variant<Args...>& value) {
    std::visit([](const auto& val) { free_printer(val); }, value);
}

std::vector<std::wstring> s_excludedDevicePropertyKeys = {
    L"DEVPKEY_Device_Children",
    L"DEVPKEY_Device_Siblings",
};

void free_printer(const std::map<std::wstring, DevicePropertyData>& value) {
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
    
}
using MyTest = std::variant<int, std::monostate>;

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

int main() {
    DeviceManagerUtils::COMCheck(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

    using StackMember = std::variant<DeviceNode, IndentGuard>;
    std::stack<StackMember> stack;
    stack.push(DeviceNode::rootDevice());
    while (!stack.empty()) {
        std::visit(
            overloaded {
                [&stack](DeviceNode& devNode) {
                    free_printer(devNode);
                    stack.pop();
                    stack.push(IndentGuard{});
                    for (auto& childDevice : devNode.children()) {
                        stack.push(childDevice);
                    }
                },
                [&stack](IndentGuard&) { stack.pop(); }
            },
            stack.top()
        );
    }

    return 0;
}