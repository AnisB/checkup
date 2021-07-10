// Bento includes
#include <bento_memory/system_allocator.h>
#include <bento_memory/common.h>

// Internal includes
#include "allocator_c_api.h"

static bento::SystemAllocator _base_allocator;

CheckUpAllocatorObject* checkup_create_allocator()
{
	return (CheckUpAllocatorObject*)bento::make_new<bento::SystemAllocator>(_base_allocator);
}

void checkup_destroy_allocator(CheckUpAllocatorObject* allocator)
{
	bento::SystemAllocator* sys_alloc = (bento::SystemAllocator*)allocator;
	bento::make_delete<bento::SystemAllocator>(_base_allocator, sys_alloc);
}
