#pragma once
#include <Tbx/Plugins/Plugin.h>
#include <Tbx/Windowing/Window.h>

namespace SDLWindowing
{
    class SDLWindowFactoryPlugin 
        : public Tbx::Plugin
        , public Tbx::IWindowFactory
    {
    public:
        SDLWindowFactoryPlugin(std::weak_ptr<Tbx::App> app) {}

        void OnLoad() override;
        void OnUnload() override;
        std::shared_ptr<Tbx::Window> Create(const std::string& title, const Tbx::Size& size, const Tbx::WindowMode mode) override;

    protected:
        Tbx::Window* New();
        void Delete(Tbx::Window* window);
    };

    TBX_REGISTER_PLUGIN(SDLWindowFactoryPlugin);
}
