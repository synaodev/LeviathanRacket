#include "./video.hpp"

#include "../video/frame_buffer.hpp"
#include "../video/glcheck.hpp"
#include "../resource/icon.hpp"
#include "../utility/constants.hpp"
#include "../utility/setup_file.hpp"
#include "../utility/logger.hpp"

#include <SDL2/SDL.h>

video_t::video_t() :
	window(nullptr),
	context(nullptr),
	params(),
	major(4),
	minor(6)
{

}

video_t::~video_t() {
	if (context != nullptr) {
		SDL_GL_DeleteContext(context);
		context = nullptr;
	}
	if (window != nullptr) {
		SDL_DestroyWindow(window);
		window = nullptr;
	}
}

bool video_t::init(const setup_file_t& config, bool editor) {
	config.get("Video", "VerticalSync", params.vsync);
	config.get("Video", "Fullscreen", 	params.full);
	config.get("Video", "ScaleFactor", params.scaling);
	config.get("Video", "FrameLimiter", params.framerate);
	// Use OpenGL 4.X by default.
	bool_t use_opengl_4 = true;
	config.get("Video", "UseOpenGL4", use_opengl_4);
	if (!use_opengl_4) {
		this->major = 3;
		this->minor = 3;
	}
	// Setup parameters.
	params.scaling = glm::clamp(
		params.scaling,
		screen_params_t::kDefaultScaling,
		screen_params_t::kHighestScaling
	);
	params.framerate = glm::max(
		params.framerate,
		screen_params_t::kDefaultFramerate
	);
	if (window != nullptr) {
		synao_log("Window already created!\n");
		return false;
	}
	if (context != nullptr) {
		synao_log("OpenGL context already created!\n");
		return false;
	}
#if defined(LEVIATHAN_PLATFORM_MACOS)
	// MacOS build needs to set Forward Compatible Flags.
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG) < 0) {
		synao_log("Setting context flags failed! SDL Error: {}\n", SDL_GetError());
		return false;
	}
#endif
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) < 0) {
		synao_log("Setting OpenGL Core profile failed! SDL Error: {}\n", SDL_GetError());
		return false;
	}
	if (SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0) < 0) {
		synao_log("Setting depth buffer size failed! SDL Error: {}\n", SDL_GetError());
		return false;
	}
	if (SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0) < 0) {
		synao_log("Setting stencil buffer size failed! SDL Error: {}\n", SDL_GetError());
		return false;
	}
	if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) < 0) {
		synao_log("Setting double-buffering failed! SDL Error: {}\n", SDL_GetError());
		return false;
	}
	// Create window.
	if (editor) {
		window = SDL_CreateWindow(
			constants::EditorName,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			constants::EditorWidth<sint_t>(),
			constants::EditorHeight<sint_t>(),
			SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
		);
	} else {
		window = SDL_CreateWindow(
			constants::NormalName,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			constants::NormalWidth<sint_t>() * params.scaling,
			constants::NormalHeight<sint_t>() * params.scaling,
			SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
		);
	}
	if (window == nullptr) {
		synao_log("Window creation failed! SDL Error: {}\n", SDL_GetError());
		return false;
	}
	// Try to set fullscreen.
	if (params.full and SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) < 0) {
		synao_log("Fullscreen after window creation failed! SDL Error: {}\n", SDL_GetError());
		return false;
	}
	// Try every OpenGL version from 4.6 to 3.3 and break when no errors.
	while (1) {
		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, this->major) < 0) {
			synao_log("Setting OpenGL major version failed! SDL Error: {}\n", SDL_GetError());
			return false;
		}
		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, this->minor) < 0) {
			synao_log("Setting OpenGL minor version failed! SDL Error: {}\n", SDL_GetError());
			return false;
		}
		context = SDL_GL_CreateContext(window);
		if (context != nullptr) {
			break;
		} else if (this->major == 4 and this->minor > 0) {
			this->minor -= 1;
		} else if (this->major == 4 and this->minor == 0) {
			this->major = 3;
			this->minor = 3;
		} else {
			synao_log("Error! OpenGL version must be at least 3.3!\n");
			break;
		}
	}
	// If OpenGL 3.3 isn't available, it's worth telling the user.
	if (context == nullptr) {
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR,
			"OpenGL Error",
			"Running Leviathan Racket requires at least OpenGL 3.3!\n",
			nullptr
		);
		synao_log("OpenGL context creation failed! SDL Error: {}\n", SDL_GetError());
		return false;
	}
	// Confirm OpenGL version.
	if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &this->major) < 0) {
		synao_log("Getting OpenGL major version failed! SDL Error: {}\n", SDL_GetError());
		return false;
	}
	if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &this->minor) < 0) {
		synao_log("Getting OpenGL minor version failed! SDL Error: {}\n", SDL_GetError());
		return false;
	}
	synao_log("OpenGL Version is {}.{}!\n", this->major, this->minor);
	// Load OpenGL extensions with GLAD
	if (gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress) == 0) {
		synao_log("OpenGL Extension loading failed!\n");
		return false;
	}
	// Clear and swap so the screen isn't left blank.
	if (editor) {
		frame_buffer_t::clear(
			constants::EditorDimensions<sint_t>(),
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
		);
	} else {
		frame_buffer_t::clear(
			constants::NormalDimensions<sint_t>() * params.scaling,
			glm::vec4(0.0f, 0.0f, 0.125f, 1.0f)
		);
	}
	SDL_GL_SwapWindow(window);
	// Try to set v-sync state.
	if (!editor and SDL_GL_SetSwapInterval(params.vsync) < 0) {
		synao_log("Vertical sync after OpenGL context creation failed! SDL Error: {}\n", SDL_GetError());
		return false;
	}
	// Set window icon.
	SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(
		icon::pixels(),
		icon::width(),
		icon::height(),
		icon::depth(),
		icon::pitch(),
		SDL_PIXELFORMAT_RGBA32
	);
	if (surface != nullptr) {
		SDL_SetWindowIcon(window, surface);
		SDL_FreeSurface(surface);
		surface = nullptr;
	} else {
		synao_log("Icon surface creation failed! SDL Error: {}\n", SDL_GetError());
	}
	synao_log("Video system initialized.\n");
	return true;
}

