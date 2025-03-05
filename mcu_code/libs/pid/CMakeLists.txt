cmake_minimum_required(VERSION 3.12)
project(pid)

# Crear biblioteca estática
add_library(pid STATIC
    src/pid.c
)

# Incluir directorios para la biblioteca
target_include_directories(pid PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Linkear con el Pico SDK si es necesario
target_link_libraries(pid PUBLIC pico_stdlib)