#pragma once

// Base includes
#include <float.h>

// Bento includes
#include <bento_base/log.h>
#include <bento_memory/common.h>
#include <bento_math/types.h>
#include <bento_collection/dynamic_string.h>

#include "checkup_rest/request.h"
#include "checkup_route/route.h"

namespace checkup
{
    struct TRouteViewport
    {
        ALLOCATOR_BASED;
        TRouteViewport(bento::IAllocator& allocator)
        : _allocator(allocator)
        , origin(allocator, "INVALID")
        , destination(allocator, "INVALID")
        , name(allocator, "INVALID")
        , vehicle(allocator, "INVALID")
        , startX(0)
        , startY(0)
        , width(0)
        , height(0)
        , debugColor()
        {
        }

        bento::DynamicString origin;
        bento::DynamicString destination;
        bento::DynamicString name;
        bento::DynamicString vehicle;
        float startX;
        float startY;
        float width;
        float height;
        bento::Vector3 debugColor;
        bento::IAllocator& _allocator;
    };

	enum class TransportMode
	{
		Car,
		Bicycle,
		Invalid
	};

    struct TRouteInfo
    {
        ALLOCATOR_BASED;
        TRouteInfo(bento::IAllocator& allocator)
        : _allocator(allocator)
        , duration(0)
        , length(0)
        , baseDuration(0)
        , typicalDuration(0)
        {
        }
        uint32_t duration;
        float length;
        uint32_t typicalDuration;
        uint32_t baseDuration;
        bento::IAllocator& _allocator;
    };

    void build_route_request(const char* originCoords, const char* destinationCoords, TransportMode mode, const char* token, TRequest& request);
    bool build_route_data(const bento::DynamicString& routeData, TRouteInfo& routeInfo, bento::ILogger* logger = nullptr);
}