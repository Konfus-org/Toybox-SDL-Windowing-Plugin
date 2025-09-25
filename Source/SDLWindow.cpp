#include "SDLWindow.h"
#include <Tbx/Events/WindowEvents.h>
#include <Tbx/Debug/Debugging.h>
#include <SDL3/SDL_video.h>

namespace SDLWindowing
{
    SDLWindow::~SDLWindow()
    {
        Close();
    }

    Tbx::NativeHandle SDLWindow::GetNativeHandle() const
    {
        return SDL_GetDisplayForWindow(_window);
    }

    Tbx::NativeWindow SDLWindow::GetNativeWindow() const
    {
        return _window;
    }

    void SDLWindow::Open()
    {
        Uint32 flags = SDL_WINDOW_RESIZABLE;

        if (_useOpenGl)
        {
            // Set attribute
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#ifdef TBX_DEBUG
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
            // Validate attributes
            int att = 0;
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &att);
            TBX_ASSERT(att == 4, "Failed to set OpenGL context major version to 4");
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &att);
            TBX_ASSERT(att == 5, "Failed to set OpenGL context minor version to 5");
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &att);
            TBX_ASSERT(att == SDL_GL_CONTEXT_PROFILE_CORE, "Failed to set OpenGL context profile to core");
#ifdef TBX_DEBUG
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &att);
            TBX_ASSERT(att == SDL_GL_CONTEXT_DEBUG_FLAG, "Failed to set OpenGL context debug flag");
#endif

            flags |= SDL_WINDOW_OPENGL;
        }

        switch (_currentMode)
        {
            using enum Tbx::WindowMode;
            case Windowed:   break;
            case Fullscreen: flags |= SDL_WINDOW_FULLSCREEN; break;
            case Borderless: flags |= SDL_WINDOW_BORDERLESS; break;
            case FullscreenBorderless: 
            {
                flags |= SDL_WINDOW_FULLSCREEN;
                flags |= SDL_WINDOW_BORDERLESS;
                break;
            }
            default:
            {
                TBX_ASSERT(false, "Invalid window mode");
            }
        }

        _window = SDL_CreateWindow(_title.c_str(), _size.Width, _size.Height, flags);
        TBX_ASSERT(_window, "SDL_CreateWindow failed: %s", SDL_GetError());

        if (_useOpenGl)
        {
            _glContext = SDL_GL_CreateContext(_window);
            TBX_ASSERT(_glContext, "Failed to create gl context for window!");
            SDL_GL_MakeCurrent(_window, _glContext);
        }
    }

    void SDLWindow::Close()
    {
        // We've already been closed...
        if (_window == nullptr) return;

        // Cleanup...
        SDL_DestroyWindow(_window);
        _window = nullptr;

        // Reset the context
        SDL_GL_DestroyContext(_glContext);
    }

    void SDLWindow::Update()
    {
        SDL_Event e;
        SDL_PollEvent(&e);
        switch (e.type)
        {
            case SDL_EVENT_QUIT:
            {
                Close();
                break;
            }
            case SDL_EVENT_WINDOW_RESIZED:
            {
                if (e.window.windowID == SDL_GetWindowID(_window))
                {
                    int w, h;
                    SDL_GetWindowSize(_window, &w, &h);
                    SetSize(Tbx::Size(w, h));
                }
                break;
            }
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
            {
                if (e.window.windowID ==  SDL_GetWindowID(_window))
                {
                    Focus();
                }
                break;
            }
            case SDL_EVENT_WINDOW_FOCUS_LOST:
            {
                if (e.window.windowID == SDL_GetWindowID(_window))
                {
                    _isFocused = false;
                }
            }
        }

        if (_useOpenGl)
        {
            SDL_GL_SwapWindow(_window);
        }
    }

    void SDLWindow::Focus()
    {
        _isFocused = true;
        SDL_RaiseWindow(_window);
        if (_useOpenGl)
        {
            SDL_GL_MakeCurrent(_window, _glContext);
        }
    }

    bool SDLWindow::IsClosed()
    {
        return _isClosed;
    }

    bool SDLWindow::IsFocused()
    {
        return _isFocused;
    }

    const std::string& SDLWindow::GetTitle() const
    {
        return _title;
    }

    void SDLWindow::SetTitle(const std::string& title)
    {
        _title = title;

        if (!_window)
        {
            return;
        }

        SDL_SetWindowTitle(_window, title.c_str());
    }

    const Tbx::Size& SDLWindow::GetSize() const
    {
        return _size;
    }

    void SDLWindow::SetSize(const Tbx::Size& size)
    {
        _size = size;

        if (!_window)
        {
            return;
        }

        SDL_SetWindowSize(_window, _size.Width, _size.Height);
    }

    void SDLWindow::SetMode(const Tbx::WindowMode& mode)
    {
        _currentMode = mode;
        if (_window == nullptr)
        {
            return;
        }

        switch (_currentMode)
        {
            using enum Tbx::WindowMode;
            case Windowed:
            {
                SDL_SetWindowFullscreen(_window, false);
                SDL_SetWindowBordered(_window, true);
                break;
            }
            case Fullscreen:
            {
                SDL_SetWindowFullscreen(_window, true);
                break;
            }
            case Borderless:
            {
                SDL_SetWindowFullscreen(_window, false);
                SDL_SetWindowBordered(_window, false);
                break;
            }
            case FullscreenBorderless:
            {
                SDL_SetWindowFullscreen(_window, true);
                SDL_SetWindowBordered(_window, false);
                break;
            }
        }
    }

    Tbx::WindowMode SDLWindow::GetMode()
    {
        return _currentMode;
    }
}