#pragma once

// Bento includes
#include <bento_memory/common.h>
#include <bento_math/types.h>

namespace checkup
{
    namespace graphics
    {
    	struct TGPUBackend;

    	// Initialize and release the gpu backend
        TGPUBackend* initialize_gpu_backend(const char* windowName, uint32_t width, uint32_t height, bento::IAllocator& allocator);
        void release_gpu_backend(TGPUBackend* gpuBackend);

        // Fetch the state of the backend
        bool get_backend_state(TGPUBackend* gpuBackend);

    	// Rasterization pipeline
        uint32_t create_program(TGPUBackend* context, const char* vertexShader, const char* fragmentShader, bento::IAllocator& allocator);
        void destroy_program(TGPUBackend* context, uint32_t rasterProgram);

        // Utility
        void set_viewport_size(TGPUBackend* context, uint32_t width, uint32_t height);
        void set_clear_color(TGPUBackend* context, const bento::Vector4& color);
        void clear_color_buffer(TGPUBackend* context);
        void clear_depth_buffer(TGPUBackend* context);
        void clear_color_and_depth_buffer(TGPUBackend* context);
        void swap_buffers(TGPUBackend* gpuBackend);
    }
}