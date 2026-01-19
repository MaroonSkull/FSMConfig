# API Reference

## Пространство имен `fsmconfig`

Все классы и типы библиотеки находятся в пространстве имен `fsmconfig`.

## Типы

### `StateId`
```cpp
using StateId = std::string;
```
Тип для идентификации состояния.

### `EventId`
```cpp
using EventId = std::string;
```
Тип для идентификации события.

### `TransitionId`
```cpp
using TransitionId = std::string;
```
Тип для идентификации перехода.

### `CallbackId`
```cpp
using CallbackId = std::string;
```
Тип для идентификации коллбэка.

### `VariableId`
```cpp
using VariableId = std::string;
```
Тип для идентификации переменной.

### `VariableValue`
```cpp
using VariableValue = std::variant<bool, int, double, std::string>;
```
Тип значения переменной (поддержка различных типов).

### `ActionCallback`
```cpp
using ActionCallback = std::function<void()>;
```
Тип коллбэка для действий.

### `ConditionCallback`
```cpp
using ConditionCallback = std::function<bool()>;
```
Тип коллбэка для условий.

### `EventCallback`
```cpp
using EventCallback = std::function<void(const EventId&)>;
```
Тип коллбэка для обработки событий.

## Классы

### `StateMachine`

Основной класс конечного автомата.

#### Методы

```cpp
StateMachine();
~StateMachine();
StateMachine(StateMachine&&) noexcept;
StateMachine& operator=(StateMachine&&) noexcept;

bool load_config(const std::string& config_path);
bool start();
void stop();
bool send_event(const EventId& event);
StateId get_current_state() const;
bool is_running() const;
```

### `ConfigParser`

Класс для парсинга YAML конфигураций.

#### Методы

```cpp
ConfigParser();
~ConfigParser();
ConfigParser(ConfigParser&&) noexcept;
ConfigParser& operator=(ConfigParser&&) noexcept;

MachineConfig parse_file(const std::string& config_path);
MachineConfig parse_string(const std::string& yaml_content);
bool validate(const MachineConfig& config) const;
```

### `CallbackRegistry`

Класс для регистрации и выполнения коллбэков.

#### Методы

```cpp
CallbackRegistry();
~CallbackRegistry();
CallbackRegistry(CallbackRegistry&&) noexcept;
CallbackRegistry& operator=(CallbackRegistry&&) noexcept;

bool register_action(const CallbackId& id, ActionCallback callback);
bool register_condition(const CallbackId& id, ConditionCallback callback);
bool register_event_handler(const CallbackId& id, EventCallback callback);
bool execute_action(const CallbackId& id) const;
bool execute_condition(const CallbackId& id) const;
bool execute_event_handler(const CallbackId& id, const EventId& event) const;
bool unregister(const CallbackId& id);
bool has_callback(const CallbackId& id) const;
void clear();
```

### `EventDispatcher`

Класс для диспетчеризации событий.

#### Методы

```cpp
EventDispatcher();
~EventDispatcher();
EventDispatcher(EventDispatcher&&) noexcept;
EventDispatcher& operator=(EventDispatcher&&) noexcept;

bool register_handler(const EventId& event, EventHandler handler);
bool dispatch(const EventId& event);
size_t process_queue();
bool has_pending_events() const;
void clear_queue();
bool unregister_handler(const EventId& event);
bool has_handler(const EventId& event) const;
```

### `State`

Класс состояния конечного автомата.

#### Методы

```cpp
State(const StateId& id, const std::string& description = "");
~State();
State(State&&) noexcept;
State& operator=(State&&) noexcept;

StateId get_id() const;
std::string get_description() const;
void set_description(const std::string& description);
void set_on_enter(ActionCallback callback);
void set_on_exit(ActionCallback callback);
bool enter();
bool exit();
bool is_active() const;
void set_active(bool active);
```

### `VariableManager`

Менеджер переменных для конечного автомата.

#### Методы

```cpp
VariableManager();
~VariableManager();
VariableManager(VariableManager&&) noexcept;
VariableManager& operator=(VariableManager&&) noexcept;

bool set(const VariableId& id, bool value);
bool set(const VariableId& id, int value);
bool set(const VariableId& id, double value);
bool set(const VariableId& id, const std::string& value);

std::optional<VariableValue> get(const VariableId& id) const;
bool get_bool(const VariableId& id, bool default_value = false) const;
int get_int(const VariableId& id, int default_value = 0) const;
double get_double(const VariableId& id, double default_value = 0.0) const;
std::string get_string(const VariableId& id, const std::string& default_value = "") const;

bool has(const VariableId& id) const;
bool remove(const VariableId& id);
void clear();
size_t size() const;
bool empty() const;
```

## Структуры

### `Transition`
```cpp
struct Transition {
    TransitionId id;
    StateId from_state;
    StateId to_state;
    EventId trigger_event;
    ConditionCallback condition;
    ActionCallback action;
};
```

### `StateDefinition`
```cpp
struct StateDefinition {
    StateId id;
    std::string description;
    std::vector<TransitionId> transitions;
    ActionCallback on_enter;
    ActionCallback on_exit;
};
```

### `MachineConfig`
```cpp
struct MachineConfig {
    std::string name;
    StateId initial_state;
    std::unordered_map<StateId, StateDefinition> states;
    std::unordered_map<TransitionId, Transition> transitions;
};
```
