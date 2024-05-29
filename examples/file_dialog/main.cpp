/*=============================================================================
   Copyright (c) 2016-2023 Joel de Guzman

   Distributed under the MIT License (https://opensource.org/licenses/MIT)
=============================================================================*/
#include <elements.hpp>
#include <list>
#include <nfd.hpp>

using namespace cycfi::elements;
using namespace cycfi::artist;

// Main window background color
auto constexpr color_bg = rgba(35, 35, 37, 255);
auto constexpr color_accent = rgba(55, 55, 57, 255);
auto constexpr color_red = colors::red.level(0.7).opacity(0.4);
auto constexpr color_green = colors::green.level(0.7).opacity(0.4);
auto background = box(color_bg);

std::string txt_file, cpp_file, folder;

enum class NFD_TARGET {FILE, FOLDER};
template<NFD_TARGET target>
auto make_dialog_button = [] (auto& text_field, auto& bg, std::pair<std::string, std::string> filter) {
	auto folder_button = button("");
	auto clear_button = button("");
	auto folder_icon = icon(icons::folder_open_empty);
	auto clear_icon = icon(icons::cancel);

	folder_button.on_click = [&text_field, filter] (bool) {
		NFD::Guard guard;
		NFD::UniquePath output;
		nfdresult_t result;

		if constexpr (target == NFD_TARGET::FILE) {
			nfdfilteritem_t filterItem[1] = {{filter.first.c_str(), filter.second.c_str()}};
			result = NFD::OpenDialog(output, filterItem, 1);
		} else {
			result = NFD::PickFolder(output);
		}

		if (result == NFD_OKAY) {
			std::cout << "User picked target: " << output.get() << std::endl;
			text_field.second->set_text(output.get());
			text_field.second->on_enter(output.get());
		} else if (result == NFD_CANCEL) {
			puts("User cancelled selection.");
		} else {
			printf("Error: %s\n", NFD_GetError());
		}
	};

	clear_button.on_click = [&text_field] (bool) {
		text_field.second->set_text("");
		text_field.second->on_enter("");
	};

	return htile(
		fixed_size(
			{ 30, 30 },
			layer(
				align_center_middle(
					folder_icon
				),
				folder_button
			)
		),
		hspace(5),
		layer(
			link(text_field.first),
			link(bg)
		),
		layer(
			fixed_size(
				{ 20, 15 },
				layer(
					align_center_middle(
						clear_icon
					),
					clear_button
				)
			),
			frame({})
		)
	);
};

auto make_file_dialog_button = [] (auto& text_field, auto& bg, std::pair<std::string, std::string> filter) {return make_dialog_button<NFD_TARGET::FILE>(text_field, bg, filter);};
auto make_folder_dialog_button = [] (auto& text_field, auto& bg) {return make_dialog_button<NFD_TARGET::FOLDER>(text_field, bg, {});};

/**
 * @brief Text autocompleter. 
 * 
 * First determines if the path is valid for autocompletion.
 * If so, it evaluates all files in the directory @a path with the @a cmp_func. 
 * If there are no matches, an empty string is returned along with 'false'. 
 * If there is only a single match, the full path to the file is returned along with 'true'. 
 * If there are multiple matches, the longest common prefix is returned along with 'false'. 
 */
auto autocomplete = [] (std::string_view path, unsigned int& last_size, std::function<bool(const std::string&)> cmp_func) {
	// prevent autocompletion when deleting text
	if (path.size() < last_size) {
		last_size = path.size();
		return std::make_pair(std::string(), false);
	}
	last_size = path.size();

	// only autocomplete if the last character is a '/' and there are less than 20 matches
	if (path.back() != '/') {return std::make_pair(std::string(), false);;}
	if (!std::filesystem::is_directory(path)) {return std::make_pair(std::string(), false);;}
	if (20 < std::distance(std::filesystem::directory_iterator(path), std::filesystem::directory_iterator{})) {return std::make_pair(std::string(), false);;}

	std::list<std::string> matches;
	for (auto& p : std::filesystem::directory_iterator(path)) {
		if (cmp_func(p.path())) {
			matches.push_back(p.path());
		}
	}

	// no matches, return empty string
	if (matches.empty()) {return std::make_pair(std::string(path), false);}

	// only one match, auto-fill
	if (matches.size() == 1) {
		return std::make_pair(matches.front(), true);
	}

	// multiple matches, find the longest common prefix
	std::string prefix = matches.front();
	for (auto& match : matches) {
		if (prefix == match) {continue;}
		std::string tmp;
		for (size_t i = 0; i < std::min(prefix.size(), match.size()); ++i) {
			if (prefix[i] == match[i]) {
				tmp += prefix[i];
			} else {
				break;
			}
		}
		prefix = tmp;
	}

	// no slashes
	if (prefix.size() > 1) {
		return std::make_pair(prefix, false);
	}
	return std::make_pair(std::string(path), false);
};

