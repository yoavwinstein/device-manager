#include "DeviceNode.h"
#include "Printing.h"
#include <stack>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

int main() {
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