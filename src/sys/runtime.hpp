#ifndef SYNAO_SYSTEM_RUNTIME_HPP
#define SYNAO_SYSTEM_RUNTIME_HPP

#include "./kernel.hpp"
#include "./camera.hpp"

#include "../utl/enums.hpp"
#include "../cnt/kontext.hpp"
#include "../act/naomi.hpp"
#include "../eve/receiver.hpp"
#include "../fld/tilemap.hpp"
#include "../mnu/stack_gui.hpp"
#include "../mnu/dialogue_gui.hpp"
#include "../mnu/inventory_gui.hpp"
#include "../oly/draw_title_view.hpp"
#include "../oly/draw_headsup.hpp"

struct setup_file_t;
struct input_t;
struct video_t;
struct audio_t;
struct music_t;
struct renderer_t;

struct runtime_t : public not_copyable_t {
public:
	runtime_t();
	runtime_t(runtime_t&&) = default;
	runtime_t& operator=(runtime_t&&) = default;
	~runtime_t() = default;
public:
	bool init(const setup_file_t& config, input_t& input, audio_t& audio, music_t& music, renderer_t& renderer);
	void handle(setup_file_t& config, policy_t& policy, input_t& input, video_t& video, audio_t& audio, music_t& music);
	void update(real64_t delta, const setup_file_t& config, policy_t& policy, input_t& input, audio_t& audio, renderer_t& renderer);
	void render(const video_t& video, renderer_t& renderer) const;
	bool viable() const;
private:
	bool setup_field(audio_t& audio, renderer_t& renderer);
	void setup_boot(const setup_file_t& config, renderer_t& renderer);
	void setup_load();
	void setup_save();
#ifdef SYNAO_DEBUG_BUILD
	void setup_debug(input_t& input, const renderer_t& renderer);
#endif // SYNAO_DEBUG_BUILD
private:
	real64_t accum;
	kernel_t kernel;
	receiver_t receiver;
	stack_gui_t stack_gui;
	dialogue_gui_t dialogue_gui;
	inventory_gui_t inventory_gui;
	draw_title_view_t title_view;
	draw_headsup_t headsup;
	camera_t camera;
	naomi_state_t naomi_state;
	kontext_t kontext;
	tilemap_t tilemap;
};

#endif // SYNAO_SYSTEM_RUNTIME_HPP