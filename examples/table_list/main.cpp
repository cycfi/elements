/*=============================================================================
   Copyright (c) 2022 Johann Philippe

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

class table_cell_composer
{
public:

    using composer_fn = std::function<element_ptr(size_t line, size_t index)>;
    table_cell_composer(size_t lines, size_t columns, composer_fn &&fn)
        : _num_lines(lines)
        , _num_columns(columns)
        , _composer(fn)
    {}

    element_ptr compose(size_t line, size_t col)
    {
        return _composer(line, col);
    }

    size_t lines()      { return _num_lines; }
    size_t columns()    { return _num_columns; }

private:

    size_t _num_lines, _num_columns;
    composer_fn _composer;
};

class table_list : public vlist
{
public:

    using table_composer_ptr = std::shared_ptr<table_cell_composer>;

    element_ptr line_maker(size_t line)
    {
        if (h_list[line].get() == nullptr)
        {
            auto h_composer =
                basic_horizontal_cell_composer(_table_composer->columns(),
                [&, line](size_t col) {return _table_composer->compose(line, col);}
            );
            h_list[line] = share(hlist(h_composer));
        }
        return h_list[line];
    };

    table_list(table_composer_ptr ptr)
        : vdynamic_list(basic_vertical_cell_composer(ptr->lines(), [&](size_t line) {return line_maker(line);}))
        , _table_composer(ptr)
        , h_list(ptr->lines(), nullptr)
    {
    }

protected:

    table_composer_ptr _table_composer;
    std::vector<element_ptr> h_list;
};

int main(int argc, char* argv[])
{
   app _app(argc, argv, "Table List", "com.cycfi.table-list");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view view_(_win);

   size_t lines = 500;
   size_t columns = 100;

   std::vector<std::vector<element_ptr>> ptr_list(lines);
   for(size_t i = 0; i < lines; i++)
   {
       ptr_list[i].resize(columns, nullptr);
   }

   auto && make_cell = [&](size_t l, size_t c)
   {
       color cell_color =
            ((l % 2 == 0) ? colors::deep_pink : colors::royal_blue)
            .opacity(c % 2 == 0 ? 0.1 : 0.05)
            ;

       return share(
            limit({{100, 50}, {200, 100}},
                layer(
                    align_center_middle(
                        label(std::string(std::to_string(l) + "  " + std::to_string(c)))
                    ),
                    rbox(cell_color, 6)
                )
            )
        );
   };

   table_cell_composer comp(lines, columns, make_cell);
   table_list l(share(comp));

   view_.content(
        margin({10, 10, 10, 10},
            scroller(
                hold(share(l))
            )
        ),
        background
    );

   _app.run();
   return 0;
}
