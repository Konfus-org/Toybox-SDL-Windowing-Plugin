#pragma 
#include "SDLWindow.h"
#include <Tbx/Plugins/Plugin.h>
#include <Tbx/Windowing/Window.h>
#include <Tbx/Events/AppEvents.h>

namespace SDLWindowing
{
    class SDLWindowFactoryPlugin 
        : public Tbx::Plugin
        , public Tbx::IWindowFactory
    {
    public:
        SDLWindowFactoryPlugin(std::weak_ptr<Tbx::App> app)
            : _app(app) {}

        void OnLoad() override;
        void OnUnload() override;
        std::shared_ptr<Tbx::Window> Create(
            const std::string& title,
            const Tbx::Size& size,
            const Tbx::WindowMode& mode,
            Tbx::Ref<Tbx::EventBus> eventBus) override;

    protected:
        void OnAppSettingsChanged(const Tbx::AppSettingsChangedEvent& e);

        SDLWindow* New(Tbx::Ref<Tbx::EventBus> eventBus);
        void Delete(Tbx::Window* window);

    private:
        std::weak_ptr<Tbx::App> _app = {};
        bool _usingOpenGl = false;
    };

    TBX_REGISTER_PLUGIN(SDLWindowFactoryPlugin);
}
