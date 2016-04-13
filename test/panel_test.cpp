#include <photon/window.hpp>
#include <photon/app.hpp>
#include <photon/widget/panel.hpp>
#include <photon/widget/margin.hpp>
#include <photon/widget/tile.hpp>

int main()
{
    using namespace photon;

    auto& my_app = make_app<app>();
    
    {
        auto m = top_margin(
            { 20 }
          , panel()
        );
        
//        auto e = m;
        
        auto e = margin({ 20, 0, 20, 20 }, vtile(m, m, m, m));

        window main_window("Photon", { 1000, 600 }, colors::gray[30], my_app, e);
        my_app.run();
    }

   return 0;
}

