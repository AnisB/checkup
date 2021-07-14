// Bento includes
#include <bento_base/log.h>

// SDK includes
#include "checkup_route/utility.h"

namespace checkup
{
   
    TransportMode string_to_transport(const char* transport)
    {
        if (strcmp(transport, "car") == 0)
            return TransportMode::Car;
        if (strcmp(transport, "bicyle") == 0)
            return TransportMode::Bicycle;
        return TransportMode::Invalid;
    }

    const char* transport_to_string(TransportMode transport)
    {
        switch (transport)
        {
        case TransportMode::Car:
            return "car";
        case TransportMode::Bicycle:
            return "bicycle";
        }
        return "INVALID";
    }
}
