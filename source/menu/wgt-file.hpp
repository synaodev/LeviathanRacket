#pragma once

#include "./widget.hpp"

#include "../overlay/draw-text.hpp"
#include "../overlay/draw-scheme.hpp"

struct wgt_file_t : public widget_i {
public:
	wgt_file_t() = default;
	wgt_file_t(arch_t flags) :
		widget_i(flags) {}
	wgt_file_t(wgt_file_t&&) = default;
	wgt_file_t& operator=(wgt_file_t&&) = default;
	~wgt_file_t() = default;
public:
	void init(const input_t& input, const video_t& video, audio_t& audio, const music_t& music, kernel_t& kernel) override;
	void handle(setup_file_t& config, input_t& input, video_t& video, audio_t& audio, music_t& music, kernel_t& kernel, stack_gui_t& stack_gui, headsup_gui_t& headsup_gui) override;
	void update(real64_t delta) override;
	void render(renderer_t& renderer) const override;
	void invalidate() const override;
private:
	enum class file_op_t : arch_t {
		Unknown,
		Loading,
		Saving
	};
	file_op_t file_op { file_op_t::Unknown };
	arch_t cursor { 0 };
	draw_text_t text {};
	draw_scheme_t arrow {};
};
