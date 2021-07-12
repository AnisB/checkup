#pragma once

// Bento includes
#include <bento_memory/common.h>

namespace checkup
{
    namespace graphics
    {
		struct TNativeWindowType;
		struct TNativeDisplayType;
		TNativeWindowType* create_native_window(const char* name, uint32_t width, uint32_t height, bento::IAllocator& allocator);
		void destroy_native_window(TNativeWindowType* window);
		TNativeDisplayType* get_native_display(TNativeWindowType* window);
		void show_window(TNativeWindowType* window);
		void hide_window(TNativeWindowType* window);
		bool get_window_state(TNativeWindowType* window);
	}
}
