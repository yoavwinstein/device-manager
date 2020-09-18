#include "DeviceNode.h"
#include "DeviceManagerUtils.h"
#include <iostream>
#include <map>
#include <stack>
#include <type_traits>

size_t s_indent = 0;
class IndentGuard {
public:
    IndentGuard() { s_indent += 2; }
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

template <typename T>
void hex_print(const T& value) {
    auto begin = reinterpret_cast<const BYTE*>(&value);
    auto end = reinterpret_cast<const BYTE*>(&value + 1);
    for (auto ptr = begin; ptr < end; ++ptr) {
        std::cout << std::hex << *ptr;
    }
}

template <typename T>
std::enable_if_t<std::is_integral_v<T>> free_printer(T value) {
    std::cout << std::hex << value;
}

void free_printer(const std::wstring& value) {
    std::wcout<< value << endlind;
}

template <typename T>
void free_printer(const std::vector<T>& value) {
    IndentGuard g;
    std::cout << endlind;
    for (const auto& val : value) {
        free_printer(val);
    }
}

void free_printer(const GUID& value) {
    RPC_WSTR rpcStr{};
    DeviceManagerUtils::RPCCheck(UuidToString(&value, &rpcStr));
    std::wstring nameStr = reinterpret_cast<wchar_t*>(rpcStr);
    DeviceManagerUtils::RPCCheck(RpcStringFree(&rpcStr));
    free_printer(nameStr);
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

void free_printer(const std::map<std::wstring, DevicePropertyData>& value) {
    IndentGuard g;
    for (const auto& val : value) {
        std::wcout << val.first << ": ";
        free_printer(val.second);
        std::cout << std::endl;
    }
}

void free_printer(DeviceNode& devNode) {
    auto deviceProperties = devNode.properties();
    auto devicePropetriesMap = getPropertyMap(deviceProperties);
    auto [status, problem] = devNode.statusAndProblem();

    std::cout << "Device named ";
    free_printer(devicePropetriesMap[L"DEVPKEY_Device_FriendlyName"]);
    std::cout << " status " << std::hex << status;
    if (problem) {
        std::cout << " Reported problem " << problem;
    }
    std::cout << endlind << "Properties:" << endlind;
    IndentGuard g;
    free_printer(devicePropetriesMap);
    
}
using MyTest = std::variant<int, std::monostate>;

int main() {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    //Device root = getDeviceTreeByConnection();
    auto rootDevice = DeviceNode::rootDevice();
    free_printer(rootDevice);

//     CM_Locate_DevNode(&root.instance, nullptr, CM_LOCATE_DEVNODE_NORMAL);
//     std::queue<Device> devices;
//     devices.push(root);
//     int totalDevices = 0;
//     while (!devices.empty()) {
//         std::cout << ++totalDevices << std::endl;
//         auto& currDevice = devices.front();
//         DEVINST currentChild{};
//         auto result = CM_Get_Child(&currentChild, currDevice.instance, 0);
//         DEVINST nextChild{};
//         while (result == 0) {
//             devices.push({ currentChild });
//             currDevice.children.push_back({ currentChild });
//             result = CM_Get_Sibling(&nextChild, currentChild, 0);
//             currentChild = nextChild;
//         }
//         devices.pop();
//     }



    return 0;
}