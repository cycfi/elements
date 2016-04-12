#include <photon/window.hpp>
#include <photon/app.hpp>
#include <photon/widget/panel.hpp>
#include <photon/widget/margin.hpp>


int main()
{
    namespace ph = photon;

    auto& app = photon::make_app<ph::app>();
    {
        ph::widget_ptr panel { new ph::panel{} };
        ph::widget_ptr margin { new ph::margin{ { 50, 50, 50, 50 }, panel } };
    
        ph::window main_window("Photon", { 1000, 600 }, ph::colors::gray[30], app, margin);
        app.run();
    }

	return 0;
}

