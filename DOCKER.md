# Docker и DevContainer для FSMConfig

## Введение

Проект FSMConfig интегрирован с Docker для обеспечения изолированной и воспроизводимой среды разработки и непрерывной интеграции. Docker позволяет:

- **Единая среда разработки**: Все разработчики используют одинаковые версии компиляторов, библиотек и инструментов
- **Воспроизводимость сборок**: CI/CD пайплайны используют те же образы, что и локальная разработка
- **Быстрое начало работы**: Новые разработчики могут начать работу без установки зависимостей
- **Изоляция**: Проектные зависимости не влияют на систему хоста

Проект использует два разных подхода:
- **DevContainer** - для интерактивной разработки в VSCode
- **Docker для CI** - оптимизированный образ для автоматизированной сборки и тестирования

---

## DevContainer (Разработка)

### Что такое DevContainer

DevContainer (Development Container) - это функция VSCode, которая позволяет разрабатывать код внутри Docker контейнера. Это обеспечивает:

- Полностью настроенную среду разработки "из коробки"
- Автоматическую установку необходимых расширений VSCode
- Изоляцию проектных зависимостей от системы хоста
- Единые настройки для всей команды разработчиков

### Как открыть проект в DevContainer (VSCode)

**Требования:**
- Установленный [Docker Desktop](https://www.docker.com/products/docker-desktop) или Docker Engine
- Установленный [Visual Studio Code](https://code.visualstudio.com/)
- Установленное расширение [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)

**Шаги:**

1. Откройте проект FSMConfig в VSCode
2. Нажмите `F1` (или `Ctrl+Shift+P`) для открытия палитры команд
3. Выберите `Dev Containers: Reopen in Container`
4. Дождитесь сборки образа и запуска контейнера (первый запуск займет несколько минут)

При первом запуске VSCode автоматически:
- Соберёт Docker образ на основе [`.devcontainer/Dockerfile`](.devcontainer/Dockerfile)
- Установит указанные расширения
- Выполнит команду из `postCreateCommand`

### Какие расширения установлены автоматически

В DevContainer автоматически устанавливаются следующие расширения:

| Расширение | Описание |
|------------|----------|
| `ms-vscode.cpptools` | Официальный пакет инструментов C/C++ для VSCode (IntelliSense, отладка) |
| `ms-vscode.cmake-tools` | Инструменты для работы с CMake (сборка, отладка, запуск тестов) |
| `twxs.cmake` | Подсветка синтаксиса CMake файлов |
| `xaver.clang-format` | Форматирование кода с помощью clang-format |
| `cheshirekow.cmake-format` | Форматирование CMake файлов |

### Какие инструменты доступны внутри контейнера

DevContainer включает следующие инструменты разработки:

**Компиляторы и система сборки:**
- GCC 14 (компилятор по умолчанию)
- G++ 14
- CMake (последняя версия из репозиториев Ubuntu)

**Библиотеки:**
- libyaml-cpp-dev (для работы с YAML конфигурациями)
- libgtest-dev (Google Test для написания тестов)

**Утилиты разработки:**
- pkg-config (для поиска библиотек)
- git (система контроля версий)
- vim (текстовый редактор)
- curl, wget (утилиты для загрузки файлов)

**Инструменты отладки и анализа:**
- gdb (отладчик)
- valgrind (анализатор памяти)
- clang-format (форматирование кода)
- clang-tidy (статический анализ)

**Дополнительно:**
- Zsh с Oh My Zsh (улучшенная оболочка)
- Общие утилиты из devcontainers/features/common-utils

### Как собрать проект внутри DevContainer

После открытия проекта в DevContainer выполните следующие команды:

**1. Конфигурация проекта (выполняется автоматически при первом запуске):**
```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

**2. Сборка проекта:**
```bash
cd build
cmake --build . -j$(nproc)
```

Или используйте панель CMake Tools в VSCode:
- Откройте панель CMake (Ctrl+Shift+P → CMake: Configure)
- Нажмите "Build All"

**3. Сборка в режиме Release:**
```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

### Как запустить тесты внутри DevContainer

**Запуск всех тестов:**
```bash
cd build
ctest --output-on-failure
```

**Запуск конкретного теста:**
```bash
cd build
ctest -R test_name --output-on-failure
```

**Запуск тестов с подробным выводом:**
```bash
cd build
ctest --verbose
```

**Запуск теста напрямую:**
```bash
cd build/tests
./test_state_machine
```

---

## Docker для CI (Continuous Integration)

### Описание Dockerfile для CI

[`Dockerfile`](Dockerfile) в корне проекта оптимизирован для CI/CD пайплайнов:

**Особенности:**
- Минимальный размер образа (только необходимые зависимости)
- Ubuntu 24.04 в качестве базового образа
- GCC 14 как компилятор по умолчанию
- C++26 стандарт
- Режим сборки Release по умолчанию
- Отсутствие пользователя vscode (запуск от root для CI)

**Установленные пакеты:**
- build-essential (gcc, g++, make)
- cmake
- libyaml-cpp-dev
- libgtest-dev
- pkg-config
- git

### Как собрать Docker образ для CI

**Сборка образа локально:**
```bash
docker build -t fsmconfig:ci .
```

**Сборка с использованием кэша:**
```bash
docker build --cache-from fsmconfig:ci -t fsmconfig:ci .
```

**Сборка без кэша (полная пересборка):**
```bash
docker build --no-cache -t fsmconfig:ci .
```

### Как запустить тесты в Docker контейнере

**Запуск тестов в режиме Debug:**
```bash
docker run --rm fsmconfig:ci bash -c "
  mkdir build && \
  cd build && \
  cmake -DCMAKE_BUILD_TYPE=Debug .. && \
  cmake --build . -j\$(nproc) && \
  ctest --output-on-failure
"
```

**Запуск тестов в режиме Release:**
```bash
docker run --rm fsmconfig:ci bash -c "
  mkdir build && \
  cd build && \
  cmake -DCMAKE_BUILD_TYPE=Release .. && \
  cmake --build . -j\$(nproc) && \
  ctest --output-on-failure
"
```

**Запуск с монтированием исходного кода (для быстрой итерации):**
```bash
docker run --rm -v $(pwd):/app -w /app fsmconfig:ci bash -c "
  mkdir -p build && \
  cd build && \
  cmake .. && \
  cmake --build . -j\$(nproc) && \
  ctest --output-on-failure
"
```

### Примеры команд

**Проверка версии компилятора:**
```bash
docker run --rm fsmconfig:ci bash -c "g++ --version"
```

**Проверка версии CMake:**
```bash
docker run --rm fsmconfig:ci bash -c "cmake --version"
```

**Проверка информации о системе:**
```bash
docker run --rm fsmconfig:ci bash -c "uname -a && cat /etc/os-release"
```

**Интерактивный запуск контейнера:**
```bash
docker run --rm -it fsmconfig:ci bash
```

---

## GitHub Actions CI/CD

### Описание workflow

CI пайплайн описан в [`.github/workflows/ci.yml`](.github/workflows/ci.yml) и выполняет следующие задачи:

1. **Checkout кода**: Получает исходный код из репозитория
2. **Настройка Docker Buildx**: Подготавливает расширенный билдер Docker
3. **Сборка Docker образа**: Создаёт образ `fsmconfig:ci` с кэшированием
4. **Проверка версий**: Выводит версии компилятора, CMake и информацию о системе
5. **Сборка и тестирование**: Компилирует проект и запускает тесты
6. **Загрузка результатов**: Сохраняет результаты тестов как артефакты

### Какие проверки выполняются

**Матрица сборки:**
- Debug конфигурация
- Release конфигурация

**Проверки:**
- Сборка проекта с CMake
- Запуск всех тестов через CTest
- Проверка на ошибки компиляции
- Проверка на ошибки линковки
- Проверка прохождения всех тестов

### Как просмотреть результаты CI

1. Откройте репозиторий на GitHub
2. Перейдите во вкладку **Actions**
3. Выберите нужный workflow run
4. Просмотрите детали каждого шага
5. Скачайте артефакты с результатами тестов (если есть)

Артефакты доступны для скачивания в течение 90 дней.

### Триггеры для запуска CI

CI автоматически запускается при следующих событиях:

**Push события:**
- Push в ветку `main`
- Push в ветку `master`
- Push в ветку `develop`

**Pull Request события:**
- Pull Request в ветку `main`
- Pull Request в ветку `master`
- Pull Request в ветку `develop`

---

## Локальное использование Docker

### Как собрать Docker образ локально

**Сборка образа для разработки (DevContainer):**
```bash
cd .devcontainer
docker build -t fsmconfig:dev .
```

**Сборка образа для CI:**
```bash
docker build -t fsmconfig:ci .
```

**Сборка с тегом версии:**
```bash
docker build -t fsmconfig:1.0.0 .
```

### Как запустить контейнер для разработки

**Запуск с монтированием исходного кода:**
```bash
docker run --rm -it \
  -v $(pwd):/workspace \
  -w /workspace \
  fsmconfig:dev \
  bash
```

**Запуск с доступом к отладчику:**
```bash
docker run --rm -it \
  -v $(pwd):/workspace \
  -w /workspace \
  --cap-add=SYS_PTRACE \
  --security-opt seccomp=unconfined \
  fsmconfig:dev \
  bash
```

**Запуск с сохранением изменений в образе:**
```bash
docker run -it \
  -v $(pwd):/workspace \
  -w /workspace \
  --name fsmconfig-dev \
  fsmconfig:dev \
  bash
```

### Как смонтировать исходный код

**Монтирование текущей директории:**
```bash
docker run --rm -v $(pwd):/workspace -w /workspace fsmconfig:dev bash
```

**Монтирование с правами доступа пользователя:**
```bash
docker run --rm \
  -v $(pwd):/workspace \
  -w /workspace \
  -u $(id -u):$(id -g) \
  fsmconfig:dev \
  bash
```

**Монтирование нескольких директорий:**
```bash
docker run --rm \
  -v $(pwd):/workspace \
  -v $(pwd)/build:/workspace/build \
  -w /workspace \
  fsmconfig:dev \
  bash
```

### Примеры команд

**Сборка проекта в контейнере:**
```bash
docker run --rm \
  -v $(pwd):/workspace \
  -w /workspace \
  fsmconfig:ci \
  bash -c "mkdir -p build && cd build && cmake .. && cmake --build . -j\$(nproc)"
```

**Запуск тестов в контейнере:**
```bash
docker run --rm \
  -v $(pwd):/workspace \
  -w /workspace/build \
  fsmconfig:ci \
  ctest --output-on-failure
```

**Запуск примера:**
```bash
docker run --rm \
  -v $(pwd):/workspace \
  -w /workspace/build/examples/simple_fsm \
  fsmconfig:ci \
  ./simple_fsm
```

---

## Устранение неполадок

### Частые проблемы и их решения

**Проблема: Контейнер не собирается из-за отсутствия прав доступа**

**Решение:**
```bash
sudo usermod -aG docker $USER
newgrp docker
```

**Проблема: Ошибка "Permission denied" при работе с файлами в смонтированной директории**

**Решение 1:** Запуск контейнера с UID текущего пользователя:
```bash
docker run --rm -v $(pwd):/workspace -u $(id -u):$(id -g) fsmconfig:dev bash
```

**Решение 2:** Изменение прав доступа к файлам:
```bash
sudo chown -R $(id -u):$(id -g) .
```

**Проблема: DevContainer не запускается в VSCode**

**Решения:**
1. Убедитесь, что Docker Desktop запущен
2. Проверьте наличие расширения "Dev Containers"
3. Откройте логи DevContainer (F1 → Dev Containers: Show Container Log)
4. Попробуйте пересобрать контейнер (F1 → Dev Containers: Rebuild Container)

**Проблема: Тесты падают внутри контейнера, но работают локально**

**Решение:**
1. Проверьте, что все зависимости установлены в образе
2. Убедитесь, что версии компиляторов совпадают
3. Проверьте переменные окружения (CXXFLAGS, CMAKE_BUILD_TYPE)
4. Запустите тесты с подробным выводом: `ctest --verbose`

**Проблема: Ошибка памяти при запуске Valgrind**

**Решение:**
```bash
docker run --rm -v $(pwd):/workspace -w /workspace \
  --cap-add=SYS_PTRACE \
  --security-opt seccomp=unconfined \
  fsmconfig:dev \
  valgrind --leak-check=full ./build/tests/test_state_machine
```

### Как пересобрать Docker образ

**Полная пересборка без кэша:**
```bash
docker build --no-cache -t fsmconfig:ci .
```

**Пересборка с очисткой старых слоёв:**
```bash
docker build --no-cache --pull -t fsmconfig:ci .
```

**Пересборка DevContainer:**
```bash
cd .devcontainer
docker build --no-cache -t fsmconfig:dev .
```

### Как очистить кэш Docker

**Удаление всех остановленных контейнеров:**
```bash
docker container prune
```

**Удаление неиспользуемых образов:**
```bash
docker image prune
```

**Удаление всех неиспользуемых ресурсов:**
```bash
docker system prune -a
```

**Удаление конкретного образа:**
```bash
docker rmi fsmconfig:ci
```

**Принудительное удаление образа:**
```bash
docker rmi -f fsmconfig:ci
```

**Очистка BuildKit кэша:**
```bash
docker builder prune -a
```

---

## Архитектура Docker интеграции

### Различия между devcontainer и CI Dockerfile

| Характеристика | DevContainer Dockerfile | CI Dockerfile |
|----------------|------------------------|---------------|
| **Расположение** | [`.devcontainer/Dockerfile`](.devcontainer/Dockerfile) | [`Dockerfile`](Dockerfile) (корень проекта) |
| **Цель** | Интерактивная разработка | Автоматизированная CI/CD |
| **Размер образа** | Больший (~500-700 MB) | Минимальный (~200-300 MB) |
| **Пользователь** | `vscode` (UID 1000) | root |
| **Инструменты** | Полный набор (gdb, valgrind, clang-format, vim, zsh) | Минимальный набор (только для сборки) |
| **Расширения** | Автоматическая установка в VSCode | Не применимо |
| **Права sudo** | Да (без пароля) | Не требуется |
| **Режим сборки** | Debug по умолчанию | Release по умолчанию |
| **Shell** | Zsh с Oh My Zsh | Bash |
| **Монтирование** | Автоматическое через VSCode | Ручное через `-v` |

### Почему используются два разных Dockerfile

**DevContainer Dockerfile** оптимизирован для:
- Удобства разработки с полным набором инструментов
- Интерактивной отладки и профилирования
- Форматирования и статического анализа кода
- Комфортной работы в терминале (Zsh, Oh My Zsh)
- Интеграции с VSCode (IntelliSense, CMake Tools)

**CI Dockerfile** оптимизирован для:
- Минимального размера образа (быстрая загрузка в CI)
- Быстрой сборки (меньше слоёв, меньше зависимостей)
- Воспроизводимости результатов сборки
- Минимального времени выполнения CI пайплайна
- Экономии ресурсов GitHub Actions

Разделение на два Dockerfile позволяет:
- Использовать разные оптимизации для разных сценариев
- Не устанавливать лишние инструменты в CI
- Предоставлять разработчикам максимум удобств
- Сокращать время выполнения CI пайплайнов
- Упростить поддержку и обновление образов

---

## Ссылки

### Официальная документация Docker

- [Docker Documentation](https://docs.docker.com/)
- [Dockerfile Reference](https://docs.docker.com/engine/reference/builder/)
- [Docker Best Practices](https://docs.docker.com/develop/dev-best-practices/)
- [Docker BuildKit](https://docs.docker.com/build/buildkit/)

### Документация VSCode DevContainers

- [Visual Studio Code Dev Containers](https://code.visualstudio.com/docs/devcontainers/containers)
- [Dev Containers Reference](https://code.visualstudio.com/docs/devcontainers/reference)
- [Creating a Dev Container](https://code.visualstudio.com/docs/devcontainers/create-dev-container)
- [Dev Container Features](https://code.visualstudio.com/docs/devcontainers/features)

### Документация GitHub Actions

- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [Workflow Syntax](https://docs.github.com/en/actions/using-workflows/workflow-syntax-for-github-actions)
- [Building and testing code](https://docs.github.com/en/actions/automating-builds-and-tests/building-and-testing-code)
- [Docker Build Push Action](https://github.com/docker/build-push-action)

### Дополнительные ресурсы

- [CMake Documentation](https://cmake.org/documentation/)
- [Google Test Documentation](https://google.github.io/googletest/)
- [libyaml-cpp](https://github.com/jbeder/yaml-cpp)
- [GCC Documentation](https://gcc.gnu.org/onlinedocs/)

---

## Краткая справка команд

### DevContainer
```bash
# Открыть проект в DevContainer
F1 → Dev Containers: Reopen in Container

# Пересобрать контейнер
F1 → Dev Containers: Rebuild Container

# Показать логи контейнера
F1 → Dev Containers: Show Container Log
```

### Docker
```bash
# Сборка образа
docker build -t fsmconfig:ci .

# Запуск контейнера
docker run --rm -v $(pwd):/workspace -w /workspace fsmconfig:ci bash

# Просмотр образов
docker images

# Удаление образа
docker rmi fsmconfig:ci

# Очистка системы
docker system prune -a
```

### CMake
```bash
# Конфигурация проекта
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

# Сборка проекта
cmake --build build -j$(nproc)

# Запуск тестов
cd build && ctest --output-on-failure
```

---

**Документация обновлена:** 2025-01-20
**Версия:** 1.0.0
