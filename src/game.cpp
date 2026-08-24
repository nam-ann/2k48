module game;

import core;

namespace fs = std::filesystem;
namespace views = std::ranges::views;

static std::vector<GameSnapshot> game_history;

static std::chrono::steady_clock::time_point start_time;
static std::int8_t board_size_1;

static std::random_device rd;
static std::mt19937 gen(rd());

template <typename Get>
static void forward_line(Get&& get, bool& changed) {
    std::int8_t write = 0;
    
    for (auto read : views::iota(std::int8_t(0), board_size)) {
        auto value = get(read);
        if (not value) continue;

        if (not changed and read != write) [[unlikely]] changed = true;
        get(write++) = value;
    }

    while (write < board_size) get(write++) = 0;

    for (auto i : views::iota(std::int8_t(0), board_size_1)) {
        auto& value = get(i);
        if (not value) continue;

        if (value == get(i + 1)) {
            value *= 2;
            get(i + 1) = 0;

            current_game.best_score += value;
            if (current_game.highest_tile < value) current_game.highest_tile = value;

            if (value >= 2048) {
                time_taken = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start_time);
                game_state = GameState::WIN;
            }
            if (not changed) [[unlikely]] changed = true;
        }
    }

    write = 0;

    for (auto read : views::iota(std::int8_t(0), board_size)) {
        auto value = get(read);
        if (value) get(write++) = value;
    }

    while (write < board_size) get(write++) = 0;
}

template <typename Get>
static void backward_line(Get&& get, bool& changed) {
    std::int8_t write = board_size_1;

    for (auto read : views::iota(std::int8_t(0), board_size) | views::reverse) {
        auto value = get(read);
        if (not value) continue;
        
        if (not changed and read != write) [[unlikely]] changed = true;
        get(write--) = value;
    }

    while (write >= 0) get(write--) = 0;

    for (auto i : views::iota(std::int8_t(1), board_size) | views::reverse) {
        auto& value = get(i);
        if (not value) continue;

        if (value == get(i - 1)) {
            value *= 2;
            get(i - 1) = 0;

            current_game.best_score += value;
            if (current_game.highest_tile < value) current_game.highest_tile = value;

            if (value >= 2048) {
                time_taken = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start_time);
                game_state = GameState::WIN;
            }
            if (not changed) [[unlikely]] changed = true;
        }
    }

    write = board_size_1;

    for (auto read : views::iota(std::int8_t(0), board_size) | views::reverse) {
        auto value = get(read);
        if (value) get(write--) = value;
    }

    while (write >= 0) get(write--) = 0;
}

using randint = std::uniform_int_distribution<int>;
using randbool = std::bernoulli_distribution;

static alignas(randint) std::byte idist_x[sizeof(randint)];
static alignas(randint) std::byte idist_y[sizeof(randint)];
static alignas(randbool) std::byte idist_spawn[sizeof(randbool)];

static void create_dist() {
    std::construct_at((randint*)idist_x, 0, board_size_1);
    std::construct_at((randint*)idist_y, 0, board_size_1);
    std::construct_at((randbool*)idist_spawn, 0.9);
}

static auto is_not_grayscale(int n) {
    int code = n - 16;
    int r = code / 36;
    int g = (code % 36) / 6;
    int b = code % 6;
    return not (r == g and g == b);
}

void colorize() {
    auto nums = views::iota(16, 232)
        | views::filter(is_not_grayscale)
        | std::ranges::to<std::vector>();

    std::ranges::shuffle(nums, std::mt19937{ std::random_device{}() });
    for (auto [i, n] : views::zip(views::iota(0, 11), nums)) {
        colors[i] = std::format("\x1b[38;5;{}m", n);
    }

    colors[11] = colors[12] = "\033[97m";
}

void start() {
    board_size_1 = board_size - 1;
    create_dist();

    auto& dist_x = (randint&)idist_x;
    auto& dist_y = (randint&)idist_y;
    auto& dist_spawn = (randbool&)idist_spawn;

    std::memset(current_game.game_matrix, 0, sizeof current_game.game_matrix);
    current_game.game_matrix[dist_x(gen)][dist_y(gen)] = 2;
    game_history.clear();

    colorize();
    start_time = std::chrono::steady_clock::now();
}

