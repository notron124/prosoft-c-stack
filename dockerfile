FROM ubuntu:22.04

# Скачиваем все зависимости
RUN apt-get update && apt-get install -y \
    cmake \
    libgtest-dev \
    libgmock-dev \
    build-essential \
    && rm -rf /var/lib/apt/lists/* # Уменьшает размер образа

# Создаем рабочую директорию
WORKDIR /workspace 

# Копируем файлы с хоста в рабочую директорию
COPY . .
