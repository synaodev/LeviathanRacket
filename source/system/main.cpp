#include "./main.hpp"

#include <cstdlib>
#include <cstring>
#include <csignal>
#include <atomic>
#include <iostream>
#include <SDL2/SDL.h>

#include "./input.hpp"
#include "./video.hpp"
#include "./audio.hpp"
#include "./music.hpp"
#include "./renderer.hpp"
#include "./runtime.hpp"

#include "../resource/vfs.hpp"
#include "../utility/constants.hpp"
#include "../utility/logger.hpp"
#include "../utility/setup-file.hpp"

static std::atomic<bool> interrupt = false;
static void sigint_handler(int) {
	interrupt = true;
}

static constexpr uint_t kStopDelay = 40;
static constexpr uint_t kNormDelay = 10;

static bool main_loop(setup_file_t& config, input_t& input, video_t& video, audio_t& audio, music_t& music, renderer_t& renderer) {
	policy_t policy = policy_t::Run;
	runtime_t runtime;
	if (!runtime.init(input, video, audio, music, renderer)) {
		synao_log("Runtime initialization failed!\n");
		return false;
	}
	synao_log("Entering main loop...\n");
	watch_t sync_watch, head_watch;
	while (policy != policy_t::Quit) {
		if (interrupt) {
			synao_log("[SIGINT]\n");
			break;
		}
		policy = input.poll(policy, meta_state_t::get_event_callback());
		if (policy != policy_t::Stop) {
			runtime.update(head_watch.restart());
			if (runtime.viable()) {
				if (runtime.handle(config, input, video, audio, music, renderer)) {
					runtime.render(video, renderer);
					auto& params = video.get_parameters();
					if (params.vsync != 0) {
						SDL_Delay(kNormDelay);
					} else {
						real64_t waiting = (1.0 / params.framerate) - sync_watch.elapsed();
						if (waiting > 0.0) {
							uint_t ticks = static_cast<uint_t>(waiting * 1000.0);
							SDL_Delay(ticks);
						}
						sync_watch.restart();
					}
				} else {
					policy = policy_t::Quit;
				}
			}
		} else {
			SDL_Delay(kStopDelay);
		}
	}
	if (!input.save(config)) {
		synao_log("Warning! Something went wrong when trying to save inputs to macro file!\n");
	}
	return config.save();
}

