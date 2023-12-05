/*=============================================================================
   Copyright (c) 2023 Kristian Lytje

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>

using namespace cycfi::elements;

// Main window background color
auto constexpr bg_color_accent = rgba(55, 55, 57, 255);
auto constexpr bg_color = rgba(35, 35, 37, 255);
auto constexpr bred     = colors::red.level(0.7).opacity(0.4);
auto background = box(bg_color);

inline auto pretty_printer = [] (float value) {
    std::stringstream ss;
    ss << std::setprecision(3) << value;
    return ss.str();
};

inline auto axis_transform = [] (float value) {
    return value*10;
};

inline auto axis_transform_inv = [] (float value) {
    return value/10;
};

auto make_range_slider(view& _view) {
	static auto track = basic_track<5, false>(colors::black);
	static auto thumb = margin(
		{1, 2, 1, 2},
		box(colors::white_smoke)
	);
	static auto _range_slider = range_slider(
		fixed_size(
			{5, 30},
			box(colors::light_gray)
		),
        fixed_size(
			{5, 30},
			box(colors::light_gray)
		),
		slider_labels<11>(
			slider_marks<20, 10*5, 10>(track), 0.8, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"
		),
		{0.1, 0.8}
	);

	static auto _min_textbox = input_box("min level");
	static auto _max_textbox = input_box("max level");
	static auto _min_bg = box(bg_color);
	static auto _max_bg = box(bg_color);

	_range_slider.on_change.first = [&_view] (float value) {
		_min_textbox.second->set_text(pretty_printer(axis_transform(value)));
        _view.refresh(_min_textbox.first);
	};

	_range_slider.on_change.second = [&_view] (float value) {
		_max_textbox.second->set_text(pretty_printer(axis_transform(value)));
        _view.refresh(_max_textbox.first);
	};

	_min_textbox.second->on_text = [] (std::string_view text) {
		if (text.empty()) {
			_min_bg = bg_color;
		} else {
			_min_bg = bg_color_accent;
		}
	};

	_min_textbox.second->on_enter = [&_view] (std::string_view text) {
		try {
			_range_slider.value_first(axis_transform_inv(std::stof(std::string(text))));
			_min_bg = bg_color;
			_view.refresh(_range_slider);
		} catch (std::exception&) {
			_min_bg = bred;
		}
	};

    _max_textbox.second->on_text = [] (std::string_view text) {
		if (text.empty()) {
			_max_bg = bg_color;
		} else {
			_max_bg = bg_color_accent;
		}
	};

	_max_textbox.second->on_enter = [&_view] (std::string_view text) {
		try {
			_range_slider.value_second(axis_transform_inv(std::stof(std::string(text))));
			_max_bg = bg_color;
			_view.refresh(_range_slider);
		} catch (std::exception&) {
			_max_bg = bred;
		}
	};

	return vtile(
		margin(
			{50, 10, 50, 10},
			link(_range_slider)
		),
		layer(
			align_left(
				margin(
					{50, 10, 50, 10},
					hsize(
						100,
						layer(
							link(_min_textbox.first),
							link(_min_bg)
						)
					)
				)
			),
			align_right(
				margin(
					{50, 10, 50, 10},
					hsize(
						100,
						layer(
							link(_max_textbox.first),
							link(_max_bg)
						)
					)
				)
			)
		)
	);
}

int main(int argc, char* argv[])
{
   app _app(argc, argv, "RangeSlider", "com.cycfi.range_slider");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view _view(_win);

   _view.content(
      make_range_slider(_view),
      background
   );

   _app.run();
   return 0;

}