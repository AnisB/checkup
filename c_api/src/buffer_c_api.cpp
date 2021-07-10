// Bento includes
#include <bento_collection/vector.h>

// Internal include
#include "buffer_c_api.h"

CheckUpBufferObject* checkup_create_buffer(CheckUpAllocatorObject* alloc, uint32_t size, unsigned char* input_buffer)
{
	bento::IAllocator* alloc_ptr = (bento::IAllocator*)alloc;
	bento::Vector<char>* buffer = bento::make_new<bento::Vector<char>>(*alloc_ptr, *alloc_ptr);
	buffer->resize(size);
	if(size)
	{
		memcpy(buffer->begin(), input_buffer, size);
	}
	return (CheckUpBufferObject*)buffer;
}

uint32_t checkup_buffer_size(CheckUpBufferObject* buffer)
{
	bento::Vector<char>* buffer_ptr = (bento::Vector<char>*)buffer;
	return buffer_ptr->size();
}

void checkup_buffer_get_data(CheckUpBufferObject* buffer, unsigned char* output_buffer)
{
	bento::Vector<char>* buffer_ptr = (bento::Vector<char>*)buffer;
	uint32_t buff_size = buffer_ptr->size();
	if(buff_size)
	{
		memcpy(output_buffer, buffer_ptr->begin(), buffer_ptr->size());
	}
}

void checkup_destroy_buffer(CheckUpBufferObject* buffer)
{
	bento::Vector<char>* buffer_ptr = (bento::Vector<char>*)buffer;
	bento::make_delete<bento::Vector<char>>(*(buffer_ptr->_allocator), buffer_ptr);
}