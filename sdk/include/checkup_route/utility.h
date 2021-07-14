#pragma once

// Bento includes 
#include <bento_math/types.h>
#include <bento_memory/common.h>
#include <bento_collection/dynamic_string.h>

// SDK includes
#include "checkup_route/route.h"

// External includes
#include <float.h>

namespace checkup
{
    TransportMode string_to_transport(const char* transport);
    const char* transport_to_string(TransportMode transport);
}