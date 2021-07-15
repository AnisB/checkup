#pragma once

// Base includes
#include <float.h>
#include <map>
#include <string>

// Bento includes
#include <bento_base/log.h>
#include <bento_memory/common.h>
#include <bento_math/types.h>
#include <bento_collection/dynamic_string.h>

#include "checkup_rest/request.h"
#include "checkup_route/route.h"

namespace checkup
{
    struct TRATPViewport
    {
        ALLOCATOR_BASED;
        TRATPViewport(bento::IAllocator& allocator)
        : _allocator(allocator)
        , name(allocator, "INVALID")
        , line(allocator)
        , startX(0)
        , startY(0)
        , width(0)
        , height(0)
        , debugColor()
        {
        }

        bento::DynamicString name;
        bento::DynamicString line;
        float startX;
        float startY;
        float width;
        float height;
        bento::Vector3 debugColor;
        bento::IAllocator& _allocator;
    };

    struct TRATPInfo
    {
        std::map<std::string, std::string> lines_message;
    };


    void build_lines_from_string(const char* data, bento::Vector<bento::DynamicString>& outputArray);
    void build_ratp_request(TRequest& request);
    bool build_ratp_data(const bento::DynamicString& ratpData, TRATPInfo& ratpInfo, bento::ILogger* logger = nullptr);
}