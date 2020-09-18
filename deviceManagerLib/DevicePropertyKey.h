#pragma once
#include "DevicePropertyName.h"
#include "DevicePropertyData.h"
#include <Windows.h>
#include <cfgmgr32.h>
#include <string>

class DevicePropertyKey {
public:
    DevicePropertyKey(DEVINST deviceInstance, const DEVPROPKEY& propKey);

    std::wstring getPropertyName();

    DevicePropertyData getPropertyValue();

private:
    DEVINST m_propertyDevice;
    DEVPROPKEY m_propertyKey;
};

