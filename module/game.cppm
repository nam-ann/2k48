export module game;

import std;

namespace fs = std::filesystem;

export {
	void colorize();
	void start();
	void process();
	bool save(fs::path const& path);
	bool load(fs::path const& path);
}