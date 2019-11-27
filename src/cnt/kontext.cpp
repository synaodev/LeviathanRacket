#include "./kontext.hpp"
#include "./location.hpp"
#include "./kinematics.hpp"
#include "./health.hpp"
#include "./sprite.hpp"
#include "./routine.hpp"
#include "./blinker.hpp"
#include "./liquid.hpp"

#include <cinttypes>
#include <angelscript.h>
#include <tmxlite/ObjectGroup.hpp>

#include "../sys/kernel.hpp"
#include "../eve/receiver.hpp"
#include "../utl/misc.hpp"
#include "../utl/hash.hpp"
#include "../utl/logger.hpp"
#include "../utl/tmx_convert.hpp"

kontext_t::kontext_t() :
	panic_draw(false),
	liquid_flag(false),
	registry(),
	spawn_commands(),
	ctor_table(),
	run_event(),
	push_event()
{

}

bool kontext_t::init(receiver_t& receiver) {
	run_event.connect<&receiver_t::run_event>(&receiver);
	push_event.connect<&receiver_t::push_from_function>(&receiver);
	return true;
}

void kontext_t::reset() {
	panic_draw = false;
	liquid_flag = false;
	auto view = registry.view<actor_header_t>();
	for (auto&& actor : view) {
		registry.destroy(actor);
	}
	spawn_commands.clear();
}

void kontext_t::handle(audio_t& audio, receiver_t& receiver, camera_t& camera, naomi_state_t& naomi_state, tilemap_t& tilemap) {
	kinematics_t::handle(*this, tilemap);
	routine_t::handle(audio, camera, naomi_state, *this, tilemap);
	health_t::handle(audio, receiver, naomi_state, *this);
	if (liquid_flag) {
		liquid::handle(audio, *this);
	}
	if (!spawn_commands.empty()) {
		for (auto&& spawn : spawn_commands) {
			this->create(spawn);
		}
		spawn_commands.clear();
	}
}

void kontext_t::update(real64_t delta) {
	panic_draw = false;
	sprite_t::update(*this, delta);
	blinker_t::update(*this, delta);
}

void kontext_t::render(renderer_t& renderer, rect_t viewport) const {
	sprite_t::render(*this, renderer, viewport, panic_draw);
	if (liquid_flag) {
		liquid::render(*this, renderer, viewport);
	}
	if constexpr (misc::kHitboxes) {
		location_t::render(*this, renderer, viewport);
	}
}

entt::entity kontext_t::search_type(arch_t type) const {
	entt::view<actor_header_t> view = registry.view<actor_header_t>();
	for (auto&& actor : view) {
		auto& header = registry.get<actor_header_t>(actor);
		if (header.type == type) {
			return actor;
		}
	}
	return entt::null;
}

entt::entity kontext_t::search_id(sint_t identity) const {
	if (identity > 0) {
		entt::view<actor_trigger_t> view = registry.view<actor_trigger_t>();
		for (auto&& actor : view) {
			auto& trigger = registry.get<actor_trigger_t>(actor);
			if (trigger.identity == identity) {
				return actor;
			}
		}
	}
	return entt::null;
}

void kontext_t::destroy_id(sint_t identity) {
	entt::entity actor = this->search_id(identity);
	if (actor != entt::null) {
		this->dispose(actor);
	}
}

void kontext_t::kill_id(sint_t identity) {
	entt::entity actor = this->search_id(identity);
	if (actor != entt::null) {
		if (registry.has<health_t>(actor)) {
			auto& health = registry.get<health_t>(actor);
			health.current = 0;
		}
	}
}

static const byte_t kMapActor[] = "actor";
static const byte_t kMapWater[] = "water";

bool kontext_t::create(const std::string& name, glm::vec2 position, direction_t direction, sint_t identity, arch_t flags) {
	arch_t type = SYNAO_HASH(name.c_str());
	auto iter = ctor_table.find(type);
	if (iter != ctor_table.end()) {
		entt::entity actor = registry.create();
		registry.assign<actor_header_t>(actor, type);
		registry.assign<location_t>(actor, position, direction);
		if (identity != 0) {
			registry.assign<actor_trigger_t>(actor, identity, flags);
		}
		iter->second(actor, *this);
		return true;
	}
	SYNAO_LOG("Couldn't create %s!\n", name.c_str());
	return false;
}

bool kontext_t::create(const std::string& name, real_t x, real_t y, direction_t direction, sint_t identity, arch_t flags, asIScriptFunction* function) {
	arch_t type = SYNAO_HASH(name.c_str());
	auto iter = ctor_table.find(type);
	if (iter != ctor_table.end()) {
		spawn_commands.emplace_back(type, glm::vec2(x, y), direction, identity, flags);
		std::invoke(push_event, identity, function);
	} else if (function != nullptr) {
		function->Release();
	}
}

