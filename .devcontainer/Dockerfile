# Базовый образ Ubuntu 24.04 с поддержкой GCC 14
FROM ubuntu:24.04

# Метаданные образа
LABEL description="Devcontainer для FSMConfig C++ проекта (C++26)"
LABEL version="1.0.0"
LABEL maintainer="FSMConfig Team"

# Отключаем интерактивные запросы при установке пакетов
ARG DEBIAN_FRONTEND=noninteractive

# Обновляем список пакетов и устанавливаем базовые зависимости в один слой для оптимизации кэширования
RUN apt-get update && apt-get install -y \
    # Компиляторы и инструменты сборки
    build-essential \
    gcc-14 \
    g++-14 \
    # Система сборки
    cmake \
    # Зависимости проекта
    libyaml-cpp-dev \
    libgtest-dev \
    # Утилиты для разработки
    pkg-config \
    git \
    vim \
    curl \
    wget \
    # Инструменты отладки и анализа
    gdb \
    valgrind \
    # Инструменты форматирования и статического анализа
    clang-format \
    clang-tidy \
    # Очистка кэша apt для уменьшения размера образа
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Настраиваем GCC 14 как компилятор по умолчанию
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 100 \
    && update-alternatives --set gcc /usr/bin/gcc-14 \
    && update-alternatives --set g++ /usr/bin/g++-14

# Создаём пользователя vscode с UID 1000
RUN groupadd -g 1000 vscode \
    && useradd -u 1000 -g vscode -m -s /bin/bash vscode

# Добавляем пользователя в sudoers без пароля
RUN echo "vscode ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/vscode \
    && chmod 0440 /etc/sudoers.d/vscode

# Создаём рабочую директорию и настраиваем права
RUN mkdir -p /workspace \
    && chown -R vscode:vscode /workspace

# Переключаемся на пользователя vscode
USER vscode

# Устанавливаем рабочую директорию
WORKDIR /workspace

# Устанавливаем переменную окружения для C++26
ENV CXXFLAGS="-std=c++26"

# Команда по умолчанию при запуске контейнера
CMD ["/bin/bash"]
