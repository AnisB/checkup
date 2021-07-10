#pragma once

#include "types_c_api.h"

extern "C"
{
	// Function to create a new checkup allocator
	C_API_EXPORT CheckUpAllocatorObject* orma_create_allocator();

	// Function to destroy a checkup allocator
	C_API_EXPORT void checkup_destroy_allocator(CheckUpAllocatorObject* allocator);
}