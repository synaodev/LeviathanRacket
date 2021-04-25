#ifndef LEVIATHAN_INCLUDED_AUDIO_CHANNEL_HPP
#define LEVIATHAN_INCLUDED_AUDIO_CHANNEL_HPP

#include "../types.hpp"

struct noise_t;

struct channel_t : public not_copyable_t {
public:
	channel_t() = default;
	channel_t(channel_t&& that) noexcept = default;
	channel_t& operator=(channel_t&& that) noexcept = default;
	~channel_t() {
		this->destroy();
	}
public:
	void create();
	void create(real_t volume);
	void destroy();
	bool attach();
	bool attach(const noise_t* noise);
	void set_volume(real_t volume);
	void play();
	void stop();
	void pause();
	bool playing() const;
	bool stopped() const;
	bool paused() const;
	real_t get_volume() const;
private:
	bool_t ready { false };
	uint_t handle { 0 };
	const noise_t* current { nullptr };
};

#endif // LEVIATHAN_INCLUDED_AUDIO_CHANNEL_HPP
