add_library(plotter STATIC src/plotter.c src/cJSON.c)
target_include_directories(plotter PUBLIC include)
target_link_libraries(plotter pico_stdlib)