#include "DeviceNode.h"
#include "DeviceManagerUtils.h"

DeviceNode DeviceNode::rootDevice() {
    DEVINST root{};
    DeviceManagerUtils::CMCheck(CM_Locate_DevNode(&root, nullptr, CM_LOCATE_DEVNODE_NORMAL));
    return root;
}

std::tuple<ULONG, ULONG> DeviceNode::statusAndProblem() {
    ULONG status{};
    ULONG problem{};
    DeviceManagerUtils::CMCheck(CM_Get_DevNode_Status(&status, &problem, m_deviceInstance, 0));
    return { status, problem };
}

std::vector<DeviceNode> DeviceNode::children() {
    std::vector<DeviceNode> returnValue;
    DEVINST currentChild{};
    DEVINST nextChild{};
    auto res = CM_Get_Child(&currentChild, m_deviceInstance, 0);
    while (res == CR_SUCCESS) {
        returnValue.push_back(currentChild);
        res = CM_Get_Sibling(&nextChild, currentChild, 0);
        currentChild = nextChild;
    }
    if (res != CR_NO_SUCH_DEVNODE) {
        throw DeviceManagerUtils::CMException(res);
    }
    return returnValue;
}

std::vector<DevicePropertyKey> DeviceNode::properties() {
    std::vector<DevicePropertyKey> returnValue{};

    ULONG propertiesLength{};
    auto result = CM_Get_DevNode_Property_Keys(m_deviceInstance, nullptr, &propertiesLength, 0);
    if (result != CR_BUFFER_SMALL) {
        throw DeviceManagerUtils::CMException(result);
    }
    std::vector<DEVPROPKEY> propKeys(propertiesLength);
    DeviceManagerUtils::CMCheck(CM_Get_DevNode_Property_Keys(m_deviceInstance, propKeys.data(), &propertiesLength, 0));

    for (auto& propKey : propKeys) {
        returnValue.push_back({ m_deviceInstance, propKey });
    }

    return returnValue;
}

DeviceNode::DeviceNode(DEVINST deviceInstance) : m_deviceInstance(deviceInstance) {}
