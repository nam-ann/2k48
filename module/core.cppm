export module core;

import std;

using namespace std::string_view_literals;

export {
	enum class GameState : std::uint8_t {
		MENU,
		NEW_GAME,
		CONTINUE,
		VIEW,
		EXIT,

		IN_GAME,
		WIN,
		LOSE,

		IN_VIEW,
	};

	enum class Key : std::uint8_t {
		NONE,
		UP,
		LEFT,
		DOWN,
		RIGHT,
	};
	
	char get_key_press_down();

	std::int8_t board_size = 0;
	GameState game_state = GameState::MENU;
	Key current_key = Key::UP;
	std::vector<std::vector<std::uint16_t>> game_matrix;
	std::string colors[13];

	std::size_t total_moves = 0;
	std::uint16_t best_score = 0;
	std::uint16_t highest_tile = 0;
	std::uint16_t* currently_spawned = nullptr;

	std::chrono::seconds time_taken;

	constexpr auto top_left_corner = "╭"sv;
	constexpr auto bot_left_corner = "╰"sv;
	constexpr auto top_right_corner = "╮"sv;
	constexpr auto bot_right_corner = "╯"sv;

	constexpr auto horizontal_edge = "──────"sv;
	constexpr auto vertical_edge = "│"sv;

	constexpr auto right_t = "├"sv;
	constexpr auto left_t = "┤"sv;
	constexpr auto bot_t = "┬"sv;
	constexpr auto top_t = "┴"sv;

	constexpr auto intersection = "┼"sv;

	constexpr auto game_logo = R"(
  ████████  █████      █████ █████   ████████  
 ███▒▒▒▒███▒▒███      ▒▒███ ▒▒███   ███▒▒▒▒███ 
▒▒▒    ▒███ ▒███ █████ ▒███  ▒███ █▒███   ▒███ 
   ███████  ▒███▒▒███  ▒███████████▒▒████████  
  ███▒▒▒▒   ▒██████▒   ▒▒▒▒▒▒▒███▒█ ███▒▒▒▒███ 
 ███      █ ▒███▒▒███        ▒███▒ ▒███   ▒███ 
▒██████████ ████ █████       █████ ▒▒████████  
▒▒▒▒▒▒▒▒▒▒ ▒▒▒▒ ▒▒▒▒▒       ▒▒▒▒▒   ▒▒▒▒▒▒▒▒   
)"sv;
}