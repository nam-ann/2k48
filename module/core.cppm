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

	struct GameSnapshot {
		std::uint16_t game_matrix[9][9];

		std::uint64_t best_score = 0;
		std::uint16_t highest_tile = 0;
	};
	
	void enable_utf8();
	char get_key_press_down();

	std::int8_t board_size = 0;
	GameState game_state = GameState::MENU;
	Key current_key = Key::UP;
	std::uint16_t game_matrix[9][9];
	std::string colors[13];

	std::uint64_t total_moves = 0;
	std::uint64_t best_score = 0;
	std::uint16_t highest_tile = 0;
	std::uint16_t* currently_spawned = nullptr;
	
	std::chrono::seconds time_taken;

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