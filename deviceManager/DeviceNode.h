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

    std::tuple<ULONG, ULONG> statusAndProblem() const;
    std::vector<DeviceNode> children() const;
    std::vector<DevicePropertyKey> properties() const;

private:
    DeviceNode(DEVINST deviceInstance);
    DEVINST m_deviceInstance;
};
