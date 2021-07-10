#pragma once

// Bento includes
#include <bento_memory/common.h>
#include <bento_collection/dynamic_string.h>

namespace checkup
{
	struct TRequest
	{
		ALLOCATOR_BASED;
		TRequest(bento::IAllocator& allocator)
		: validity(false)
		, api(allocator, "INVALID")
		, content(allocator, "INVALID")
		, result(allocator, "INVALID")
		{
		}
		bool validity;
		bento::DynamicString api;
		bento::DynamicString content;
		bento::DynamicString result;
	};
}