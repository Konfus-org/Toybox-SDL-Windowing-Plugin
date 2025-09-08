#include "SDLWindow.h"
#include <Tbx/App/App.h>
#include <Tbx/Events/WindowEvents.h>
#include <Tbx/Events/EventCoordinator.h>
#include <Tbx/Debug/Debugging.h>

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

    // TODO: Introduce the concept of a render context to TBX and make this a render context provider plugin!
    Tbx::ProcAddress SDLWindow::GetProcAddress() const
    {
        return static_cast<Tbx::ProcAddress>(SDL_GL_GetProcAddress);
    }

    void SDLWindow::SwapBuffers()
    {
        SDL_GL_SwapWindow(_window);
    }

    int SDLWindow::GetSwapInterval() const
    {
        int interval;
        SDL_GL_GetSwapInterval(&interval);
        return interval;
    }

    void SDLWindow::SetSwapInterval(int interval)
    {
        SDL_GL_SetSwapInterval(interval);
    }

    void SDLWindow::Open(const Tbx::WindowMode& mode)
    {
        _currentMode = mode;

        Uint32 flags = SDL_WINDOW_RESIZABLE;

        auto useOpenGl = Tbx::App::GetInstance()->GetGraphicsSettings().Api == Tbx::GraphicsApi::OpenGL;
        if (useOpenGl)
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
            case Tbx::WindowMode::Windowed: break;
            case Tbx::WindowMode::Fullscreen: flags |= SDL_WINDOW_FULLSCREEN; break;
            case Tbx::WindowMode::Borderless: flags |= SDL_WINDOW_BORDERLESS; break;
            case Tbx::WindowMode::FullscreenBorderless: 
            {
                flags |= SDL_WINDOW_FULLSCREEN;
                flags |= SDL_WINDOW_BORDERLESS;
                break;
            }
        }

        _window = SDL_CreateWindow(_title.c_str(), _size.Width, _size.Height, flags);
        TBX_ASSERT(_window, "SDL_CreateWindow failed: %s", SDL_GetError());

        if (useOpenGl)
        {
            _glContext = SDL_GL_CreateContext(_window);
            TBX_ASSERT(_glContext, "Failed to create gl context for window!");
            SDL_GL_MakeCurrent(_window, _glContext);
        }

        auto e = Tbx::WindowOpenedEvent(GetId());
        Tbx::EventCoordinator::Send(e);
    }

    void SDLWindow::Close()
    {
        // We've already been closed...
        if (_window == nullptr) return;

        // Cleanup...
        SDL_DestroyWindow(_window);
        _window = nullptr;

        // Notify things we've closed
        auto e = Tbx::WindowClosedEvent(GetId());
        Tbx::EventCoordinator::Send(e);

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
                int w, h;
                SDL_GetWindowSize(_window, &w, &h);
                SetSize({ w, h });
                break;
            }
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
            {
                Focus();
                break;
            }
            default:
            {
                break;
            }
        }
    }

    void SDLWindow::Focus()
    {
        SDL_RaiseWindow(_window);
        if (Tbx::App::GetInstance()->GetGraphicsSettings().Api == Tbx::GraphicsApi::OpenGL)
        {
            SDL_GL_MakeCurrent(_window, _glContext);
        }

        auto e = Tbx::WindowFocusedEvent(GetId());
        Tbx::EventCoordinator::Send(e);
    }

    const std::string& SDLWindow::GetTitle() const
    {
        return _title;
    }

    void SDLWindow::SetTitle(const std::string& title)
    {
        _title = title;
        SDL_SetWindowTitle(_window, title.c_str());
    }

    const Tbx::Size& SDLWindow::GetSize() const
    {
        return _size;
    }

    void SDLWindow::SetSize(const Tbx::Size& size)
    {
        _size = size;
        SDL_SetWindowSize(_window, _size.Width, _size.Height);

        auto e = Tbx::WindowResizedEvent(GetId(), _size);
        Tbx::EventCoordinator::Send(e);
    }

    void SDLWindow::SetMode(const Tbx::WindowMode& mode)
    {
        _currentMode = mode;
        switch (_currentMode)
        {
            case Tbx::WindowMode::Windowed:
            {
                SDL_SetWindowFullscreen(_window, false);
                SDL_SetWindowBordered(_window, true);
                break;
            }
            case Tbx::WindowMode::Fullscreen:
            {
                SDL_SetWindowFullscreen(_window, true);
                break;
            }
            case Tbx::WindowMode::Borderless:
            {
                SDL_SetWindowFullscreen(_window, false);
                SDL_SetWindowBordered(_window, false);
                break;
            }
            case Tbx::WindowMode::FullscreenBorderless:
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