void video_t::submit(const frame_buffer_t* frame_buffer, arch_t index) const {
	if (frame_buffer != nullptr) {
		const glm::ivec2 source_dimensions = frame_buffer->get_integral_dimensions();
		const glm::ivec2 destination_dimensions = this->get_integral_dimensions();
		frame_buffer_t::bind(nullptr, frame_buffer_binding_t::Write, 0);
		frame_buffer_t::bind(frame_buffer, frame_buffer_binding_t::Read, index);
		frame_buffer_t::blit(source_dimensions, destination_dimensions);
	}
}

void video_t::flush() const {
	if (window != nullptr and context != nullptr) {
		SDL_GL_SwapWindow(window);
	}
}

void video_t::set_parameters(screen_params_t params) {
	if (this->params.vsync != params.vsync) {
		this->params.vsync = params.vsync;
		if (SDL_GL_SetSwapInterval(params.vsync) < 0) {
			synao_log("Vertical sync change failed! SDL Error: {}\n", SDL_GetError());
		}
	}
	if (this->params.full != params.full) {
		this->params.full = params.full;
		if (SDL_SetWindowFullscreen(window, params.full ? SDL_WINDOW_FULLSCREEN : 0) < 0) {
			synao_log("Window mode change failed! SDL Error: {}\n", SDL_GetError());
		} else {
			SDL_SetWindowPosition(
				window,
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED
			);
		}
	}
	if (this->params.scaling != params.scaling) {
		this->params.scaling = params.scaling;
		this->params.scaling = glm::clamp(
			this->params.scaling,
			screen_params_t::kDefaultScaling,
			screen_params_t::kHighestScaling
		);
		SDL_SetWindowSize(
			window,
			constants::NormalWidth<sint_t>() * params.scaling,
			constants::NormalHeight<sint_t>() * params.scaling
		);
		SDL_SetWindowPosition(
			window,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED
		);
	}
	if (this->params.framerate != params.framerate) {
		this->params.framerate = glm::max(
			params.framerate,
			screen_params_t::kDefaultFramerate
		);
	}
}

const screen_params_t& video_t::get_parameters() const {
	return params;
}

glm::vec2 video_t::get_dimensions() const {
	return glm::vec2(this->get_integral_dimensions());
}

glm::ivec2 video_t::get_integral_dimensions() const {
	return constants::NormalDimensions<sint_t>() * params.scaling;
}

glm::ivec2 video_t::get_editor_dimensions() const {
	return constants::EditorDimensions<sint_t>();
}

glm::ivec2 video_t::get_opengl_version() const {
	return glm::ivec2(major, minor);
}
