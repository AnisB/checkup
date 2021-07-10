#pragma once

// Internal includes
#include "types_c_api.h"

// External includes
#include <stdint.h>

extern "C"
{
	// Create a buffer with a given size and value
	C_API_EXPORT CheckUpBufferObject* checkup_create_buffer(CheckUpAllocatorObject* alloc, uint32_t size, unsigned char* input_buffer);

	// Get the size of the buffer object
	C_API_EXPORT uint32_t checkup_buffer_size(CheckUpBufferObject* buffer);

	// Copy the buffer into a pointer
	C_API_EXPORT void checkup_buffer_get_data(CheckUpBufferObject* buffer, unsigned char* output_buffer);

	// Destroy a previously created buffer
	C_API_EXPORT void checkup_destroy_buffer(CheckUpBufferObject* buffer);
}