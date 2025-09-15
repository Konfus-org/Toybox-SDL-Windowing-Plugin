#pragma once
#include <Tbx/Plugins/Plugin.h>
#include <Tbx/Windowing/IWindow.h>

namespace SDLWindowing
{
    class SDLWindowFactory : public Tbx::IWindowFactory, public Tbx::Plugin
    {
    public:
        SDLWindowFactory(const std::weak_ptr<Tbx::App>& app) {}

        void OnLoad() override;
        void OnUnload() override;

        std::shared_ptr<Tbx::IWindow> Create(const std::string& title, const Tbx::Size& size, const Tbx::WindowMode mode) override;

    protected:
        Tbx::IWindow* New();
        void Delete(Tbx::IWindow* window);
    };

    TBX_REGISTER_PLUGIN(SDLWindowFactory);
}
