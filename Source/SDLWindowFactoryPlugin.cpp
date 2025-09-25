#include "SDLWindowFactoryPlugin.h"
#include "SDLWindow.h"
#include "Tbx/Debug/Debugging.h"

namespace SDLWindowing
{
    void SDLWindowFactoryPlugin::OnLoad()
    {
        TBX_ASSERT(SDL_Init(SDL_INIT_VIDEO)  != 0, "Failed to initialize SDL");
    }

    void SDLWindowFactoryPlugin::OnUnload()
    {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        SDL_Quit(); // If we are unloading our windowing quit all of SDL
    }

    std::shared_ptr<Tbx::Window> SDLWindowFactoryPlugin::Create(const std::string& title, const Tbx::Size& size, const Tbx::WindowMode mode)
    {
        auto window = std::shared_ptr<Tbx::Window>(New(), [this](Tbx::Window* win) { Delete(win); });
        window->SetTitle(title);
        window->SetSize(size);
        window->SetMode(mode);
        return window;
    }

    void SDLWindowFactoryPlugin::Delete(Tbx::Window* window)
    {
        delete window;
    }

    Tbx::Window* SDLWindowFactoryPlugin::New()
    {
        auto* newWindow = new SDLWindow();
        return newWindow;
    }
}
