# Tech Stack

## C++ стандарт и особенности

- **Стандарт:** C++20
- **Компилятор:** GCC/Clang с поддержкой C++20
- **Особенности:**
  - Использование RAII для управления ресурсами
  - Smart pointers (`std::unique_ptr`, `std::shared_ptr`)
  - STL контейнеры и алгоритмы
  - Lambda-выражения для коллбэков
  - `std::function` и `std::bind` для обратных вызовов

## CMake конфигурация

- **Минимальная версия:** CMake 3.15+
- **Система сборки:** Современный CMake с использованием target-based подхода
- **Основные цели:**
  - `fsmconfig` — основная библиотека
  - `fsmconfig_tests` — тесты
  - `simple_fsm`, `game_state`, `network_protocol` — примеры использования

**Пример сборки:**

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## yaml-cpp

- **Версия:** 0.7.0 или выше
- **Назначение:** Парсинг YAML-конфигураций
- **Интеграция:** Через CMake с тремя fallback-методами:
  1. `find_package(yaml-cpp CONFIG)` для системных пакетов
  2. `pkg_check_modules(yaml-cpp)` через PkgConfig (для Arch Linux)
  3. Кастомный модуль [`cmake/Findyaml-cpp.cmake`](../cmake/Findyaml-cpp.cmake) как последний вариант

**Пример конфигурации:**

```yaml
# Глобальные переменные
variables:
  max_attempts: 3

# Определения состояний
states:
  idle:
    on_enter: on_idle_enter
    on_exit: on_idle_exit
    actions:
      - log_idle_state
  
  active:
    on_enter: on_active_enter
    on_exit: on_active_exit
    actions:
      - log_active_state

# Определения переходов
transitions:
  - from: idle
    to: active
    event: start
    on_transition: on_start_transition
  
  - from: active
    to: idle
    event: stop
    on_transition: on_stop_transition
```

## Система тестирования

- **Фреймворк:** Google Test (gtest)
- **Запуск тестов:**
  ```bash
  cd build
  ctest --output-on-failure
  ```
- **Покрытие:** Юнит-тесты для каждого компонента + интеграционные тесты

## Линтеры и инструменты качества кода

| Инструмент | Назначение |
|------------|------------|
| **clangd** | Language Server Protocol для C++ |
| **clang-tidy** | Статический анализ кода |
| **clang-format** | Форматирование кода |
| **cppcheck** | Дополнительный статический анализ |
| **include-what-you-use** | Анализ включаемых заголовков |

**Рекомендуемые проверки clang-tidy:**

```bash
clang-tidy src/**/*.cpp -- -Iinclude/ -std=c++20
```

## Конфигурационные файлы линтеров

В проекте созданы конфигурационные файлы линтеров в корневой директории проекта:

### .clang-tidy
```yaml
Checks: >
  -*,
  bugprone-*,
  -bugprone-easily-swappable-parameters,
  cert-*,
  clang-analyzer-*,
  cppcoreguidelines-*,
  -cppcoreguidelines-avoid-magic-numbers,
  -cppcoreguidelines-pro-bounds-pointer-arithmetic,
  -cppcoreguidelines-pro-type-reinterpret-cast,
  modernize-*,
  -modernize-use-trailing-return-type,
  performance-*,
  readability-*,
  -readability-magic-numbers,
  -readability-identifier-length
WarningsAsErrors: ''
HeaderFilterRegex: '.*'
FormatStyle: file
```

### .clang-format
```yaml
BasedOnStyle: Google
IndentWidth: 4
ColumnLimit: 100
```

### .cppcheck
```ini
--enable=all
--inconclusive
--std=c++20
-Iinclude/
```

## CI/CD для линтеров

В проекте настроен автоматический запуск линтеров через GitHub Actions:
- Файл: [`.github/workflows/linters.yml`](../.github/workflows/linters.yml)
- Триггеры: push и pull request в ветки `main` и `develop`
- Выполняемые проверки:
  - `clang-tidy-check` — статический анализ кода
  - `clang-format-check` — проверка форматирования
  - `cppcheck-check` — дополнительный статический анализ
