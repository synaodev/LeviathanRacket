#include "./texture.hpp"
#include "./gl-check.hpp"

#include "../utility/logger.hpp"
#include "../utility/thread-pool.hpp"

#include <glm/gtc/constants.hpp>

texture_t::texture_t(texture_t&& that) noexcept : texture_t() {
	if (this != &that) {
		bool this_temp = ready.load();
		ready.store(false);
		bool that_temp = that.ready.load();
		that.ready.store(false);

		std::swap(future, that.future);
		std::swap(allocator, that.allocator);
		std::swap(name, that.name);
		std::swap(dimensions, that.dimensions);

		ready.store(that_temp);
		that.ready.store(this_temp);
	}
}

texture_t& texture_t::operator=(texture_t&& that) noexcept {
	if (this != &that) {
		bool this_temp = ready.load();
		ready.store(false);
		bool that_temp = that.ready.load();
		that.ready.store(false);

		std::swap(future, that.future);
		std::swap(allocator, that.allocator);
		std::swap(name, that.name);
		std::swap(dimensions, that.dimensions);

		ready.store(that_temp);
		that.ready.store(this_temp);
	}
	return *this;
}

texture_t::~texture_t() {
	if (future.valid()) {
		auto result = future.get();
	}
}

void texture_t::load(const std::string& full_path, sampler_allocator_t* allocator, thread_pool_t& thread_pool) {
	assert(!ready);
	this->allocator = allocator;
	this->future = thread_pool.push([](const std::string& full_path) -> image_t {
		return image_t::generate(full_path);
	}, full_path);
}

void texture_t::assure() {
	if (!ready and future.valid()) {
		// Load texture now
		const image_t image = future.get();
		if (!image.empty()) {
			const glm::ivec2 dimensions = image.get_dimensions();
			auto& handle = allocator->texture(dimensions);

			this->dimensions = dimensions;
			this->name = handle.count;

			if ((handle.count + 1) < sampler_t::get_maximum_textures()) {
				// Save previous unit and set to working unit
				sint_t previous = 0;
				glCheck(glGetIntegerv(GL_ACTIVE_TEXTURE, &previous));
				glCheck(glActiveTexture(sampler_t::get_working_unit()));

				glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, handle.id));
				glCheck(glTexSubImage3D(
					GL_TEXTURE_2D_ARRAY, 0, 0, 0,
					handle.count, dimensions.x, dimensions.y, 1,
					GL_RGBA, GL_UNSIGNED_BYTE, &image[0]
				));
				if (sampler_t::has_immutable_option()) {
					glCheck(glGenerateMipmap(GL_TEXTURE_2D_ARRAY));
				}
				glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));

				// Restore previous unit
				glCheck(glActiveTexture(previous));

				// Increment texture count
				++handle.count;
			} else {
				synao_log("Warning! This texture handle has no space left! This message should not print!\n");
			}
		}
		ready = true;
	}
}

void texture_t::assure() const {
	if (!ready) {
		const_cast<texture_t*>(this)->assure();
	}
}

bool texture_t::valid() const {
	return dimensions != glm::zero<glm::ivec2>();
}

uint_t texture_t::get_handle() const {
	this->assure();
	if (allocator) {
		return allocator->texture().id;
	}
	return 0;
}

sint_t texture_t::get_name() const {
	this->assure();
	return name;
}

glm::vec2 texture_t::get_dimensions() const {
	this->assure();
	return glm::vec2(dimensions);
}

glm::vec2 texture_t::get_inverse_dimensions() const {
	this->assure();
	if (dimensions.x != 0.0f and dimensions.y != 0.0f) {
		return 1.0f / glm::vec2(dimensions);
	}
	return glm::one<glm::vec2>();
}

glm::ivec2 texture_t::get_integral_dimensions() const {
	this->assure();
	return dimensions;
}

atlas_t::atlas_t(atlas_t&& that) noexcept : atlas_t() {
	if (this != &that) {
		bool this_temp = ready.load();
		ready.store(false);
		bool that_temp = that.ready.load();
		that.ready.store(false);

		std::swap(future, that.future);
		std::swap(allocator, that.allocator);
		std::swap(name, that.name);
		std::swap(dimensions, that.dimensions);

		ready.store(that_temp);
		that.ready.store(this_temp);
	}
}

atlas_t& atlas_t::operator=(atlas_t&& that) noexcept {
	if (this != &that) {
		bool this_temp = ready.load();
		ready.store(false);
		bool that_temp = that.ready.load();
		that.ready.store(false);

		std::swap(future, that.future);
		std::swap(allocator, that.allocator);
		std::swap(name, that.name);
		std::swap(dimensions, that.dimensions);

		ready.store(that_temp);
		that.ready.store(this_temp);
	}
	return *this;
}

atlas_t::~atlas_t() {
	if (future.valid()) {
		auto result = future.get();
	}
}

void atlas_t::load(const std::string& full_path, sampler_allocator_t* allocator, thread_pool_t& thread_pool) {
	assert(!ready);
	this->allocator = allocator;
	this->future = thread_pool.push([](const std::string& full_path) -> image_t {
		return image_t::generate(full_path);
	}, full_path);
}

