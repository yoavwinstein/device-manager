#pragma once
#include "DevicePropertyKey.h"
#include "DevicePropertyData.h"
#include <Windows.h>
#include <tuple>
#include <vector>
#include <string>


class DeviceNode {
public:
    using DeviceProperty = std::pair<std::wstring, DevicePropertyData>;

    static DeviceNode rootDevice();

    std::tuple<ULONG, ULONG> statusAndProblem();
    std::vector<DeviceNode> children();
    std::vector<DevicePropertyKey> properties();

private:
    DeviceNode(DEVINST deviceInstance);
    DEVINST m_deviceInstance;
};