static int process(setup_file_t& config) {
	// Global input/video/audio devices are generated here...
	input_t input;
	if (!input.init(config)) {
		return EXIT_FAILURE;
	}
	video_t video;
	if (!video.init(config)) {
		return EXIT_FAILURE;
	}
	audio_t audio;
	if (!audio.init(config)) {
		return EXIT_FAILURE;
	}
	// Global virtual filesystem device generated here.
	// Accessible from anywhere in order to reduce headaches.
	// Must destroy this before destroying video and audio devices.
	vfs_t fs;
	if (!fs.init(config)) {
		return EXIT_FAILURE;
	}
	// Global music device is dependent on existance of audio device.
	// Must destroy this before destroying audio device.
	music_t music;
	if (!music.init(config)) {
		return EXIT_FAILURE;
	}
	// Global renderer device is dependent on existance of virtual filesystem and audio devices.
	// Must destroy this before destroying virtual filesystem and audio devices.
	renderer_t renderer;
	if (!renderer.init(fs)) {
		return EXIT_FAILURE;
	}
	if (!main_loop(config, input, video, audio, music, renderer)) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

static std::string get_boot_path() {
	const std::string init_path = vfs_t::resource_path(vfs_resource_path_t::Init);
	return init_path + "boot.cfg";
}

static void write_config(setup_file_t& config, const std::string& boot_path) {
	config.clear(boot_path);
	config.set("Setup", "MetaMenu", 0);
	config.set("Setup", "LegacyGL", 0);
	config.set("Setup", "Language", std::string("english"));
	config.set("Video", "VerticalSync", 0);
	config.set("Video", "Fullscreen", 0);
	config.set("Video", "ScaleFactor", 3);
	config.set("Video", "FrameLimiter", 60);
	config.set("Audio", "Volume", 1.0f);
	config.set("Music", "Volume", 0.34f);
	config.set("Music", "Channels", 2);
	config.set("Music", "SamplingRate", 44100);
	config.set("Music", "BufferedTime", 0.1f);
	config.set("Input", "KeyJump", (sint_t)SDL_SCANCODE_Z);
	config.set("Input", "KeyHammer", (sint_t)SDL_SCANCODE_X);
	config.set("Input", "KeyItem", (sint_t)SDL_SCANCODE_LSHIFT);
	config.set("Input", "KeyLiteDash", (sint_t)SDL_SCANCODE_A);
	config.set("Input", "KeyContext", (sint_t)SDL_SCANCODE_SPACE);
	config.set("Input", "KeyStrafe", (sint_t)SDL_SCANCODE_LCTRL);
	config.set("Input", "KeyInventory", (sint_t)SDL_SCANCODE_TAB);
	config.set("Input", "KeyOptions", (sint_t)SDL_SCANCODE_ESCAPE);
	config.set("Input", "KeyUp", (sint_t)SDL_SCANCODE_UP);
	config.set("Input", "KeyDown", (sint_t)SDL_SCANCODE_DOWN);
	config.set("Input", "KeyLeft", (sint_t)SDL_SCANCODE_LEFT);
	config.set("Input", "KeyRight", (sint_t)SDL_SCANCODE_RIGHT);
	config.set("Input", "JoyJump", 0);
	config.set("Input", "JoyHammer", 1);
	config.set("Input", "JoyItem", 2);
	config.set("Input", "JoyLiteDash", 3);
	config.set("Input", "JoyContext", 4);
	config.set("Input", "JoyStrafe", 5);
	config.set("Input", "JoyInventory",	6);
	config.set("Input", "JoyOptions", 7);
	const std::string init_path = vfs_t::resource_path(vfs_resource_path_t::Init);
	if (!vfs_t::create_directory(init_path)) {
		synao_log("Warning! Will not be able to save newly generated config file!\n");
	}
}

static setup_file_t load_config() {
	setup_file_t result;
	const std::string boot_path = get_boot_path();
	if (!result.load(boot_path)) {
		synao_log("Couldn't find main configuration file at \"{}\"! Generating new config file.\n", boot_path);
		write_config(result, boot_path);
	}
	return result;
}

static bool mount(const byte_t* provided_directory) {
	// Save initial working directory before attempting to change it
	const std::string working_directory = vfs_t::working_directory();
	if (provided_directory) {
		if (vfs_t::mount(provided_directory, false)) {
			return true;
		} else {
			synao_log("Warning! Couldn\'t mount filesystem at directory: \"{}\"!\n", provided_directory);
		}
	}
	// Try to mount (initial) working directory
	if (vfs_t::mount(working_directory, false)) {
		return true;
	}
	// If that doesn't work, look inside executable directory
	const std::string executable_directory = vfs_t::executable_directory();
	if (vfs_t::mount(executable_directory)) {
		return true;
	}
	// If mounting is impossible, notify user of the initial working directory
	synao_log("Warning! Working directory before mounting process was \"{}\"!\n", working_directory);
	return false;
}

int main(int argc, char** argv) {
	// If writing to stdout (debug build), speed up I/O
#ifdef LEVIATHAN_BUILD_DEBUG
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);
	std::cout.tie(nullptr);
#endif
	// Print version information
	synao_log(
		"====== Leviathan Racket ======\n"
		"   Version: {}.{}.{}.{}\n"
		"   Author: Tyler Cadena\n"
		"   Platform: {} {}\n"
		"   Compiler: {}\n"
		"   Toolchain: {}\n"
		"   Build Type: {}\n"
		"==============================\n",
		LEVIATHAN_VERSION_INFORMATION_MAJOR,
		LEVIATHAN_VERSION_INFORMATION_MINOR,
		LEVIATHAN_VERSION_INFORMATION_PATCH,
		LEVIATHAN_VERSION_INFORMATION_TWEAK,
		version_information::platform,
		version_information::architecture,
		version_information::compiler,
		version_information::toolchain,
		version_information::build_type
	);
	// Handle arguments
	{
		const byte_t* directory = nullptr;
		for (sint_t it = 1; it < argc; ++it) {
			const byte_t* option = argv[it];
			if (!directory) {
				directory = option;
			} else {
				break;
			}
		}
		// Set "mounting" directory for virtual filesystem
		if (!mount(directory)) {
			synao_log("Fatal error! Could not mount filesystem!\n");
			return EXIT_FAILURE;
		}
	}
	// Register SIGINT handler
	std::signal(SIGINT, sigint_handler);
	// Initialize SDL2
	if (std::atexit(SDL_Quit) != 0) {
		synao_log("Pushing to \"std::atexit\" buffer failed!\n");
		return EXIT_FAILURE;
	}
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
		synao_log("SDL Initialization failed! SDL Error: {}\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	// Load config
	setup_file_t config = load_config();
	// Run process
	return process(config);
}
