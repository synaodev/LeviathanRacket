#pragma once

#include "./widget.hpp"

#include "../overlay/draw-text.hpp"
#include "../overlay/draw-scheme.hpp"

struct wgt_audio_t : public widget_i {
public:
	wgt_audio_t() = default;
	wgt_audio_t(arch_t flags) :
		widget_i(flags) {}
	wgt_audio_t(wgt_audio_t&&) = default;
	wgt_audio_t& operator=(wgt_audio_t&&) = default;
	~wgt_audio_t() = default;
public:
	void init(const input_t& input, const video_t& video, audio_t& audio, const music_t& music, kernel_t& kernel) override;
	void handle(config_t& config, input_t& input, video_t& video, audio_t& audio, music_t& music, kernel_t& kernel, stack_gui_t& stack_gui, headsup_gui_t& headsup_gui) override;
	void update(real64_t delta) override;
	void render(renderer_t& renderer) const override;
	void invalidate() const override;
private:
	void setup_text(const audio_t& audio, const music_t& music);
private:
	arch_t cursor { 0 };
	draw_text_t text {};
	draw_scheme_t arrow {};
};
