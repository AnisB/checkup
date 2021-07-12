// Bento includes
#include <bento_base/log.h>

// SDK includes
#include "checkup_graphics/platform.h"
#include "checkup_graphics/backend.h"

// External includes
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <string>

namespace checkup
{
    // Tracks the global state of the backend
    static bool valid_backend = false;

    namespace graphics
    {
        GLuint load_shader(GLenum shaderType, const char* shaderSource, bento::IAllocator& allocator)
        {
            GLuint shader = glCreateShader(shaderType);
            if (shader != 0)
            {
                glShaderSource(shader, 1, &shaderSource, NULL);
                glCompileShader(shader);

                GLint compiled = 0;
                glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

                if (compiled != GL_TRUE)
                {
                    GLint infoLen = 0;
                    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

                    if (infoLen > 0)
                    {
                        // Log the errors
                        char* logBuffer = (char*)allocator.allocate(infoLen, 4);
                        glGetShaderInfoLog(shader, infoLen, NULL, logBuffer);
                        auto logger = bento::default_logger();
                        logger->log(bento::LogLevel::error, "SHADER COMPILATION", logBuffer);
                        allocator.deallocate(logBuffer);

                        // Delete the shader
                        glDeleteShader(shader);
                        return 0;
                    }
                }
            }

            return shader;
        }

        uint32_t create_program(TGPUBackend*, const char* vertexSource, const char* fragmentSource, bento::IAllocator& allocator)
        {
            // Compile both shaders
            GLuint vertexShader = load_shader(GL_VERTEX_SHADER, vertexSource, allocator);
            GLuint fragmentShader = load_shader(GL_FRAGMENT_SHADER, fragmentSource, allocator);

            // Check and destroy if needed
            bool validShaders = (vertexShader != 0) && (fragmentShader != 0);
            if (!validShaders)
            {
                if (vertexShader != 0)
                    glDeleteShader(vertexShader);
                if (fragmentShader != 0)
                    glDeleteShader(fragmentShader);
                return 0;
            }

            GLuint program = glCreateProgram();
            if (program != 0)
            {
                glAttachShader(program, vertexShader);
                glAttachShader(program, fragmentShader);
                glLinkProgram(program);
                GLint linkStatus = GL_FALSE;
                glGetProgramiv(program , GL_LINK_STATUS, &linkStatus);

                if(linkStatus != GL_TRUE)
                {
                    GLint bufLength = 0;
                    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);

                    if (bufLength > 0)
                    {
                        char* logBuffer = (char*)allocator.allocate(bufLength, 4);
                        glGetProgramInfoLog(program, bufLength, NULL, logBuffer);
                        auto logger = bento::default_logger();
                        logger->log(bento::LogLevel::error, "SHADER COMPILATION", logBuffer);
                        allocator.deallocate(logBuffer);
                    }
                    glDeleteProgram(program);
                    return 0;
                }
            }
            return (uint32_t)program;
        }

        void destroy_program(TGPUBackend*, uint32_t rasterProgram)
        {
            GLuint program = (GLuint)rasterProgram;
            glDeleteProgram(program);
        }

        void set_viewport_size(TGPUBackend*, uint32_t width, uint32_t height)
        {
			glViewport(0, 0, width, height);
        }

        void set_clear_color(TGPUBackend*, const bento::Vector4& color)
        {
    		glClearColor(color.x, color.y, color.z, color.w);
        }

        void clear_color_buffer(TGPUBackend*)
        {
    		glClear(GL_COLOR_BUFFER_BIT);
        }

        void clear_depth_buffer(TGPUBackend*)
        {
    		glClear(GL_DEPTH_BUFFER_BIT);
        }

        void clear_color_and_depth_buffer(TGPUBackend*)
        {
    		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        }

        struct TEGLGPUBackend
        {
            EGLDisplay display;
            EGLContext context;
            EGLSurface surface;
            EGLNativeWindowType native_window;
        };

        TGPUBackend* initialize_gpu_backend(const char* windowName, uint32_t width, uint32_t height, bento::IAllocator& allocator)
        {
            // Create the rendering context
            TEGLGPUBackend* gpuBackend = bento::make_new<TEGLGPUBackend>(allocator);

            /* create a native window */
            gpuBackend->native_window = (EGLNativeWindowType)create_native_window(windowName, width, height);

            /* get an EGL display connection */
            gpuBackend->display = eglGetDisplay((EGLNativeDisplayType)get_native_display((TNativeWindowType*)gpuBackend->native_window));

            // Initialize EGL for this display, returns EGL version
            EGLint eglVersionMajor, eglVersionMinor;
            eglInitialize(gpuBackend->display, &eglVersionMajor, &eglVersionMinor);

            // Bind the opengl es api
            eglBindAPI(EGL_OPENGL_ES_API);

            EGLint attributes[] =
            {
                // Request OpenGL ES 2.0 configs 
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_NONE
            };
            EGLint numConfigs;
            EGLConfig configs[1];
            // Establish the config
            eglChooseConfig(gpuBackend->display, attributes, configs, 1, &numConfigs);

            // Create the window surface
            EGLint surfaceAttributes[] = { EGL_NONE };
            gpuBackend->surface = eglCreateWindowSurface(gpuBackend->display, configs[0], gpuBackend->native_window, surfaceAttributes);
            
            EGLint context_attributes[] = { EGL_CONTEXT_MAJOR_VERSION, 3,
                EGL_CONTEXT_MINOR_VERSION, 1,
                EGL_NONE };
            gpuBackend->context = eglCreateContext(gpuBackend->display, configs[0], EGL_NO_CONTEXT, context_attributes);

            /* connect the context to the surface */
            eglMakeCurrent(gpuBackend->display, gpuBackend->surface, gpuBackend->surface, gpuBackend->context);
            
            // Mark the backend as valid
            valid_backend = true;

            // Return the opaque rendering context
            return (TGPUBackend*)gpuBackend;
        }

        void release_gpu_backend(TGPUBackend* gpuBackendPtr)
        {
            TEGLGPUBackend* gpuBackend =  (TEGLGPUBackend*)gpuBackendPtr;
            eglDestroyContext(gpuBackend->display, gpuBackend->context);
            eglDestroySurface(gpuBackend->display, gpuBackend->surface);
            eglTerminate(gpuBackend->display);
            destroy_native_window((TNativeWindowType*)gpuBackend->native_window);
        }

        bool get_backend_state(TGPUBackend* gpuBackendPtr)
        {
            TEGLGPUBackend* gpuBackend = (TEGLGPUBackend*)gpuBackendPtr;
            return valid_backend && get_window_state((TNativeWindowType*)(gpuBackend->native_window));
        }

        void swap_buffers(TGPUBackend* gpuBackendPtr)
        {
            TEGLGPUBackend* gpuBackend = (TEGLGPUBackend*)gpuBackendPtr;
            eglSwapBuffers(gpuBackend->display, gpuBackend->surface);
        }
    }
}