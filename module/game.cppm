export module game;

import std;

namespace fs = std::filesystem;

export {
	void colorize();
	void start();
	void process();

	void undo();

	enum class SaveState : std::uint8_t {
		FAILED,
		NORMAL,
		WON,
		LOST
	};

	bool save(fs::path const& path);
	SaveState load(fs::path const& path);
}