auto make_io_menu(view& _view) {
	static auto txt_box_bg = box(color_bg);
	static auto cpp_box_bg = box(color_bg);
	static auto folder_box_bg = box(color_bg);

	static auto txt_box = input_box("txt path");
	static auto cpp_box = input_box("cpp path");
	static auto folder_box = input_box("folder path");

	auto txt_validation_func = [] (std::string_view p) {return std::filesystem::path(p).extension() == ".txt";};
	auto cpp_validation_func = [] (std::string_view p) {return std::filesystem::path(p).extension() == ".cpp";};

	txt_box.second->on_text = [txt_validation_func, &_view] (std::string_view text) {
		if (text.size() == 1) {
			txt_box_bg = color_accent;
		} else if (text.empty()) {
			txt_box_bg = color_bg;
		}

		if (!txt_file.empty()) {
			txt_file.clear();
			txt_box_bg = color_accent;
		}
		_view.refresh(txt_box_bg);

		static unsigned int last_size = 0;
		auto fill = autocomplete(text, last_size, txt_validation_func);
		if (!fill.first.empty()) {txt_box.second->set_text(fill.first);}
		if (fill.second) {txt_box.second->on_enter(fill.first);}
	};

	txt_box.second->on_enter = [txt_validation_func, &_view] (std::string_view text) -> bool {
		if (text.empty()) {
			txt_box_bg = color_bg;
			txt_file.clear();
			_view.refresh(txt_box_bg);
			return false;
		}

		if (!txt_validation_func(text)) {
			std::cout << "invalid txt file \"" << text << "\"" << std::endl;
			txt_box_bg = color_red;
			txt_file.clear();
			_view.refresh(txt_box_bg);
			return false;
		}

		// check if we can use a relative path instead of absolute
		std::string file = std::string(text);
		if (auto curpath = std::filesystem::current_path().string(); file.find(curpath) != std::string::npos) {
			file = std::filesystem::relative(file, curpath).string();
		}
		txt_box.second->set_text(file);

		txt_file = file;
		txt_box_bg = color_green;
		_view.refresh(txt_box_bg);
		std::cout << "txt file was set to " << txt_file << std::endl;
		return true;
	};

	cpp_box.second->on_text = [cpp_validation_func, &_view] (std::string_view text) {
		if (text.size() == 1) {
			cpp_box_bg = color_accent;
		} else if (text.empty()) {
			cpp_box_bg = color_bg;
		}

		if (!cpp_file.empty()) {
			cpp_file.clear();
			cpp_box_bg = color_accent;
		}
		_view.refresh(cpp_box_bg);

		static unsigned int last_size = 0;
		auto fill = autocomplete(text, last_size, cpp_validation_func);
		if (!fill.first.empty()) {cpp_box.second->set_text(fill.first);}
		if (fill.second) {cpp_box.second->on_enter(fill.first);}
	};

	cpp_box.second->on_enter = [cpp_validation_func, &_view] (std::string_view text) -> bool {
		if (text.empty()) {
			cpp_box_bg = color_bg;
			cpp_file.clear();
			_view.refresh(txt_box_bg);
			return false;
		}

		if (!cpp_validation_func(text)) {
			std::cout << "invalid cpp file \"" << text << "\"" << std::endl;
			cpp_box_bg = color_red;
			txt_file.clear();
			_view.refresh(cpp_box_bg);
			return false;
		}

		// check if we can use a relative path instead of absolute
		std::string file = std::string(text);
		if (auto curpath = std::filesystem::current_path().string(); file.find(curpath) != std::string::npos) {
			file = std::filesystem::relative(file, curpath).string();
		}
		cpp_box.second->set_text(file);

		cpp_file = file;
		cpp_box_bg = color_green;
		_view.refresh(cpp_box_bg);
		std::cout << "cpp file was set to " << cpp_file << std::endl;
		return true;
	};

	folder_box.second->on_text = [&_view] (std::string_view text) {
		if (text.size() == 1) {
			folder_box_bg = color_accent;
		} else if (text.empty()) {
			folder_box_bg = color_bg;
		}
		_view.refresh(folder_box_bg);
	};

	folder_box.second->on_enter = [&_view] (std::string_view text) -> bool {
		folder = text;
		std::cout << "output path was set to \"" << folder << "\"" << std::endl;
		return true;
	};

	auto txt_box_field = make_file_dialog_button(txt_box, txt_box_bg, {"txt file", "txt"});
	auto cpp_box_field = make_file_dialog_button(cpp_box, cpp_box_bg, {"C/CPP source file", "c,cpp"});
	auto folder_box_field = make_folder_dialog_button(folder_box, folder_box_bg);

	return htile(
		margin(
			{50, 10, 50, 10},
			hsize(
				300,
				txt_box_field
			)
		),
		margin(
			{50, 10, 50, 10},
			hsize(
				300,
				cpp_box_field
			)
		),
		margin(
			{50, 10, 50, 10},
			hsize(
				300,
				folder_box_field
			)
		)
	);
}

auto make_start_button = [] (view& _view) {
	auto start_button = button("Start");
	start_button->set_body_color(color_green);
	start_button.on_click = [&_view] (bool) {
		if (txt_file.empty() || cpp_file.empty() || folder.empty()) {
			auto popup = message_box0("Missing input files.", icons::hand);
			open_popup(popup, _view);
			return;
		}

		auto popup = message_box0(
			"All files accepted. You chose:\n"
				"txt file: " + txt_file + "\n"
				"cpp file: " + cpp_file + "\n"
				"output folder: " + folder + "\n", 
			icons::hand
		);
		open_popup(popup, _view);
	};

	return hsize(
		200,
		start_button
	);
};

int main(int argc, char* argv[]) {
	app _app("Empty Starter");
	window _win(_app.name());
	_win.on_close = [&_app]() { _app.stop(); };

	view view_(_win);

	view_.content(
		vtile(
			margin(
				{10, 10, 10, 10},
				align_center_middle(
					label("Select the files to be processed").font_size(18)
				)
			),
			margin(
				{10, 10, 10, 10},
				align_center_middle(
					make_io_menu(view_)
				)
			),
			margin(
				{10, 10, 10, 10},
				align_center_middle(
					make_start_button(view_)
				)
			)
		),
		background
	);

	_app.run();
	return 0;
}
