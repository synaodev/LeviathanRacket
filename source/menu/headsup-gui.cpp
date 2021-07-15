#include "./headsup-gui.hpp"

#include "../resource/id.hpp"
#include "../resource/vfs.hpp"
#include "../system/kernel.hpp"
#include "../system/receiver.hpp"
#include "../utility/logger.hpp"

#include <angelscript/scriptarray.h>

namespace {
	constexpr arch_t kHeadFontIndex 		= 2;
	const glm::vec2 kMainSchemePosition 	{ 2.0f, 2.0f };
	const glm::vec2 kLeviathanCountPosition { 10.0f, 5.0f };
	const rect_t kLeviathanCountBounding 	{ 56.0f, 0.0f, 8.0f, 9.0f };
	const glm::vec2 kBarrierUnitsPosition 	{ 47.0f, 2.0f };
	const rect_t kBarrierUnitsBounding 		{ 45.0f, 0.0f, 6.0f, 8.0f };
	const glm::vec2 kOxygenCountPosition 	{ 2.0f, 18.0f };
	const rect_t kOxygenCountBounding 		{ 56.0f, 9.0f, 8.0f, 9.0f };
	const glm::vec2 kTitleViewPosition 		{ 240.0f, 36.0f };
	const glm::vec4 kTitleViewColor			{ 0.25f, 0.25f, 1.0f, 1.0f };
}

bool headsup_gui_t::init(receiver_t& receiver) {
	suspender = [&receiver] {
		receiver.suspend();
	};
	const animation_t* heads_animation = vfs_t::animation(res::anim::Heads);
	const animation_t* items_animation = vfs_t::animation(res::anim::Items);
	const texture_t* texture = vfs_t::texture(res::img::Heads);
	const palette_t* palette = vfs_t::palette(res::pal::Heads);
	if (!heads_animation or !items_animation or !texture or !palette) {
		synao_log("HeadsUp GUI is missing resources and so child overlays cannot be renderered!\n");
		return false;
	}
	if (!this->refresh()) {
		return false;
	}

	main_scheme.set_file(heads_animation);
	main_scheme.set_table(0);
	main_scheme.set_position(kMainSchemePosition);

	leviathan_count.set_texture(texture);
	leviathan_count.set_palette(palette);
	leviathan_count.set_bounding(kLeviathanCountBounding);
	leviathan_count.set_position(kLeviathanCountPosition);
	leviathan_count.set_minimum_zeroes(3);
	leviathan_count.set_visible(true);
	leviathan_count.set_backwards(false);

	barrier_units.set_texture(texture);
	barrier_units.set_palette(palette);
	barrier_units.set_bounding(kBarrierUnitsBounding);
	barrier_units.set_position(kBarrierUnitsPosition);

	oxygen_count.set_texture(texture);
	oxygen_count.set_palette(palette);
	oxygen_count.set_bounding(kOxygenCountBounding);
	oxygen_count.set_position(kOxygenCountPosition);
	oxygen_count.set_visible(false);
	oxygen_count.set_backwards(false);

	item_view.init(texture, palette, heads_animation, items_animation);
	fight_meter.init(heads_animation);
	fade.init();
	synao_log("HeadsUp GUI is ready.\n");
	return true;
}

bool headsup_gui_t::refresh() {
	const font_t* font = vfs_t::font(kHeadFontIndex);
	if (!font) {
		synao_log("HeadsUp GUI is missing a font and so titles cannot be rendered!\n");
		return false;
	}
	title_view.set_font(font);
	title_view.set_persistent(kTitleViewPosition, kTitleViewColor);
	return true;
}

void headsup_gui_t::reset() {
	main_scheme.set_state(0);
	main_scheme.set_direction(direction_t::Right);
	fight_meter.reset();
	fade.reset();
}

void headsup_gui_t::handle(const kernel_t& kernel, const dialogue_gui_t& dialogue_gui) {
	title_view.handle(dialogue_gui);
	item_view.handle(kernel);
	fade.handle();
}

void headsup_gui_t::update(real64_t delta) {
	main_scheme.update(delta);
	fight_meter.update(delta);
}

void headsup_gui_t::render(renderer_t& renderer, const kernel_t& kernel) const {
	title_view.render(renderer);
	if (!kernel.has(kernel_t::Lock)) {
		main_scheme.render(renderer);
		leviathan_count.render(renderer);
		barrier_units.render(renderer);
		oxygen_count.render(renderer);
		item_view.render(renderer);
		fight_meter.render(renderer);
	} else {
		main_scheme.invalidate();
		leviathan_count.invalidate();
		barrier_units.invalidate();
		oxygen_count.invalidate();
		item_view.invalidate();
		fight_meter.invalidate();
	}
	if (fade.is_visible()) {
		fade.render(renderer);
	}
}

void headsup_gui_t::invalidate() const {
	title_view.invalidate();
	fade.invalidate();
}

void headsup_gui_t::set_parameters(headsup_params_t params) {
	main_scheme.set_table(params.main_state);
	main_scheme.set_direction(params.main_direction);
	leviathan_count.set_value(params.current_leviathan);
	barrier_units.set_values(params.current_barrier, params.maximum_barrier);
	oxygen_count.set_visible(params.current_oxygen != params.maximum_oxygen);
	oxygen_count.set_value(params.current_oxygen);
}

void headsup_gui_t::set_fight_values(sint_t current, sint_t maximum) {
	fight_meter.set_values(current, maximum);
}

void headsup_gui_t::set_field_text(const std::string& text) {
	title_view.set_head(text);
}

void headsup_gui_t::set_field_text() {
	title_view.set_head();
}

void headsup_gui_t::push_card(const std::string& text, arch_t font_index) {
	title_view.push(text, font_index);
}

void headsup_gui_t::clear_cards() {
	title_view.clear();
}

void headsup_gui_t::set_card_position(arch_t index, real_t x, real_t y) {
	title_view.set_position(index, x, y);
}

void headsup_gui_t::set_card_centered(arch_t index, bool horizontal, bool vertical) {
	title_view.set_centered(index, horizontal, vertical);
}

void headsup_gui_t::fade_in() {
	fade.fade_in();
	std::invoke(suspender);
}

void headsup_gui_t::fade_out() {
	fade.fade_out();
	std::invoke(suspender);
}

bool headsup_gui_t::is_fade_done() const {
	return fade.is_done();
}

bool headsup_gui_t::is_fade_moving() const {
	return fade.is_moving();
}

sint_t headsup_gui_t::get_main_state() const {
	return main_scheme.get_table();
}
