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

auto make_range_slider = [] (view& _view, auto& _range_slider, std::string _title) {
	static auto _min_textbox = input_box("min level");
	static auto _max_textbox = input_box("max level");
	static auto _min_bg = box(bg_color);
	static auto _max_bg = box(bg_color);

	auto pretty_printer = [] (float value) {
		std::stringstream ss;
		ss << std::setprecision(3) << value;
		return ss.str();
	};

	auto axis_transform = [] (float value) {
		return value*10;
	};

	auto axis_transform_inv = [] (float value) {
		return value/10;
	};

	// note: ok to take a reference to _range_slider here, since we declared it static
	_range_slider.on_change.first = [&_view, &_range_slider, pretty_printer, axis_transform] (float value) {
		_min_textbox.second->set_text(pretty_printer(axis_transform(value)));
        _view.refresh(_min_textbox.first);
	};
	_range_slider.edit_value_first(_range_slider.value_first());

	_range_slider.on_change.second = [&_view, &_range_slider, pretty_printer, axis_transform] (float value) {
		_max_textbox.second->set_text(pretty_printer(axis_transform(value)));
        _view.refresh(_max_textbox.first);
	};
	_range_slider.edit_value_second(_range_slider.value_second());

	_min_textbox.second->on_text = [] (std::string_view text) {
		if (text.empty()) {
			_min_bg = bg_color;
		} else {
			_min_bg = bg_color_accent;
		}
	};

	_min_textbox.second->on_enter = [&_view, &_range_slider, axis_transform_inv] (std::string_view text) {
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

	_max_textbox.second->on_enter = [&_view, &_range_slider, axis_transform_inv] (std::string_view text) {
		try {
			_range_slider.value_second(axis_transform_inv(std::stof(std::string(text))));
			_max_bg = bg_color;
			_view.refresh(_range_slider);
		} catch (std::exception&) {
			_max_bg = bred;
		}
	};

	return margin(
		{10, 10, 10, 10},
		layer(
			vtile(
				vspace(10),
				align_center(
					label(_title).font_size(18)
				),
				vspace(10),
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
			),
			frame{}
		)
	);
};

auto make_log_range_slider(view& _view) {
	double min_val = 1e-4;
	double max_val = 1e0;
	static auto track = basic_track<5, false>(colors::black);
	static auto _range_slider = range_slider(
		basic_thumb<10>(),
		basic_thumb<10>(),
		slider_labels<10>(
			slider_marks_log<20, 4>(track), 0.8, "1e-4", "1e-3", "1e-2", "1e-1", "1e0"
		),
		{0.1, 0.8}
	);

	static auto _min_textbox = input_box("min level");
	static auto _max_textbox = input_box("max level");
	static auto _min_bg = box(bg_color);
	static auto _max_bg = box(bg_color);

	auto pretty_printer = [] (float value) {
		std::stringstream ss;
		ss << std::setprecision(2) << std::scientific << value;
		return ss.str();
	};

	auto axis_transform_inv = [min_val, max_val] (float value) {
		return (std::log10(value)-std::log10(min_val))/(std::log10(max_val)-std::log10(min_val))*(1-0)+0;
	};

	auto axis_transform = [min_val, max_val] (float x) {
		double logy = (x-0)/(1-0)*(std::log10(max_val)-std::log10(min_val)) + std::log10(min_val);
		return std::pow(10, logy);
	};

	_range_slider.on_change.first = [&_view, pretty_printer, axis_transform] (float value) {
		_min_textbox.second->set_text(pretty_printer(axis_transform(value)));
        _view.refresh(_min_textbox.first);
	};
	_range_slider.edit_value_first(_range_slider.value_first());

	_range_slider.on_change.second = [&_view, pretty_printer, axis_transform] (float value) {
		_max_textbox.second->set_text(pretty_printer(axis_transform(value)));
        _view.refresh(_max_textbox.first);
	};
	_range_slider.edit_value_second(_range_slider.value_second());

	_min_textbox.second->on_text = [] (std::string_view text) {
		if (text.empty()) {
			_min_bg = bg_color;
		} else {
			_min_bg = bg_color_accent;
		}
	};

	_min_textbox.second->on_enter = [&_view, axis_transform_inv] (std::string_view text) {
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

	_max_textbox.second->on_enter = [&_view, axis_transform_inv] (std::string_view text) {
		try {
			_range_slider.value_second(axis_transform_inv(std::stof(std::string(text))));
			_max_bg = bg_color;
			_view.refresh(_range_slider);
		} catch (std::exception&) {
			_max_bg = bred;
		}
	};

	return margin(
		{10, 10, 10, 10},
		layer(
			vtile(
				vspace(10),
				align_center(
					label("Logarithmic range slider").font_size(18)
				),
				vspace(10),
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
			),
			frame{}
		)
	);
}

auto make_default_range_slider(view& _view) {
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
	return make_range_slider(_view, _range_slider, "Default linear range slider");
}

auto make_overlapping_range_slider(view& _view) {
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
		{0.1, 0.8},
		0.5 // overlap parameter - +0.5 means total overlap, -0.5 means no overlap
	);
	return make_range_slider(_view, _range_slider, "Overlapping linear range slider");
}

int main(int argc, char* argv[])
{
   app _app(argc, argv, "RangeSlider", "com.cycfi.range_slider");
   window _win(_app.name());
   _win.on_close = [&_app]() { _app.stop(); };

   view _view(_win);

   _view.content(
	  vtile(
		make_default_range_slider(_view),
		make_overlapping_range_slider(_view),
		make_log_range_slider(_view)
	  ),
      background
   );

   _app.run();
   return 0;

}