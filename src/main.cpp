import std;
import core;
import game;
import graphics;

namespace fs = std::filesystem;

int main() {
	std::string_view error;
	bool currently_saved = false;

	auto load_game = [&] {
		std::print("  Enter save location (leave blank to go back): ");

		std::string location;
		std::getline(std::cin, location);

		if (location.empty()) {
			game_state = GameState::MENU;
			return false;
		}

		fs::path path(location);
		if (not fs::exists(path)) {
			error = "No such file. Please try again.";
			return false;
		}

		if (not load(path)) {
			error = "Load failed, the file might be corrupted. Please try again.";
			return false;
		}

		return true;
	};

	auto save_game = [&] {
		std::print("  Enter save location (leave blank to go back): ");

		std::string location;
		std::getline(std::cin, location);

		if (location.empty()) return;

		fs::path path(location);
		fs::create_directories(path.parent_path());

		if (not save(path)) {
			error = "Save failed, the file might be corrupted. Please try again.";
			return;
		}

		currently_saved = true;
	};

	while (true) {
		clear_screen();
		std::println("\033[36m{}\033[0m\n", game_logo);

		switch (game_state) {
		case GameState::MENU: {
			std::println("  Welcome to \033[96m2k48!\033[0m\n\n"

				"        1. Play a New Game\n"
				"        2. Continue Previous Game\n"
				"        3. View Highscores and Statistics\n"
				"        4. Exit\n");

			if (not error.empty()) std::println("  \033[31m{}\033[0m\n", error);
			std::print("  Enter Choice: ");

			std::uint8_t choice = get_key_press_down() - '0';

			if (not choice or choice > 4) {
				error = "Invalid input. Please try again.";
				continue;
			}

			game_state = GameState(choice);
			break;
		}
		case GameState::NEW_GAME: {
			if (not error.empty()) std::println("  \033[31m{}\033[0m\n", error);
			std::print("  Enter gameboard size ('0' to go back): ");

			std::uint8_t choice = get_key_press_down() - '0';

			if (not choice) {
				game_state = GameState::MENU;
				continue;
			}
			if (choice < 3 or choice > 16) {
				error = "Invalid input. Gameboard size should range from 3 to 9";
				continue;
			}

			board_size = choice;
			game_state = GameState::IN_GAME;
			start();
			cache();

			break;
		}
		case GameState::CONTINUE: {
			if (not error.empty()) std::println("  \033[31m{}\033[0m\n", error);
			if (not load_game()) continue;

			game_state = GameState::IN_GAME;
			cache();

			break;
		}
		case GameState::VIEW: {
			if (not error.empty()) std::println("  \033[31m{}\033[0m\n", error);
			if (not load_game()) continue;

			game_state = GameState::IN_VIEW;
			cache();

			break;
		}
		case GameState::IN_GAME: {
			std::println("{}\n\n{}\n\n"

				"  W or K => Up\n"
				"  A or H => Left\n"
				"  S or J => Down\n"
				"  D or L => Right\n"
				"  Z or P => Save\n"
				"  R => Reset colors\n"
				"  M => Return to menu\n\n"

				"  Press the keys to start and continue.\n", render_score(), render());

			if (not error.empty()) std::println("  \033[31m{}\033[0m\n", error);
			char input = get_key_press_down();
			
			switch (input) {
			case 'K':
			case 'k':
			case 'W':
			case 'w': current_key = Key::UP; break;

			case 'H':
			case 'h':
			case 'A':
			case 'a': current_key = Key::LEFT; break;

			case 'J':
			case 'j':
			case 'S':
			case 's': current_key = Key::DOWN; break;

			case 'L':
			case 'l':
			case 'D':
			case 'd': current_key = Key::RIGHT; break;

			case 'M':
			case 'm': {
				if (currently_saved) {
					game_state = GameState::MENU;
					break;
				}

				std::print("  Do you want to save? (y/n/c): ");

				char inp = get_key_press_down();

				if (inp == 'C' or inp == 'c') break;
				if (inp == 'N' or inp == 'n') {
					game_state = GameState::MENU;
					continue;
				}

				std::println();
				save_game();

				game_state = GameState::MENU;
				continue;
			}

			case 'r':
			case 'R': colorize(); break;

			case 'Z':
			case 'z':
			case 'P':
			case 'p': save_game(); continue;

			default: error = "Invalid input. Please try again."; continue;
			}

			process();
			currently_saved = false;
			break;
		}
		case GameState::WIN: {
			std::println("{}\n\n"
				"  \033[96mYOU WIN!\033[0m\n\n"
				"{}", render(), render_statis());
			save_game();

			game_state = GameState::MENU;
			break;
		}
		case GameState::LOSE: {
			std::println("{}\n\n"
				"  \033[91mGAME OVER!\033[0m\n\n"
				"{}", render(), render_statis());
			save_game();

			game_state = GameState::MENU;
			break;
		}
		case GameState::IN_VIEW: {
			std::println("{}\n\n", render_statis());
			std::print("  Press any key to continue . . .");

			get_key_press_down();

			game_state = GameState::MENU;
			break;
		}
		default: return 0;
		}

		error = "";
	}
}