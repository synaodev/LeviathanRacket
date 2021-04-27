#pragma once

#include <vector>
#include <string>
#include <bitset>
#include <unordered_map>

#include "../utility/enums.hpp"

class asIScriptEngine;
class asIScriptContext;
class asIScriptFunction;
class asIScriptModule;
class asIScriptGeneric;

struct asSMessageInfo;

namespace __enum_receiver_bits {
	enum type : arch_t {
		Running,
		Waiting,
		Stalled,
		Total
	};
}

using rec_bits_t = __enum_receiver_bits::type;

struct input_t;
struct audio_t;
struct music_t;
struct kernel_t;
struct stack_gui_t;
struct dialogue_gui_t;
struct inventory_gui_t;
struct headsup_gui_t;
struct camera_t;
struct naomi_state_t;
struct kontext_t;

struct receiver_t : public not_copyable_t {
public:
	receiver_t();
	receiver_t(receiver_t&&) = default;
	receiver_t& operator=(receiver_t&&) = default;
	~receiver_t();
public:
	bool init(input_t& input, audio_t& audio, music_t& music, kernel_t& kernel, stack_gui_t& stack_gui, dialogue_gui_t& dialogue_gui, headsup_gui_t& headsup_gui, camera_t& camera, naomi_state_t& naomi_state, kontext_t& kontext);
	void reset();
	void handle(const input_t& input, kernel_t& kernel, const stack_gui_t& stack_gui, dialogue_gui_t& dialogue_gui, const inventory_gui_t& inventory_gui, headsup_gui_t& headsup_gui);
	void update(real64_t delta);
	bool running() const;
	bool load(const kernel_t& kernel);
	bool load(const std::string& name);
	bool load(const std::string& name, event_loading_t flags);
	void run_function(kernel_t& kernel);
	void run_event(sint_t id);
	void run_inventory(arch_t type, arch_t index);
	void run_death(arch_t type);
	void push_from_symbol(sint_t id, const std::string& module_name, const std::string& symbol);
	void push_from_function(sint_t id, asIScriptFunction* function);
	void suspend();
	std::string verify(asIScriptFunction* imported) const;
private:
	static void print_message(const std::string& message);
	static void error_callback(const asSMessageInfo* msg, void_t aux);
	static void calls_callback(asIScriptContext* ctx, uint_t* calls);
	asIScriptFunction* find_from_index(const std::string& module_name, arch_t index) const;
	asIScriptFunction* find_from_symbol(const std::string& module_name, const std::string& symbol) const;
	asIScriptFunction* find_from_declaration(const std::string& module_name, const std::string& declaration) const;
	asIScriptFunction* find_from_declaration(const std::string& declaration) const;
	bool has_linked_functions(asIScriptModule* source_module, asIScriptModule* boot_module, asIScriptModule* current_module) const;
	void execute_function(asIScriptFunction* function);
	void execute_function(asIScriptFunction* function, std::vector<arch_t> args);
	void close_dependencies(kernel_t& kernel, const stack_gui_t& stack_gui, const inventory_gui_t& inventory_gui, dialogue_gui_t& dialogue_gui);
	void discard_all_events();
	void link_imported_functions(asIScriptModule* module);
	void set_stalled_period();
	void set_waiting_period(real_t seconds);
	void generate_properties();
	void generate_functions(input_t& input, audio_t& audio, music_t& music, kernel_t& kernel, stack_gui_t& stack_gui, dialogue_gui_t& dialogue_gui, headsup_gui_t& headsup_gui, camera_t& camera, naomi_state_t& naomi_state, kontext_t& kontext);
private:
	std::bitset<rec_bits_t::Total> bitmask;
	real_t timer;
	uint_t calls;
	asIScriptEngine* engine;
	asIScriptContext* state;
	asIScriptModule* current;
	asIScriptFunction* boot;
	std::unordered_map<sint_t, asIScriptFunction*> events;
};
