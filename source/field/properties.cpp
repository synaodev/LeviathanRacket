#include "./properties.hpp"

#include <glm/gtc/constants.hpp>

namespace ftcv {
	void prop_to_stats(const std::vector<tmx::Property>& properties, direction_t& direction, std::string& event, arch_t& flags, sint_t& idnum, arch_t& deter) {
		if (properties.size() >= 5) {
			direction = prop_to_direction(properties[0]);
			event = prop_to_string(properties[1]);
			flags = static_cast<arch_t>(prop_to_sint(properties[2]));
			idnum = prop_to_sint(properties[3]);
			deter = static_cast<arch_t>(prop_to_sint(properties[4]));
		}
	}

	bool_t prop_to_bool(const tmx::Property& property) {
		if (property.getType() == tmx::Property::Type::Boolean) {
			return static_cast<bool_t>(property.getBoolValue());
		}
		return false;
	}

	sint_t prop_to_sint(const tmx::Property& property) {
		if (property.getType() == tmx::Property::Type::Int) {
			return property.getIntValue();
		} else if (property.getType() == tmx::Property::Type::String) {
			return std::stoi(property.getStringValue(), nullptr, 0);
		}
		return 0;
	}

	real_t prop_to_real(const tmx::Property& property) {
		if (property.getType() == tmx::Property::Type::Float) {
			return property.getFloatValue();
		} else if (property.getType() == tmx::Property::Type::String) {
			return std::stof(property.getStringValue());
		}
		return 0.0f;
	}

	direction_t prop_to_direction(const tmx::Property& property) {
		if (property.getType() == tmx::Property::Type::Int) {
			arch_t value = static_cast<arch_t>(property.getIntValue());
			return static_cast<direction_t>(value);
		}
		return direction_t::Right;
	}

	glm::vec4 prop_to_color(const tmx::Property& property) {
		if (property.getType() == tmx::Property::Type::Colour) {
			return color_to_color(property.getColourValue());
		}
		return {};
	}

	glm::vec4 color_to_color(const tmx::Colour& color) {
		return {
			static_cast<real_t>(color.r) / 255.0f,
			static_cast<real_t>(color.g) / 255.0f,
			static_cast<real_t>(color.b) / 255.0f,
			static_cast<real_t>(color.a) / 255.0f
		};
	}

	glm::vec2 vec_to_vec(const tmx::Vector2f& position) {
		return { position.x, position.y };
	}

	std::string prop_to_string(const tmx::Property& property) {
		if (property.getType() == tmx::Property::Type::String) {
			return property.getStringValue();
		}
		return {};
	}

	std::string prop_to_path(const tmx::Property& property) {
		if (property.getType() == tmx::Property::Type::File) {
			return path_to_name(property.getFileValue());
		}
		return {};
	}

	std::string path_to_name(const std::string& path) {
		arch_t position = path.find_last_of('/') + 1;
		return path.substr(position, path.find_last_of('.') - position);
	}

	rect_t rect_to_rect(const tmx::FloatRect& rectangle) {
		return {
			rectangle.left,
			rectangle.top,
			rectangle.width,
			rectangle.height
		};
	}
}
