#include <photon/window.hpp>
#include <photon/app.hpp>


int main()
{
    auto& app = photon::make_app<photon::app>();
    {
        photon::window main_window("Photon", 1000, 600, photon::colors::gray[30]);
        app.run();
    }

	return 0;
}

