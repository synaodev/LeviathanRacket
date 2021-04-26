#include "./location.hpp"
#include "./kontext.hpp"

#include "../system/renderer.hpp"

glm::vec2 location_t::center() const {
	return position + bounding.center();
}

rect_t location_t::hitbox() const {
	return rect_t {
		position + bounding.left_top(),
		bounding.dimensions()
	};
}

bool location_t::overlap(const location_t& that) const {
	return this->hitbox().overlaps(that.hitbox());
}

bool location_t::overlap(const rect_t& that) const {
	return this->hitbox().overlaps(that);
}

void location_t::hori(direction_t bits) {
	if (bits & direction_t::Left) {
		direction |= direction_t::Left;
	} else {
		direction &= ~direction_t::Left;
	}
}

void location_t::vert(direction_t bits) {
	if (bits & direction_t::Up) {
		direction |= direction_t::Up;
		direction &= ~direction_t::Down;
	} else if (bits & direction_t::Down) {
		direction |= direction_t::Down;
		direction &= ~direction_t::Up;
	} else {
		direction &= ~(direction_t::Up | direction_t::Down);
	}
}

void location_t::render(const kontext_t& kontext, renderer_t& renderer, const rect_t& viewport) {
	const glm::vec4 color { 1.0f, 1.0f, 1.0f, 0.5f };
	auto& list = renderer.display_list(
		layer_value::Foreground,
		blend_mode_t::Add,
		program_t::Colors
	);
	kontext.slice<location_t>().each([&list, &viewport, &color](entt::entity, const location_t& location) {
		const rect_t hitbox = location.hitbox();
		if (hitbox.overlaps(viewport)) {
			list.begin(display_list_t::SingleQuad)
				.vtx_blank_write(hitbox, color)
				.vtx_transform_write(hitbox.left_top())
			.end();
		}
	});
}
