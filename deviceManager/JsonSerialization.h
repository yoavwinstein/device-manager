#pragma once
#include "DeviceNode.h"
#include <cpprest/json.h>

void json_serialize(web::json::value& obj, const DeviceNode& devNode);