void process() {
	auto& dist_x = (randint&)idist_x;
	auto& dist_y = (randint&)idist_y;
    auto& dist_spawn = (randbool&)idist_spawn;

    bool changed = false;
    auto snapshot = current_game;

	switch (current_key) {
    case Key::UP: {
        for (auto col : views::iota(std::int8_t(0), board_size)) {
            forward_line([col](std::int8_t i) -> auto& { return current_game.game_matrix[i][col]; }, changed);
        }

        break;
    }
	case Key::LEFT: {
        for (auto row : views::iota(std::int8_t(0), board_size)) {
            forward_line([row](std::int8_t i) -> auto& { return current_game.game_matrix[row][i]; }, changed);
        }

        break;
	}
    case Key::DOWN: {
        for (auto col : views::iota(std::int8_t(0), board_size)) {
            backward_line([col](std::int8_t i) -> auto& { return current_game.game_matrix[i][col]; }, changed);
        }

        break;
    }
    case Key::RIGHT: {
        for (auto row : views::iota(std::int8_t(0), board_size)) {
            backward_line([row](std::int8_t i) -> auto& { return current_game.game_matrix[row][i]; }, changed);
        }

        break;
    }
    case Key::NONE: return;
	}

    if (changed) {
        ++total_moves;
        auto random = [&dist_x, &dist_y]() { currently_spawned = &current_game.game_matrix[dist_x(gen)][dist_y(gen)]; };

        for (random(); *currently_spawned; random());
        *currently_spawned = dist_spawn(gen) ? 2 : 4;

        game_history.emplace_back(snapshot);
    }
    else {
        auto original_best_score = current_game.best_score;

        for (auto idx : views::iota(std::int8_t(0), board_size)) {
            std::uint16_t col = 0;
            forward_line([&col, idx](std::int8_t i) -> auto& { return col = current_game.game_matrix[i][idx]; }, changed);
            backward_line([&col, idx](std::int8_t i) -> auto& { return col = current_game.game_matrix[i][idx]; }, changed);
        }
        for (auto idx : views::iota(std::int8_t(0), board_size)) {
            std::uint16_t row = 0;
            forward_line([&row, idx](std::int8_t i) -> auto& { return row = current_game.game_matrix[idx][i]; }, changed);
            backward_line([&row, idx](std::int8_t i) -> auto& { return row = current_game.game_matrix[idx][i]; }, changed);
        }

        current_game.best_score = original_best_score;

        if (game_state == GameState::WIN) game_state = GameState::IN_GAME;
        if (not changed) {
            time_taken = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start_time);
            game_state = GameState::LOSE;
        }
    }

    current_key = Key::NONE;
}

void undo() {
    if (game_history.empty()) return;

    current_game = game_history.back();

    game_history.pop_back();
    --total_moves;
}

bool save(fs::path const& path) {
    std::ofstream file(path, std::ios::binary);

    if (not file) return false;

    file.write(game_state == GameState::LOSE ? "\1" : game_state == GameState::WIN ? "\2" : "\0", 1);

    file.write((const char*)&current_game.best_score, sizeof current_game.best_score);
    file.write((const char*)&current_game.highest_tile, sizeof current_game.highest_tile);
    file.write((const char*)&total_moves, sizeof total_moves);
    file.write((const char*)&time_taken, sizeof time_taken);

    file.write((const char*)&board_size, sizeof board_size);
    file.write((const char*)current_game.game_matrix, sizeof current_game.game_matrix);

    return true;
}

SaveState load(fs::path const& path) {
    std::ifstream file(path, std::ios::binary);

    if (not file) return SaveState::FAILED;

    char state; file.read((char*)&state, 1);

    file.read((char*)&current_game.best_score, sizeof current_game.best_score);
    file.read((char*)&current_game.highest_tile, sizeof current_game.highest_tile);
    file.read((char*)&total_moves, sizeof total_moves);
    file.read((char*)&time_taken, sizeof time_taken);

    file.read((char*)&board_size, sizeof board_size);
    file.read((char*)current_game.game_matrix, sizeof current_game.game_matrix);

    if (state == '\1') return SaveState::LOST;
    else if (state == '\2') return SaveState::WON;

    return SaveState::NORMAL;
}