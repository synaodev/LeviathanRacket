#ifndef LEVIATHAN_INCLUDED_OVERLAY_DRAW_SCHEME_HPP
#define LEVIATHAN_INCLUDED_OVERLAY_DRAW_SCHEME_HPP

#include <glm/vec2.hpp>

#include "../utility/enums.hpp"

struct renderer_t;
struct animation_t;

struct draw_scheme_t : public not_copyable_t {
public:
	draw_scheme_t() = default;
	draw_scheme_t(draw_scheme_t&&) = default;
	draw_scheme_t& operator=(draw_scheme_t&&) = default;
	~draw_scheme_t() = default;
public:
	void update(real64_t delta);
	void render(renderer_t& renderer) const;
	void invalidate() const;
	void set_file(const animation_t* file);
	void set_visible(bool_t visible);
	void set_state(arch_t state);
	void set_direction(direction_t direction);
	void set_frame(arch_t frame);
	void set_table(sint_t table);
	void set_position(real_t x, real_t y);
	void set_position(const glm::vec2& position);
	void mut_position(real_t x, real_t y);
	void mut_position(const glm::vec2& offset);
	bool is_finished() const;
	real64_t get_timer() const;
	arch_t get_state() const;
	direction_t get_direction() const;
	arch_t get_frame() const;
	sint_t get_table() const;
	const glm::vec2& get_position() const;
public:
	static constexpr arch_t NonState = (arch_t)-1;
private:
	mutable bool_t amend { true };
	const animation_t* file { nullptr };
	bool_t visible { true };
	real64_t timer { 0.0 };
	arch_t state { 0 };
	direction_t direction { direction_t::Right };
	arch_t frame { 0 };
	sint_t table { 0 };
	glm::vec2 position {};
};

#endif // LEVIATHAN_INCLUDED_OVERLAY_DRAW_SCHEME_HPP
