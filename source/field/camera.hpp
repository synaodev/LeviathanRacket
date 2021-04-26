#ifndef LEVIATHAN_INCLUDED_SYSTEM_CAMERA_HPP
#define LEVIATHAN_INCLUDED_SYSTEM_CAMERA_HPP

#include <glm/mat4x4.hpp>

#include "../utility/rect.hpp"

struct kontext_t;
struct naomi_state_t;

struct camera_t : public not_copyable_t {
public:
	camera_t() = default;
	camera_t(camera_t&&) noexcept = default;
	camera_t& operator=(camera_t&&) noexcept = default;
	~camera_t() = default;
public:
	void reset();
	void handle(const kontext_t& kontext, const naomi_state_t& naomi_state);
	void update(real64_t delta);
	void set_view_limits(const rect_t& view_limits);
	void set_focus(const glm::vec2& position);
	void follow(sint_t identity);
	void quake(real_t power, real64_t seconds);
	void quake(real_t power);
	rect_t get_viewport() const;
	arch_t get_tile_range(const glm::ivec2& first, const glm::ivec2& last) const;
	glm::mat4 get_matrix() const;
private:
	sint_t identity { 0 };
	bool_t cycling { false };
	bool_t indefinite { false };
	real64_t timer { 0.0 };
	rect_t view_limits {};
	glm::vec2 position {};
	glm::vec2 dimensions {};
	glm::vec2 offsets {};
	real_t quake_power { 0.0f };
	real_t view_angle { 0.0f };
};

#endif // LEVIATHAN_INCLUDED_SYSTEM_CAMERA_HPP
