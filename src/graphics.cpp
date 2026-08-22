module graphics;

import core;

namespace views = std::ranges::views;
using namespace std::string_view_literals;

static constexpr auto top_left_corner = "╭"sv;
static constexpr auto bot_left_corner = "╰"sv;
static constexpr auto top_right_corner = "╮"sv;
static constexpr auto bot_right_corner = "╯"sv;

static constexpr auto horizontal_edge = "──────"sv;
static constexpr auto vertical_edge = "│"sv;

static constexpr auto right_t = "├"sv;
static constexpr auto left_t = "┤"sv;
static constexpr auto bot_t = "┬"sv;
static constexpr auto top_t = "┴"sv;

static constexpr auto intersection = "┼"sv;

static std::string_view spaces(std::uint16_t const& i) {
	auto idx = &i == currently_spawned ? i * 2048 : i;
	return colors[(std::size_t)std::log2(idx)];
}

static std::int8_t board_size_1;

static std::string r_top;
static std::string r_mid;
static std::string r_bot;

std::string render() {
	std::string result = r_top;

	auto make_bar = [&](std::int8_t i) {
		std::string repeated;

		for (auto j : views::iota(std::int8_t(0), board_size_1)) {
			auto& block = game_matrix[i][j];
			repeated += std::format("{}{:^6}\033[0m{}", spaces(block), (block ? std::to_string(block) : " "), vertical_edge);
		}

		auto& block = game_matrix[i][board_size_1];
		result += std::format("{}{}{}{:^6}\033[0m{}\n", vertical_edge, repeated, spaces(block), (block ? std::to_string(block) : " "), vertical_edge);
	};

	for (auto i : views::iota(std::int8_t(0), board_size_1)) {
		make_bar(i);
		result += r_mid;
	}

	make_bar(board_size_1);
	currently_spawned = nullptr;

	return result + r_bot;
}

static std::string rs_top;
static std::string rs_bot;

std::string render_score() {
	std::string result = rs_top;

	result += std::format("{} {:<23}{:>5} {}\n", vertical_edge, "BEST SCORE:", best_score, vertical_edge);
	result += std::format("{} {:<23}{:>5} {}\n", vertical_edge, "HIGHEST TILE:", highest_tile, vertical_edge);
	result += std::format("{} {:<23}{:>5} {}\n", vertical_edge, "MOVES:", total_moves, vertical_edge);

	return result + rs_bot;
}

static std::string rst_top;
static std::string rst_bot;

std::string render_statis() {
	std::string result = rst_top;

	result += std::format("{} {:<29}{:>5} {}\n", vertical_edge, "Final score:", best_score, vertical_edge);
	result += std::format("{} {:<29}{:>5} {}\n", vertical_edge, "Highest tile:", highest_tile, vertical_edge);
	result += std::format("{} {:<29}{:>5} {}\n", vertical_edge, "Total moves:", total_moves, vertical_edge);
	result += std::format("{} {:<29}{:>5} {}\n", vertical_edge, "Time taken:", time_taken, vertical_edge);
	
	return result + rst_bot;
}

void clear_screen() { std::print("\033[3J\033[H\033[J"); }

void cache() {
	board_size_1 = board_size - 1;

	{
		std::string const repeated = views::repeat(std::format("{}{}", horizontal_edge, bot_t))
			| views::take(board_size_1)
			| views::join
			| std::ranges::to<std::string>();

		r_top = std::format("{}{}{}{}\n", top_left_corner, repeated, horizontal_edge, top_right_corner);
	}

	{
		std::string const repeated = views::repeat(std::format("{}{}", horizontal_edge, intersection))
			| views::take(board_size_1)
			| views::join
			| std::ranges::to<std::string>();

		r_mid =  std::format("{}{}{}{}\n", right_t, repeated, horizontal_edge, left_t);
	}

	{
		std::string const repeated = views::repeat(std::format("{}{}", horizontal_edge, top_t))
			| views::take(board_size_1)
			| views::join
			| std::ranges::to<std::string>();

		r_bot = std::format("{}{}{}{}", bot_left_corner, repeated, horizontal_edge, bot_right_corner);
	}

	{
		std::string const repeated = views::repeat(horizontal_edge)
			| views::take(4)
			| views::join
			| std::ranges::to<std::string>();

		rs_top = std::format("{}{}{}{}\n", top_left_corner, repeated, horizontal_edge, top_right_corner);
		rs_bot = std::format("{}{}{}{}\n", bot_left_corner, repeated, horizontal_edge, bot_right_corner);
	}

	{
		std::string const repeated = views::repeat(horizontal_edge)
			| views::take(4)
			| views::join
			| std::ranges::to<std::string>();

		rst_top = std::format("{} \033[1;38;5;214mSTATISTICS\033[0m {}{}\n", top_left_corner, repeated, top_right_corner);
		rst_bot = std::format("{}{}{}{}{}\n", bot_left_corner, repeated, horizontal_edge, horizontal_edge, bot_right_corner);
	}
}