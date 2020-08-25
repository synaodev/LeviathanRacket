#include "./button.hpp"

#include "../utility/logger.hpp"
#include "../utility/vfs.hpp"

static constexpr arch_t kNotReady = (arch_t)-1;

demo_player_t::demo_player_t() :
	record(false),
	index(kNotReady),
	buttons()
{
	
}

demo_player_t::demo_player_t(bool_t record) :
	record(record),
	index(kNotReady),
	buttons()
{

}

bool demo_player_t::load(const std::string& name) {
	const std::vector<uint64_t> buffer = vfs::uint64_buffer(vfs::resource_path(vfs_resource_path_t::Init) + name + ".dmo");
	if (buffer.empty()) {
		synao_log("Error! Failed to load demo file!\n");
		return false;
	}
	buttons.resize(buffer.size());
	for (arch_t it = 0; it < buttons.size(); ++it) {
		buttons[it] = std::bitset<btn_t::Total>(buffer[it]);
	}
	index = 0;
	return true;
}

bool demo_player_t::write(const std::string& name) {
	if (!record) {
		return true;
	}
	const std::string path = vfs::resource_path(vfs_resource_path_t::Init) + name + ".dmo";
	std::vector<uint64_t> buffer;
	buffer.resize(buttons.size());
	for (arch_t it = 0; it < buttons.size(); ++it) {
		buffer[it] = buttons[it].to_ullong();
	}
	if (!vfs::create_recording(path, buffer)) {
		return false;
	}
	buttons.clear();
	record = false;
	return true;
}

void demo_player_t::read(std::bitset<btn_t::Total>& pressed, std::bitset<btn_t::Total>& holding) {
	if (this->playing()) {
		pressed = buttons[index++];
		holding = buttons[index++];
	}
}

void demo_player_t::store(const std::bitset<btn_t::Total>& pressed, const std::bitset<btn_t::Total>& holding) {
	if (this->recording()) {
		buttons.push_back(pressed);
		buttons.push_back(holding);
	}
}

bool demo_player_t::recording() const {
	return record;
}

bool demo_player_t::playing() const {
	if (record) {
		return false;
	}
	if (index == kNotReady) {
		return false;
	}
	if ((index + 1) >= buttons.size()) {
		return false;
	}
	return true;
}
