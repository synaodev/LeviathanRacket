#ifndef SYNAO_OVERLAY_DRAW_UNITS_HPP
#define SYNAO_OVERLAY_DRAW_UNITS_HPP

#include "../utl/enums.hpp"
#include "../utl/rect.hpp"
#include "../gfx/vertex_pool.hpp"

struct renderer_t;
struct texture_t;

struct draw_units_t : public not_copyable_t, public not_moveable_t {
public:
	draw_units_t();
	~draw_units_t() = default;
public:
	void force() const;
	void render(renderer_t& renderer) const;
	void set_position(real_t x, real_t y);
	void set_position(glm::vec2 position);
	void set_bounding(real_t x, real_t y, real_t w, real_t h);
	void set_bounding(rect_t bounding);
	void set_values(sint_t current, sint_t maximum);
	void set_texture(const texture_t* texture);
	glm::vec2 get_position() const;
private:
	void generate(arch_t current, arch_t maximum, bool_t resize);
private:
	mutable bool_t write;
	glm::vec2 position;
	rect_t bounding;
	sint_t current_value, maximum_value;
	const texture_t* texture;
	vertex_pool_t quads;
};

#endif // SYNAO_OVERLAY_DRAW_UNITS_HPP