bool kontext_t::create(const actor_spawn_t& spawn) {
	auto iter = ctor_table.find(spawn.type);
	if (iter != ctor_table.end()) {
		entt::entity actor = registry.create();
		registry.assign<actor_header_t>(actor, spawn.type);
		registry.assign<location_t>(actor, spawn.position, spawn.direction);
		if (spawn.velocity != glm::zero<glm::vec2>()) {
			registry.assign<kinematics_t>(actor, spawn.velocity);
		}
		if (spawn.identity != 0) {
			registry.assign<actor_trigger_t>(actor, spawn.identity, spawn.bitmask);
		}
		iter->second(actor, *this);
		return true;
	}
	SYNAO_LOG(
#ifdef SYNAO_MACHINE_x64
	"Couldn't spawn actor %" PRIu64 "!\n",
#else // SYNAO_MACHINE_x64
	"Couldn't spawn actor %u!\n",
#endif // SYNAO_MACHINE_x64
		spawn.type
	);
	return false;
}

void kontext_t::setup_layer(const std::unique_ptr<tmx::Layer>& layer, const kernel_t& kernel, receiver_t& receiver) {
	auto& objects = dynamic_cast<tmx::ObjectGroup*>(layer.get())->getObjects();
	for (auto&& object : objects) {
		const std::string& name = object.getName();
		const std::string& type = object.getType();
		if (type == kMapActor) {
			direction_t direction = direction_t::Right;
			std::string symbol;
			arch_t flags = 0;
			sint_t identity = 0;
			arch_t deterrent = 0;
			tmx_convert::prop_to_stats(
				object.getProperties(),
				direction, symbol,
				flags, identity, deterrent
			);
			if (kernel.get_flag(deterrent) == (flags & trigger_bits_t::Deterred)) {
				glm::vec2 position = tmx_convert::vec_to_vec(object.getPosition());
				if (this->create(name, position, direction, identity, flags)) {
					if (identity != 0) {
						const std::string& field = kernel.get_field();
						receiver.push_from_symbol(identity, field, symbol);
					}
				}
			}
		} else if (type == kMapWater) {
			liquid_flag = true;
			rect_t hitbox = tmx_convert::rect_to_rect(object.getAABB());
			entt::entity actor = registry.create();
			registry.assign<actor_header_t>(actor);
			registry.assign<liquid_body_t>(actor, hitbox);
		}
	}
}

void kontext_t::smoke(glm::vec2 position, arch_t count) {
	while (count > 0) {
		// this->spawn(ai::smoke::type, position);
		--count;
	}
}

void kontext_t::smoke(real_t x, real_t y, arch_t count) {
	this->smoke(glm::vec2(x, y), count);
}

void kontext_t::shrapnel(glm::vec2 position, arch_t count) {
	while (count > 0) {
		// this->spawn(ai::shrapnel::type, position);
		--count;
	}
}

void kontext_t::shrapnel(real_t x, real_t y, arch_t count) {
	this->shrapnel(glm::vec2(x, y), count);
}

void kontext_t::bump(sint_t identity, direction_t direction, real_t velocity_x, real_t velocity_y) {
	entt::entity actor = this->search_id(identity);
	if (actor != entt::null) {
		auto& location = registry.get<location_t>(actor);
		location.direction = direction;
		if (registry.has<sprite_t>(actor)) {
			auto& sprite = registry.get<sprite_t>(actor);
			sprite.write = true;
			sprite.direction = direction;
		}
		if (registry.has<kinematics_t>(actor)) {
			auto& kinematics = registry.get<kinematics_t>(actor);
			kinematics.velocity = glm::vec2(velocity_x, velocity_y);
		}
	}
}

void kontext_t::animate(sint_t identity, arch_t state, direction_t direction) {
	entt::entity actor = this->search_id(identity);
	if (actor != entt::null) {
		if (registry.has<sprite_t>(actor)) {
			auto& location = registry.get<location_t>(actor);
			auto& sprite = registry.get<sprite_t>(actor);
			if (direction != direction_t::Neutral) {
				location.direction = direction;
				sprite.write = true;
				sprite.direction = direction;
			}
			sprite.new_state(state);
		}
	}
}

void kontext_t::set_state(sint_t identity, arch_t state) {
	entt::entity actor = this->search_id(identity);
	if (actor != entt::null) {
		if (registry.has<routine_t>(actor)) {
			auto& routine = registry.get<routine_t>(actor);
			routine.state = state;
		}
	}
}

void kontext_t::set_mask(sint_t identity, arch_t index, bool value) {
	entt::entity actor = this->search_id(identity);
	if (actor != entt::null) {
		auto& trigger = registry.get<actor_trigger_t>(actor);
		trigger.bitmask[index] = value;
	}
}

void kontext_t::set_event(sint_t identity, asIScriptFunction* function) {
	entt::entity actor = this->search_id(identity);
	if (actor != entt::null) {
		std::invoke(push_event, identity, function);
	} else if (function != nullptr) {
		function->Release();
	}
}

bool kontext_t::still(sint_t identity) const {
	entt::entity actor = this->search_id(identity);
	if (actor != entt::null) {
		if (registry.has<kinematics_t>(actor)) {
			auto& kinematics = registry.get<kinematics_t>(actor);
			if (kinematics.velocity == glm::zero<glm::vec2>()) {
				return true;
			} else if (kinematics.velocity.y != 0.0f) {
				return kinematics.velocity.x == 0.0f and kinematics.flags[phy_t::Bottom];
			} else {
				return false;
			}
		}
	}
	return true;
}