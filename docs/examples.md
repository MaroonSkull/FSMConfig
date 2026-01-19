# Примеры использования FSMConfig

## Обзор

В этом документе описаны примеры использования библиотеки FSMConfig.

## Примеры

### 1. Simple FSM

Простой пример демонстрирует базовое использование библиотеки для создания конечного автомата с двумя состояниями.

**Файлы:**
- [`examples/simple_fsm/main.cpp`](../examples/simple_fsm/main.cpp)
- [`examples/simple_fsm/config.yaml`](../examples/simple_fsm/config.yaml)

**Конфигурация:**
- Состояния: `idle`, `active`
- Переходы: `idle_to_active`, `active_to_idle`

### 2. Game State

Пример демонстрирует использование библиотеки для управления состояниями игры.

**Файлы:**
- [`examples/game_state/main.cpp`](../examples/game_state/main.cpp)
- [`examples/game_state/config.yaml`](../examples/game_state/config.yaml)

**Конфигурация:**
- Состояния: `menu`, `playing`, `paused`, `game_over`
- Переходы: между всеми состояниями в зависимости от событий

### 3. Network Protocol

Пример демонстрирует использование библиотеки для моделирования состояний сетевого протокола.

**Файлы:**
- [`examples/network_protocol/main.cpp`](../examples/network_protocol/main.cpp)
- [`examples/network_protocol/config.yaml`](../examples/network_protocol/config.yaml)

**Конфигурация:**
- Состояния: `disconnected`, `connecting`, `connected`, `error`
- Переходы: между всеми состояниями в зависимости от событий

## Сборка примеров

```bash
mkdir build && cd build
cmake .. -DBUILD_EXAMPLES=ON
make
```

## Запуск примеров

```bash
# Simple FSM
./build/examples/simple_fsm

# Game State
./build/examples/game_state

# Network Protocol
./build/examples/network_protocol
```

## Создание собственного примера

1. Создайте YAML конфигурацию с состояниями и переходами
2. Создайте C++ файл с использованием [`StateMachine`](../include/fsmconfig/state_machine.hpp)
3. Загрузите конфигурацию через [`load_config()`](../include/fsmconfig/state_machine.hpp)
4. Зарегистрируйте коллбэки через [`CallbackRegistry`](../include/fsmconfig/callback_registry.hpp)
5. Запустите автомат через [`start()`](../include/fsmconfig/state_machine.hpp)
6. Отправляйте события через [`send_event()`](../include/fsmconfig/state_machine.hpp)
