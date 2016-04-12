#include <photon/window.hpp>
#include <photon/app.hpp>
#include <photon/widget/panel.hpp>
#include <photon/widget/margin.hpp>

int main()
{
    using namespace photon;

    auto& my_app = make_app<app>();
    {
        auto w = margin(
            { 50, 50, 50, 50 }
          , panel()
        );

        window main_window("Photon", { 1000, 600 }, colors::gray[30], my_app, w);
        my_app.run();
    }

   return 0;
}

