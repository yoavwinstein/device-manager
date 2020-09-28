#pragma once

#include "pch.h"

#include <functional>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>

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

            auto dispatcher = winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().Dispatcher();
            if (dispatcher != nullptr) {
                dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
                                    [&]() {
                    auto currentWindow = winrt::Windows::UI::Xaml::Window::Current();
                    if (currentWindow != nullptr) {
                        auto currentContent = currentWindow.Content();
                        if (currentContent != nullptr) {
                            currentContent.AllowDrop(true);

                            using namespace winrt::Windows::UI::Xaml;
                            currentContent.DragOver([](auto sender, const DragEventArgs args) -> winrt::IAsyncAction {
                                args.AcceptedOperation(winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation::Copy);
                            });
                            currentContent.DragLeave([](auto sender, const DragEventArgs args) {
                                // Nothing!
                            });
                            currentContent.Drop([&](auto sender, const DragEventArgs args) -> winrt::IAsyncAction {
                                if (OnFileActivated) {
                                    auto items = co_await args.DataView().GetStorageItemsAsync();
                                    if (items.Size() > 0) {
                                        auto file = items.GetAt(0).as<winrt::Windows::Storage::StorageFile>();
                                        auto buf = co_await winrt::Windows::Storage::FileIO::ReadBufferAsync(file);
                                        std::string content;
                                        content.resize(buf.Length());
                                        memcpy(content.data(), buf.data(), buf.Length());
                                        winrt::Microsoft::ReactNative::JSValue value(content);
                                        OnFileActivated(std::move(value));
                                    }
                                }
                            });
                        }
                    }
                });
            }
        }

        REACT_EVENT(OnFileActivated, L"onFileActivated");
        std::function<void (React::JSValue&)> OnFileActivated;
    };
}