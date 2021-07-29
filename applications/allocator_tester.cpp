// SDK includes
#include <bento_base/security.h>
#include <bento_memory/common.h>
#include <bento_memory/page_allocator.h>

struct TByte4
{
	int data;
};

struct TByte8
{
	double data;
};

struct TByte16
{
	bool data[16];
};

struct TByte32
{
	float data[8];
};

int main()
{
	// Create an allocator that allocates 16 bytes per chunk
	bento::PageAllocator pageAllocator;
	pageAllocator.initialize(16);

	// Tests that the structure size correctly accounts for the allocation size
	{
		TByte4* byte4 = bento::make_new<TByte4>(pageAllocator);
		assert(byte4 != nullptr);
		assert(pageAllocator.usage_flags() == 0x00000001);
		bento::make_delete<TByte4>(pageAllocator, byte4);

		TByte8* byte8 = bento::make_new<TByte8>(pageAllocator);
		assert(byte8 != nullptr);
		assert(pageAllocator.usage_flags() == 0x00000001);
		bento::make_delete<TByte8>(pageAllocator, byte8);

		TByte16* byte16 = bento::make_new<TByte16>(pageAllocator);
		assert(byte16 != nullptr);
		assert(pageAllocator.usage_flags() == 0x00000001);
		bento::make_delete<TByte16>(pageAllocator, byte16);

		TByte32* byte32 = bento::make_new<TByte32>(pageAllocator);
		assert(byte32 == nullptr);
		assert(pageAllocator.usage_flags() == 0x00000000);
		bento::make_delete<TByte32>(pageAllocator, byte32);
	}

	// Tests that freeing a given chunk works correctly and that chunk can be re-attributed
	{
		// Allocate the first three chunks
		TByte16* c0 = bento::make_new<TByte16>(pageAllocator);
		assert(c0 != nullptr);
		assert(pageAllocator.usage_flags() == 0x00000001);
		TByte16* c1 = bento::make_new<TByte16>(pageAllocator);
		assert(c1 != nullptr);
		assert(pageAllocator.usage_flags() == 0x00000003);
		TByte16* c2 = bento::make_new<TByte16>(pageAllocator);
		assert(c2 != nullptr);
		assert(pageAllocator.usage_flags() == 0x00000007);

		// Free the middle chunk
		bento::make_delete<TByte16>(pageAllocator, c1);
		assert(pageAllocator.usage_flags() == 0x00000005);

		// Make sure the new data gets reallocated into the middle chunk
		TByte16* c1_bis = bento::make_new<TByte16>(pageAllocator);
		assert(c1_bis != nullptr);
		assert(pageAllocator.usage_flags() == 0x00000007);

		// Delete all the structures
		bento::make_delete<TByte16>(pageAllocator, c0);
		bento::make_delete<TByte16>(pageAllocator, c1_bis);
		bento::make_delete<TByte16>(pageAllocator, c2);
	}

	// Tests that when the page is full, the allocator returns null
	{
		// Allocate the 64 chunks of the page
		TByte16* c[64];
		for (uint32_t chunkIdx = 0; chunkIdx < 64; ++chunkIdx)
		{
			// Allocate the first three chunks
			c[chunkIdx] = bento::make_new<TByte16>(pageAllocator);
			assert(c[chunkIdx] != nullptr);
		}

		// Make sure the usage flag is full
		assert(pageAllocator.is_full());

		TByte16* tooMuch = bento::make_new<TByte16>(pageAllocator);
		assert(tooMuch == nullptr);
		assert(pageAllocator.is_full());

		// Free all the data (top to bottom this time)
		for (int32_t chunkIdx = 63; chunkIdx >= 0; --chunkIdx)
			bento::make_delete<TByte16>(pageAllocator, c[chunkIdx]);
		
		// Make sure everything was freed
		assert(pageAllocator.usage_flags() == 0x0000000000);
	}

	// If we got here, everything is fine
	return 0;
}