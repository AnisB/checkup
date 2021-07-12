// SDK includes
#include <bento_memory/system_allocator.h>
#include <bento_math/vector4.h>
#include <checkup_graphics/backend.h>
#include <checkup_graphics/shader_collection.h>

int main(int, char**)
{
    // Create an allocator for our application
    bento::SystemAllocator systemAllocator;

    // Create the rendering context
    checkup::graphics::TGPUBackend* gpuBackend = checkup::graphics::initialize_gpu_backend("Ready to go", 1920, 1080, systemAllocator);

    // Compile the target shader
    uint32_t program = checkup::graphics::create_program(gpuBackend, checkup::graphics::full_screen_quad_vs, checkup::graphics::uniform_color_fs, systemAllocator);
    checkup::graphics::destroy_program(gpuBackend, program);

    // Infite render loop
    uint32_t frameIdx = 0;
    while (checkup::graphics::get_backend_state(gpuBackend), frameIdx < 32)
    {
        // Clear the render buffer 
        checkup::graphics::set_clear_color(gpuBackend, bento::vector4(1.0f, 0.0f, 0.0f, 1.0f));
        checkup::graphics::clear_color_buffer(gpuBackend);

        // Display on screen
        checkup::graphics::swap_buffers(gpuBackend);

        // Wait 33 miliseconds
        SLEEP_FUNCTION(33);
        frameIdx++;
    }

    // Release the gpu backend
    checkup::graphics::release_gpu_backend(gpuBackend);

    // We are done!
    return 0;
}