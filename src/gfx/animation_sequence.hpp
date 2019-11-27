#ifndef SYNAO_GRAPHICS_ANIMATION_SEQUENCE_HPP
#define SYNAO_GRAPHICS_ANIMATION_SEQUENCE_HPP

#include <vector>
#include <string>

#include <array>
#include <vector>
#include <string>

#include "../utl/enums.hpp"
#include "../utl/rect.hpp"

struct sequence_frame_t
{
public:
	glm::vec2 position, origin;
public:
	sequence_frame_t(glm::vec2 position, glm::vec2 origin) : 
		position(position), 
		origin(origin) {}
	sequence_frame_t() = default;
	sequence_frame_t(const sequence_frame_t&) = default;
	sequence_frame_t(sequence_frame_t&& that) = default;
	sequence_frame_t& operator=(const sequence_frame_t&) = default;
	sequence_frame_t& operator=(sequence_frame_t&&) = default;
	~sequence_frame_t() = default;
};

struct animation_sequence_t : public not_copyable_t
{
public:
	animation_sequence_t();
	animation_sequence_t(glm::vec2 dimensions, real64_t delay, arch_t total, bool_t repeat);
	animation_sequence_t(animation_sequence_t&& that) noexcept;
	animation_sequence_t& operator=(animation_sequence_t&& that) noexcept;
	~animation_sequence_t() = default;
public:
	void append(glm::vec2 axnpnt);
	void append(glm::vec2 invert, glm::vec2 start, glm::vec4 points);
	void update(real64_t delta, bool_t& write, real64_t& timer, arch_t& frame) const;
	sequence_frame_t get_frame(arch_t frame, direction_t direction) const;
	rect_t get_quad(glm::vec2 invert, arch_t frame, direction_t direction) const;
	glm::vec2 get_dimensions() const;
	glm::vec2 get_origin(arch_t frame, direction_t direction) const;
	glm::vec2 get_action_point(direction_t direction) const;
	bool is_finished(arch_t frame, real64_t timer) const;
private:
	std::vector<sequence_frame_t> frames;
	std::vector<glm::vec2> action_points;
	glm::vec2 dimensions;
	real64_t delay;
	arch_t total;
	bool_t repeat;
};

#endif // SYNAO_GRAPHICS_ANIMATION_SEQUENCE_HPP