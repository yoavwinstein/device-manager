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
        NativeModule() {
            registerForFileActivationEvents([&](winrt::Microsoft::ReactNative::JSValue& val) {
                OnFileActivated(val);
            });
        }

        REACT_METHOD(Init, L"init");
        void Init() noexcept {
            /*auto dispatcher = winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().Dispatcher();
            if (dispatcher) {
                co_await dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
                                             []() {
                    auto currentWindow = winrt::Windows::UI::Xaml::Window::Current();
                    if (currentWindow) {
                        // todo: stuff
                    }
                });
            }*/
        }

        REACT_EVENT(OnFileActivated, L"onFileActivated");
        std::function<void (React::JSValue&)> OnFileActivated;
    };
}