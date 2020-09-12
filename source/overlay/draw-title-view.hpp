#ifndef LEVIATHAN_INCLUDED_OVERLAY_DRAW_TITLE_VIEW_HPP
#define LEVIATHAN_INCLUDED_OVERLAY_DRAW_TITLE_VIEW_HPP

#include "./draw-text.hpp"

struct draw_title_view_t : public not_copyable_t {
public:
	draw_title_view_t();
	draw_title_view_t(draw_title_view_t&&) = default;
	draw_title_view_t& operator=(draw_title_view_t&&) = default;
	~draw_title_view_t() = default;
public:
	void handle();
	void render(renderer_t& renderer) const;
	void invalidate() const;
	void push(const std::string& string, arch_t font_index);
	void clear();
	void set_font(const font_t* font);
	void set_persistent(glm::vec2 position, real_t table);
	void set_persistent(real_t x, real_t y, real_t table);
	void set_position(arch_t index, glm::vec2 position);
	void set_position(arch_t index, real_t x, real_t y);
	void set_centered(arch_t index, bool horizontal, bool vertical);
	void set_head(const std::string& string);
	void set_head();
	bool has_field() const;
	bool has_drawable() const;
private:
	real64_t timer;
	draw_text_t head, lite;
	std::vector<draw_text_t> cards;
};

#endif // LEVIATHAN_INCLUDED_OVERLAY_DRAW_TITLE_VIEW_HPP