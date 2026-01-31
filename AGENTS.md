# AGENTS.md

## Оглавление

1. [Общая информация о проекте](#общая-информация-о-проекте)
2. [Общие указания по работе](#общие-указания-по-работе)
3. [Дополнительные ресурсы](#дополнительные-ресурсы)

---

## Общая информация о проекте

### Назначение

**FSMConfig** — это C++ библиотека для работы с конечными автоматами (Finite State Machine) через YAML-конфигурацию. Библиотека позволяет декларативно описывать состояния, переходы, события и коллбэки в YAML-файлах, что упрощает создание и модификацию сложных автоматов состояний без необходимости перекомпиляции кода.

### Основные компоненты

#### Заголовочные файлы (`include/fsmconfig/`)

| Файл | Назначение |
|------|------------|
| [`state_machine.hpp`](include/fsmconfig/state_machine.hpp) | Основной класс конечного автомата |
| [`state.hpp`](include/fsmconfig/state.hpp) | Класс представления состояния |
| [`config_parser.hpp`](include/fsmconfig/config_parser.hpp) | Парсер YAML-конфигураций |
| [`event_dispatcher.hpp`](include/fsmconfig/event_dispatcher.hpp) | Диспетчер событий |
| [`callback_registry.hpp`](include/fsmconfig/callback_registry.hpp) | Реестр коллбэков |
| [`variable_manager.hpp`](include/fsmconfig/variable_manager.hpp) | Менеджер переменных |
| [`types.hpp`](include/fsmconfig/types.hpp) | Основные типы и определения |

#### Исходные файлы (`src/fsmconfig/`)

| Файл | Реализация |
|------|------------|
| [`state_machine.cpp`](src/fsmconfig/state_machine.cpp) | Логика работы конечного автомата |
| [`state.cpp`](src/fsmconfig/state.cpp) | Управление состояниями |
| [`config_parser.cpp`](src/fsmconfig/config_parser.cpp) | Парсинг YAML |
| [`event_dispatcher.cpp`](src/fsmconfig/event_dispatcher.cpp) | Обработка событий |
| [`callback_registry.cpp`](src/fsmconfig/callback_registry.cpp) | Регистрация и вызов коллбэков |
| [`variable_manager.cpp`](src/fsmconfig/variable_manager.cpp) | Управление переменными |
| [`types.cpp`](src/fsmconfig/types.cpp) | Вспомогательные типы |

#### Тесты (`tests/`)

- [`test_state_machine.cpp`](tests/test_state_machine.cpp) — тесты конечного автомата
- [`test_config_parser.cpp`](tests/test_config_parser.cpp) — тесты парсера конфигурации
- [`test_callback_registry.cpp`](tests/test_callback_registry.cpp) — тесты реестра коллбэков
- [`test_integration.cpp`](tests/test_integration.cpp) — интеграционные тесты

#### Примеры (`examples/`)

- [`simple_fsm/`](examples/simple_fsm/) — простой пример конечного автомата
- [`game_state/`](examples/game_state/) — пример управления состояниями игры
- [`network_protocol/`](examples/network_protocol/) — пример протокола сетевого взаимодействия

#### Дополнительные файлы конфигурации

| Файл/Директория | Назначение |
|-----------------|------------|
| [`cmake/Findyaml-cpp.cmake`](cmake/Findyaml-cpp.cmake) | Кастомный модуль поиска yaml-cpp |
| [`cmake/fsmconfig-config.cmake.in`](cmake/fsmconfig-config.cmake.in) | Шаблон конфигурации CMake |
| [`Dockerfile`](Dockerfile) | Docker-образ для разработки |
| [`.devcontainer/`](.devcontainer/) | Конфигурация VS Code Dev Container |
| [`.github/`](.github/) | Конфигурация GitHub Actions |
| [`.gitignore`](.gitignore) | Правила исключения Git |
| [`LICENSE`](LICENSE) | Лицензия проекта |

---

## Общие указания по работе

### ВАЖНО: Использование контекста

Для эффективной работы с проектом ВСЕГДА использовать:

1. **"use context7"** — для получения контекста о текущем состоянии проекта
2. **"Knowledge Graph Memory"** — для сохранения и извлечения знаний о проекте

### Правила создания .md файлов

**ОБЯЗАТЕЛЬНОЕ ПРАВИЛО:** Все `.md` файлы должны создаваться исключительно внутри директории `docs/`.

**Исключения:**
- [`README.md`](README.md) в корне проекта — основное описание проекта
- [`AGENTS.md`](AGENTS.md) в корне проекта — правила работы для агентов
- [`LICENSE`](LICENSE) в корне проекта — файл лицензии
- [`DOCKER.md`](DOCKER.md) в корне проекта — инструкции по Docker

**Структура директории `docs/`:**
- Можно создавать любые подпапки с любой структурой
- Примеры существующих путей:
  - [`docs/architecture.md`](docs/architecture.md)
  - [`docs/api_reference.md`](docs/api_reference.md)
  - [`docs/examples.md`](docs/examples.md)
  - `docs/architecture/decisions/` — для ADR (Architecture Decision Records)

**Важно:** Если пользователь явно не попросил создать .md файл в другой директории, ВСЕГДА создавайте .md файлы внутри `docs/`. Это обеспечивает централизованное хранение всей технической документации проекта.

### MCP Memory Server

**Назначение:** MCP Memory Server предоставляет LLM возможность сохранять и извлекать знания о проекте в виде knowledge graph.

**Конфигурация:** Сервер запускается через Docker с постоянным хранилищем `claude-memory:/app/dist`.

**Доступные инструменты:**
1. `create_entities` — создание сущностей (классов, файлов, библиотек)
2. `create_relations` — создание связей между сущностями
3. `add_observations` — добавление наблюдений к существующим сущностям
4. `read_graph` — чтение всего knowledge graph
5. `search_nodes` — поиск сущностей по имени, типу или содержимому
6. `open_nodes` — получение детальной информации о конкретных сущностях

**Рекомендуемый workflow:**

```
Начало задачи
    ↓
Проверить существование сущности (search_nodes)
    ↓
    ├─ Найдена → open_nodes → Анализ информации
    └─ Не найдена → create_entities
    ↓
Добавить новые наблюдения (add_observations)
    ↓
Создать отношения (create_relations)
```

**Когда использовать Memory Server:**

| Действие | Инструмент | Пример |
|----------|------------|-------|
| Первое знакомство с компонентом | `create_entities` | Создать сущность для нового класса |
| Обнаружение зависимости | `create_relations` | Связать класс с используемой библиотекой |
| Изучение деталей реализации | `add_observations` | Добавить информацию о методах |
| Получение контекста | `read_graph` или `search_nodes` | Извлечь знания перед задачей |
| Проверка существования | `search_nodes` | Проверить, создана ли сущность |

**Типы сущностей:**
- `project` — проект (FSMConfig)
- `class` — класс или структура (StateMachine, ConfigParser)
- `file` — файл исходного кода (state_machine.cpp)
- `library` — внешняя зависимость (yaml-cpp, gtest)
- `tool` — инструмент разработки (clang-tidy, cppcheck)
- `concept` — концепция или паттерн (RAII, Observer Pattern)

**Типы отношений:**
- `depends_on` — зависимость (A зависит от B)
- `uses` — использование (A использует B)
- `part_of` — часть (A является частью B)
- `implements` — реализация (A реализует B)
- `tests` — тестирование (A тестирует B)
- `extends` — наследование/расширение

**Пример использования:**

```python
# 1. Проверить существование
result = mcp__memory__search_nodes(query="StateMachine")

# 2. Создать если не существует
if not result["nodes"]:
    mcp__memory__create_entities(
        entities=[{
            "name": "StateMachine",
            "entityType": "class",
            "observations": [
                "Основной класс конечного автомата",
                "Расположен в include/fsmconfig/state_machine.hpp"
            ]
        }]
    )

# 3. Добавить наблюдения
mcp__memory__add_observations(
    observations=[{
        "entityName": "StateMachine",
        "contents": [
            "Метод processEvent() обрабатывает события асинхронно"
        ]
    }]
)

# 4. Создать отношения
mcp__memory__create_relations(
    relations=[
        {"from": "StateMachine", "to": "State", "relationType": "manages"}
    ]
)
```

**Важно:** Memory Server — это дополнительный инструмент для сохранения контекста между сессиями. Он НЕ заменяет анализ кода и чтение файлов, а дополняет их.

### Пайплайн выполнения высокоуровневых задач

```
1. Сделать маленькое действие
   ↓
2. Критически проанализировать результаты
   (используя режимы code-skeptic, code-reviewer)
   ↓
3. Если есть ошибки или недоработки:
   ├─ Подготовить вопрос пользователю для уточнения
   └─ Вернуться к пункту 1 и переработать решение
   ↓
4. Если ошибок нет — двигаться по задаче дальше
```

### Процесс решения высокоуровневой задачи

```
1. Создать архитектурные ADR
   └─ Расположение: docs/architecture/decisions/
   └─ **Важно:** Директория должна быть создана перед использованием
   ↓
2. Добавить диаграммы
   ├─ Диаграммы компонентов
   └─ Data flow диаграммы
   ↓
3. Документировать каждый критический path
   ↓
4. Провести аудит кода на основе документов
   ├─ Найти дублирование (DRY)
   ├─ Найти неправильную логику
   └─ Найти расхождения с архитектурным планом
```

**Пример структуры ADR:**

```markdown
# ADR-001: Выбор подхода для управления состоянием

## Статус
Предложено

## Контекст
Необходимо определить способ управления переходами между состояниями...

## Решение
Использовать паттерн State с делегированием управления...

## Последствия
Плюсы: ...
Минусы: ...
```

### Использование линтеров

**Принцип:** Использовать максимально широкий набор линтеров для обеспечения высокого качества кода.

**Обязательные проверки:**

```bash
# clang-tidy с современными проверками
clang-tidy src/**/*.cpp \
  --checks='*' \
  -warnings-as-errors='*' \
  -- -Iinclude/ -std=c++20

# cppcheck для дополнительного анализа
cppcheck --enable=all --inconclusive --std=c++20 src/

# clang-format для форматирования
clang-format -i src/**/*.cpp include/**/*.hpp
```

### Поиск библиотек

**Философия:** Не изобретать велосипед — искать готовые решения.

**Процесс:**

1. При необходимости новой функциональности — сначала поиск в интернете (tavily)
2. Цель: создать простой код, используя существующие библиотеки
3. Не бояться обилия зависимостей
4. При разработке НЕ реализовывать новые методы самостоятельно
5. Если найдена подходящая библиотека:
   - Предложить пользователю её интеграцию
   - Дождаться ответа
   - Только после одобрения — интегрировать

**Примеры поиска:**
- Для работы с JSON → `nlohmann/json`
- Для логирования → `spdlog`
- Для тестирования → `Catch2` или `gtest`
- Для работы с временем → `date` (Howard Hinnant)

---

## Дополнительные ресурсы

### Руководства и стандарты разработки

- **Технологический стек:** Подробнее: [`docs/technologies/tech-stack.md`](docs/technologies/tech-stack.md)
- **Стратегия разбиения задач:** Подробнее: [`docs/development/task-strategy.md`](docs/development/task-strategy.md)
- **Стандарты кодирования:** Подробнее: [`docs/development/coding-standards.md`](docs/development/coding-standards.md)
- **Git workflow:** Подробнее: [`docs/development/workflow.md`](docs/development/workflow.md)
- **Критический подход к разработке:** Подробнее: [`docs/development/critical-approach.md`](docs/development/critical-approach.md)

### Внутренняя документация

- [`README.md`](README.md) — общее описание проекта
- [`DOCKER.md`](DOCKER.md) — инструкции по работе с Docker
- [`docs/architecture.md`](docs/architecture.md) — архитектура проекта
- [`docs/api_reference.md`](docs/api_reference.md) — справочник по API
- [`docs/examples.md`](docs/examples.md) — примеры использования

### Внешние ресурсы

- [CMake Documentation](https://cmake.org/documentation/)
- [yaml-cpp Documentation](https://github.com/jbeder/yaml-cpp/)
- [Google Test Documentation](https://google.github.io/googletest/)
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/)

---

## Краткая шпаргалка

### Быстрый старт

```bash
# Сборка проекта
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

# Запуск тестов
ctest --output-on-failure

# Запуск линтеров
clang-tidy src/**/*.cpp -- -Iinclude/ -std=c++20
clang-format -i src/**/*.cpp include/**/*.hpp
```

### Типичный workflow

```bash
# 1. Создать ветку
git checkout -b feature/my-feature

# 2. Внести изменения
# ... код ...

# 3. Проверить линтерами
clang-tidy src/**/*.cpp -- -Iinclude/ -std=c++20

# 4. Запустить тесты
cd build && ctest --output-on-failure

# 5. Закоммитить
git add .
git commit -m "feat: add my feature"

# 6. Повторить для подзадач
# ...

# 7. Review
git diff main..feature/my-feature

# 8. Смерджить подветки
git merge feature/sub-task-1
```

---
