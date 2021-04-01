#ifndef LEVIATHAN_INCLUDED_OVERLAY_DRAW_COUNT_HPP
#define LEVIATHAN_INCLUDED_OVERLAY_DRAW_COUNT_HPP

#include "../utility/enums.hpp"
#include "../utility/rect.hpp"
#include "../video/vertex-pool.hpp"

struct renderer_t;
struct texture_t;
struct palette_t;

struct draw_count_t : public not_copyable_t {
public:
	draw_count_t();
	draw_count_t(draw_count_t&&) noexcept = default;
	draw_count_t& operator=(draw_count_t&&) noexcept = default;
	~draw_count_t() = default;
public:
	void invalidate() const;
	void render(renderer_t& renderer) const;
	void set_layer(layer_t layer);
	void set_backwards(bool_t backwards);
	void set_visible(bool_t visible);
	void set_table(sint_t table);
	void set_position(real_t x, real_t y);
	void set_position(const glm::vec2& position);
	void mut_position(real_t x, real_t y);
	void mut_position(const glm::vec2& offset);
	void set_bounding(real_t x, real_t y, real_t w, real_t h);
	void set_bounding(const rect_t& bounding);
	void new_value(sint_t number);
	void set_value(sint_t number);
	void add_value(sint_t number);
	void set_minimum_zeroes(arch_t minimum_zeroes);
	void set_texture(const texture_t* texture);
	void set_palette(const palette_t* palette);
	const glm::vec2& get_position() const;
	bool is_backwards() const;
	bool is_visible() const;
private:
	static sint_t quick_power_of_10(sint_t exponent);
	void generate_all(const std::vector<sint_t>& buffer);
	void generate_one(vtx_major_t* quad, glm::vec2 pos, glm::vec2 uvs, glm::vec2 inv, sint_t texID, sint_t palID);
private:
	mutable bool_t amend { false };
	layer_t layer { layer_value::Persistent };
	bool_t backwards { false };
	bool_t visible { false };
	sint_t table { 0 };
	glm::vec2 position {};
	rect_t bounding {};
	sint_t value { -425711 };
	arch_t minimum_zeroes { 0 };
	const texture_t* texture { nullptr };
	const palette_t* palette { nullptr };
	vertex_pool_t quads {};
};

#endif // LEVIATHAN_INCLUDED_OVERLAY_DRAW_COUNT_HPP