void atlas_t::assure() {
	if (!ready and future.valid()) {
		// Load texture now
		const image_t image = future.get();
		if (!image.empty()) {
			const glm::ivec2 dimensions = image.get_dimensions();
			auto& handle = allocator->atlas(dimensions);

			this->dimensions = dimensions;
			this->name = handle.count;

			if ((handle.count + 1) < sampler_t::get_maximum_atlases()) {
				// Save previous unit and set to working unit
				sint_t previous = 0;
				glCheck(glGetIntegerv(GL_ACTIVE_TEXTURE, &previous));
				glCheck(glActiveTexture(sampler_t::get_working_unit()));

				glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, handle.id));
				glCheck(glTexSubImage3D(
					GL_TEXTURE_2D_ARRAY, 0, 0, 0,
					handle.count, dimensions.x, dimensions.y, 1,
					GL_RGBA, GL_UNSIGNED_BYTE, &image[0]
				));
				if (sampler_t::has_immutable_option()) {
					glCheck(glGenerateMipmap(GL_TEXTURE_2D_ARRAY));
				}
				glCheck(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));

				// Restore previous unit
				glCheck(glActiveTexture(previous));

				// Increment texture count
				++handle.count;
			} else {
				synao_log("Warning! This atlas handle has no space left! This message should not print!\n");
			}
		}
		ready = true;
	}
}

void atlas_t::assure() const {
	if (!ready) {
		const_cast<atlas_t*>(this)->assure();
	}
}

bool atlas_t::valid() const {
	return dimensions != glm::zero<glm::ivec2>();
}

uint_t atlas_t::get_handle() const {
	this->assure();
	if (allocator) {
		return allocator->atlas().id;
	}
	return 0;
}

sint_t atlas_t::get_name() const {
	this->assure();
	return name;
}

glm::vec2 atlas_t::get_dimensions() const {
	this->assure();
	return glm::vec2(dimensions);
}

glm::vec2 atlas_t::get_inverse_dimensions() const {
	this->assure();
	if (dimensions.x != 0.0f and dimensions.y != 0.0f) {
		return 1.0f / glm::vec2(dimensions);
	}
	return glm::one<glm::vec2>();
}

glm::ivec2 atlas_t::get_integral_dimensions() const {
	this->assure();
	return dimensions;
}

palette_t::palette_t(palette_t&& that) noexcept : palette_t() {
	if (this != &that) {
		bool this_temp = ready.load();
		ready.store(false);
		bool that_temp = that.ready.load();
		that.ready.store(false);

		std::swap(future, that.future);
		std::swap(allocator, that.allocator);
		std::swap(name, that.name);
		std::swap(dimensions, that.dimensions);

		ready.store(that_temp);
		that.ready.store(this_temp);
	}
}

palette_t& palette_t::operator=(palette_t&& that) noexcept {
	if (this != &that) {
		bool this_temp = ready.load();
		ready.store(false);
		bool that_temp = that.ready.load();
		that.ready.store(false);

		std::swap(future, that.future);
		std::swap(allocator, that.allocator);
		std::swap(name, that.name);
		std::swap(dimensions, that.dimensions);

		ready.store(that_temp);
		that.ready.store(this_temp);
	}
	return *this;
}

palette_t::~palette_t() {
	if (future.valid()) {
		auto result = future.get();
	}
}

void palette_t::load(const std::string& full_path, sampler_allocator_t* allocator, thread_pool_t& thread_pool) {
	assert(!ready);
	this->allocator = allocator;
	this->future = thread_pool.push([](const std::string& full_path) -> image_t {
		return image_t::generate(full_path);
	}, full_path);
}

void palette_t::assure() {
	if (!ready and future.valid()) {
		// Load palette now
		const image_t image = future.get();
		if (!image.empty()) {
			const glm::ivec2 dimensions = image.get_dimensions();
			auto& handle = allocator->palette(dimensions);

			this->dimensions = dimensions;
			this->name = handle.count;

			if ((handle.count + dimensions.y) < sampler_t::get_maximum_palettes()) {
				// Save previous unit and set to working unit
				sint_t previous = 0;
				glCheck(glGetIntegerv(GL_ACTIVE_TEXTURE, &previous));
				glCheck(glActiveTexture(sampler_t::get_working_unit()));

				glCheck(glBindTexture(GL_TEXTURE_1D_ARRAY, handle.id));
				glCheck(glTexSubImage2D(
					GL_TEXTURE_1D_ARRAY, 0, 0,
					handle.count, dimensions.x, dimensions.y,
					GL_RGBA, GL_UNSIGNED_BYTE, &image[0]
				));
				if (sampler_t::has_immutable_option()) {
					glCheck(glGenerateMipmap(GL_TEXTURE_1D_ARRAY));
				}
				glCheck(glBindTexture(GL_TEXTURE_1D_ARRAY, 0));

				// Restore previous unit
				glCheck(glActiveTexture(previous));

				// Add to palette count
				handle.count += dimensions.y;
			} else {
				synao_log("Warning! This palette handle has no space left! This message should not print!\n");
			}
		}
		ready = true;
	}
}

void palette_t::assure() const {
	if (!ready) {
		const_cast<palette_t*>(this)->assure();
	}
}

bool palette_t::valid() const {
	return dimensions != glm::zero<glm::ivec2>();
}

uint_t palette_t::get_handle() const {
	this->assure();
	if (allocator) {
		return allocator->palette().id;
	}
	return 0;
}

sint_t palette_t::get_name() const {
	this->assure();
	return name;
}

real_t palette_t::convert(real_t index) const {
	this->assure();
	if (dimensions.y > 0) {
		return index / static_cast<real_t>(dimensions.y);
	}
	return 0.0f;
}
