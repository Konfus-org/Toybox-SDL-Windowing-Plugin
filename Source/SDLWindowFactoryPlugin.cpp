#include "SDLWindowFactoryPlugin.h"
#include <Tbx/App/App.h>
#include <Tbx/Debug/Debugging.h>

namespace SDLWindowing
{
    void SDLWindowFactoryPlugin::OnLoad()
    {
        TBX_ASSERT(SDL_Init(SDL_INIT_VIDEO)  != 0, "Failed to initialize SDL");

        auto tbxApp = _app.lock();
        _usingOpenGl = tbxApp->GetSettings().Api == Tbx::GraphicsApi::OpenGL;
        tbxApp->GetEventBus().Subscribe(this, &SDLWindowFactoryPlugin::OnAppSettingsChanged);
    }

    void SDLWindowFactoryPlugin::OnUnload()
    {
        _app.lock()->GetEventBus().Unsubscribe(this, &SDLWindowFactoryPlugin::OnAppSettingsChanged);
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        SDL_Quit(); // If we are unloading our windowing quit all of SDL
    }

    std::shared_ptr<Tbx::Window> SDLWindowFactoryPlugin::Create(const std::string& title, const Tbx::Size& size, const Tbx::WindowMode& mode, Tbx::Ref<Tbx::EventBus> eventBus)
    {
        auto* sdlWindow = New(eventBus);
        auto window = std::shared_ptr<Tbx::Window>((Tbx::Window*)sdlWindow, [this](Tbx::Window* win) { Delete(win); });
        // HACK: used to get around enable shared from this issues
        sdlWindow->SetThis(window);
        window->SetTitle(title);
        window->SetSize(size);
        window->SetMode(mode);
        return window;
    }

    void SDLWindowFactoryPlugin::OnAppSettingsChanged(const Tbx::AppSettingsChangedEvent& e)
    {
        _usingOpenGl = e.GetNewSettings().Api == Tbx::GraphicsApi::OpenGL;
    }

    void SDLWindowFactoryPlugin::Delete(Tbx::Window* window)
    {
        delete window;
    }

    SDLWindow* SDLWindowFactoryPlugin::New(Tbx::Ref<Tbx::EventBus> eventBus)
    {
        auto* newWindow = new SDLWindow(_usingOpenGl, eventBus);
        return newWindow;
    }
}
