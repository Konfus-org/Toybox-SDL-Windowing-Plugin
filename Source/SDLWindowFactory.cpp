#include "SDLWindowFactory.h"
#include "SDLWindow.h"
#include <Tbx/App/App.h>

namespace SDLWindowing
{
    void SDLWindowFactory::OnLoad()
    {
        TBX_ASSERT(SDL_Init(SDL_INIT_VIDEO)  != 0, "Failed to initialize SDL");
    }

    void SDLWindowFactory::OnUnload()
    {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    std::shared_ptr<Tbx::IWindow> SDLWindowFactory::Create(const std::string& title, const Tbx::Size& size, const Tbx::WindowMode mode)
    {
        auto window = std::shared_ptr<Tbx::IWindow>(New(), [this](Tbx::IWindow* win) { Delete(win); });
        window->SetTitle(title);
        window->SetSize(size);
        window->Open(mode);
        window->Focus();
        return window;
    }

    void SDLWindowFactory::Delete(Tbx::IWindow* window)
    {
        delete window;
    }

    Tbx::IWindow* SDLWindowFactory::New()
    {
        auto* newWindow = new SDLWindow();
        return newWindow;
    }
}
