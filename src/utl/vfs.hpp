#ifndef SYNAO_UTILITY_VFS_HPP
#define SYNAO_UTILITY_VFS_HPP

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

#include "../sfx/noise.hpp"
#include "../gfx/texture.hpp"
#include "../gfx/palette.hpp"
#include "../gfx/program.hpp"
#include "../gfx/font.hpp"
#include "../gfx/animation.hpp"
#include "../eve/receiver.hpp"
#include "./thread_pool.hpp"

struct setup_file_t;

namespace vfs {
	struct __vfs_t;
	static __vfs_t* device = nullptr;
	struct __vfs_t : public not_copyable_t, public not_moveable_t {
	public:
		__vfs_t() : 
			thread_pool(),
			storage_mutex(),
			language(),
			i18n(),
			noises(),
			textures(),
			palettes(),
			shaders(),
			fonts(),
			animations() {}
		~__vfs_t() {
			if (this == vfs::device) {
				vfs::device = nullptr;
			}
		}
		template<typename T>
		T& allocate_safely(const std::string& name, std::unordered_map<std::string, T>& map) {
			std::lock_guard<std::mutex> lock{vfs::device->storage_mutex};
			return map[name];
		}
	public:
		static constexpr arch_t kTotalThreads = 4;
		std::unique_ptr<thread_pool_t> thread_pool;
		std::mutex storage_mutex;
		std::string language;
		std::unordered_map<std::string, std::vector<std::string> > i18n;
		std::unordered_map<std::string, noise_t> noises;
		std::unordered_map<std::string, texture_t> textures;
		std::unordered_map<std::string, palette_t> palettes;
		std::unordered_map<std::string, shader_t> shaders;
		std::unordered_map<std::string, font_t> fonts;
		std::unordered_map<std::string, animation_t> animations;
	};
	std::unique_ptr<__vfs_t> mount(const setup_file_t& config);
	std::back_insert_iterator<std::u32string> to_utf32(
		std::string::const_iterator begin, 
		std::string::const_iterator end, 
		std::back_insert_iterator<std::u32string> output
	);
	std::vector<std::string> file_list(const std::string& directory);
	std::string string_buffer(const std::string& path);
	std::vector<byte_t> byte_buffer(const std::string& path);
	std::vector<sint_t> sint_buffer(const std::string& path);
	std::string event_path(const std::string& name, rec_loading_t flags);
	std::string i18n_find(const std::string& segment, arch_t index);
	std::string i18n_find(const std::string& segment, arch_t first, arch_t last);
	arch_t i18n_size(const std::string& segment);
	bool try_language(const std::string& language);
	std::string local_script(const std::string& name);
	std::string global_script(const std::string& name);
	const noise_t* noise(const std::string& name);
	const texture_t* texture(const std::vector<std::string>& names, const std::string& directory);
	const texture_t* texture(const std::vector<std::string>& names);
	const texture_t* texture(const std::string& name);
	const palette_t* palette(const std::string& name, const std::string& directory);
	const palette_t* palette(const std::string& name);
	const shader_t* shader(const std::string& name, shader_stage_t stage);
	const font_t* font(const std::string& name);
	const font_t* font(arch_t index);
	const animation_t* animation(const std::string& name);
}

#endif // SYNAO_UTILITY_VFS_HPP