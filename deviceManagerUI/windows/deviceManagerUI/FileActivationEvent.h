#pragma once
#include "NativeModules.h"
#include <functional>

void registerForFileActivationEvents(std::function<void (winrt::Microsoft::ReactNative::JSValue&)>&& func);
void dispatchFileActivationEvent(winrt::Microsoft::ReactNative::JSValue& value);
