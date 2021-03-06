#pragma once
#include <vector>
#include <string>
#include <Windows.h>
#include <cfgmgr32.h>


namespace DeviceManagerUtils {

std::vector<std::wstring> split(const std::wstring& str, const std::wstring& delim);

class CMException {
public:
    CMException(CONFIGRET v);
    CONFIGRET value;
};

void CMCheck(CONFIGRET value);

class RPCException {
public:
    RPCException(RPC_STATUS status);
    RPC_STATUS value;
};

class Win32Exception {
public:
    Win32Exception();
    DWORD error;
};

void RPCCheck(RPC_STATUS status);
void WIN32Check(BOOL isSuccess);

}