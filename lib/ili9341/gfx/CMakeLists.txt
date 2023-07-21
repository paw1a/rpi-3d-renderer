add_library(gfx
		gfx.c
)

target_include_directories(gfx PUBLIC
        .
)

target_link_libraries(gfx ili9341 pico_stdlib)