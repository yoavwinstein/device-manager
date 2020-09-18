#include "DeviceNode.h"
#include "Printing.h"
#include "JsonSerialization.h"
#include <stack>
#include <cpprest/json.h>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

void jsonPrint() {
    web::json::value serializedDeviceTree;
    auto rootDevice = DeviceNode::rootDevice();
    json_serialize(serializedDeviceTree, rootDevice);
    std::wcout << serializedDeviceTree.serialize();
}

void normalPrint() {
    using StackMember = std::variant<DeviceNode, IndentGuard>;
    std::stack<StackMember> stack;
    stack.push(DeviceNode::rootDevice());
    while (!stack.empty()) {
        std::visit(
            overloaded{
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
}

int main(int argc, const char** argv) {
    bool shouldJsonPrint = false;
    if (argc >= 2 && !strcmp(argv[1], "--json")) {
        shouldJsonPrint = true;
    }

    if (shouldJsonPrint) {
        jsonPrint();
    } else {
        normalPrint();
    }
    
    return 0;
}