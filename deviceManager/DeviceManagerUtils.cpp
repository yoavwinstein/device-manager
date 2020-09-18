#include "DeviceManagerUtils.h"


namespace DeviceManagerUtils {

std::vector<std::wstring> split(const std::wstring& str, const std::wstring& delim) {
    std::vector<std::wstring> tokens;
    size_t prev = 0, pos = 0;
    do {
        pos = str.find(delim, prev);
        if (pos == std::wstring::npos) pos = str.length();
        std::wstring token = str.substr(prev, pos - prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}

CMException::CMException(CONFIGRET v) : value(v) {}

void CMCheck(CONFIGRET value) {
    if (value != CR_SUCCESS) {
        throw CMException(value);
    }
}

RPCException::RPCException(RPC_STATUS status) : value(status) {}

void RPCCheck(RPC_STATUS status) {
    if (status != 0) {
        throw RPCException(status);
    }
}

}
