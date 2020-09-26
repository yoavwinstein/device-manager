#include "pch.h"
#include "FileActivationEvent.h"
#include <vector>

static std::function<void(winrt::Microsoft::ReactNative::JSValue&)> s_fileActivationEvents;
static std::vector<winrt::Microsoft::ReactNative::JSValue> s_pendingValues;

void registerForFileActivationEvents(std::function<void (winrt::Microsoft::ReactNative::JSValue&)>&& func) {
	s_fileActivationEvents = func;
	for (auto& pendingJsonValue : s_pendingValues) {
		s_fileActivationEvents(pendingJsonValue);
	}
	s_pendingValues.clear();
}

void dispatchFileActivationEvent(winrt::Microsoft::ReactNative::JSValue& value) {
	if (s_fileActivationEvents) {
		s_fileActivationEvents(value);
	} else {
		s_pendingValues.push_back(std::move(value));
	}
}

