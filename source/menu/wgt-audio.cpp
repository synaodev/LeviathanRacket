#include "./wgt-audio.hpp"

#include "../resource/id.hpp"
#include "../resource/vfs.hpp"
#include "../system/input.hpp"
#include "../system/audio.hpp"
#include "../system/music.hpp"
#include "../utility/setup-file.hpp"

#include <fmt/core.h>
#include <glm/common.hpp>

namespace {
	constexpr arch_t kAudioTotalOptions  	= 2;
	const glm::vec2 kAudioDefaultPosition 	{ 4.0f, 2.0f };
}

void wgt_audio_t::init(const input_t&, const video_t&, audio_t& audio, const music_t& music, kernel_t&) {
	ready = true;
	text.set_font(vfs_t::font(0));
	text.set_position(kAudioDefaultPosition);
	this->setup_text(audio, music);
	arrow.set_file(vfs_t::animation(res::anim::Heads));
	arrow.set_state(1);
	arrow.set_position(
		text.get_font_size().x,
		4.0f + kAudioDefaultPosition.y +
		(text.get_font_size().y * 2.0f)
	);
}

void wgt_audio_t::handle(setup_file_t& config, input_t& input, video_t&, audio_t& audio, music_t& music, kernel_t&, stack_gui_t&, headsup_gui_t&) {
	if (input.pressed[btn_t::Up]) {
		if (cursor > 0) {
			--cursor;
			audio.play(res::sfx::Select, 0);
			arrow.mut_position(0.0f, -text.get_font_size().y);
		}
	} else if (input.pressed[btn_t::Down]) {
		if (cursor < kAudioTotalOptions) {
			++cursor;
			audio.play(res::sfx::Select, 0);
			arrow.mut_position(0.0f, text.get_font_size().y);
		}
	} else if (input.holding[btn_t::Right] or input.holding[btn_t::Left]) {
		switch (cursor) {
			case 0: {
				real_t volume = audio.get_volume();
				if (input.holding[btn_t::Right]) {
					volume = glm::clamp(volume + 0.01f, 0.0f, 1.0f);
				} else {
					volume = glm::clamp(volume - 0.01f, 0.0f, 1.0f);
				}
				audio.set_volume(volume);
				config.set("Audio", "Volume", volume);
				this->setup_text(audio, music);
				break;
			}
			case 1: {
				real_t volume = music.get_volume();
				if (input.holding[btn_t::Right]) {
					volume = glm::clamp(volume + 0.01f, 0.0f, 1.0f);
				} else {
					volume = glm::clamp(volume - 0.01f, 0.0f, 1.0f);
				}
				music.set_volume(volume);
				config.set("Music", "Volume", volume);
				this->setup_text(audio, music);
				break;
			}
			default: {
				break;
			}
		}
	} else if (input.pressed[btn_t::No] or input.pressed[btn_t::Options]) {
		active = false;
	}
	if (!active) {
		input.flush();
		audio.play(res::sfx::Inven, 0);
	}
}

void wgt_audio_t::update(real64_t delta) {
	if (ready and active) {
		arrow.update(delta);
	}
}

void wgt_audio_t::render(renderer_t& renderer) const {
	if (ready and active) {
		text.render(renderer);
		arrow.render(renderer);
	}
}

void wgt_audio_t::invalidate() const {
	if (ready and active) {
		text.invalidate();
		arrow.invalidate();
	}
}

void wgt_audio_t::setup_text(const audio_t& audio, const music_t& music) {
	fmt::memory_buffer data;
	fmt::format_to(data, "{}{}\n{}{}\n",
		vfs_t::i18n_find("Audio", 0, 1),
		audio.get_volume(),
		vfs_t::i18n_find("Audio", 2),
		music.get_volume()
	);
	text.set_string(fmt::to_string(data));
}
