#pragma once

#include "pch.h"

#include <functional>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>

#include "NativeModules.h"
#include "FileActivationEvent.h"
#include <thread>

namespace deviceManagerUI
{
    REACT_MODULE(NativeModule);
    struct NativeModule
    {
        REACT_METHOD(Init, L"init");
        void Init() noexcept {
            registerForFileActivationEvents([&](winrt::Microsoft::ReactNative::JSValue& val) {
                if (OnFileActivated) {
                    OnFileActivated(val);
                }
            });
        }

        REACT_EVENT(OnFileActivated, L"onFileActivated");
        std::function<void (React::JSValue&)> OnFileActivated;
    };
}