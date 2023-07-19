add_library(fatfs_spi INTERFACE)
target_sources(fatfs_spi INTERFACE
        ff15/source/ffsystem.c
        ff15/source/ffunicode.c
        ff15/source/ff.c
        sd_driver/sd_spi.c
        sd_driver/demo_logging.c
        sd_driver/spi.c
        sd_driver/sd_card.c
        sd_driver/crc.c
        src/glue.c
        src/f_util.c
        src/ff_stdio.c
        src/my_debug.c
        src/rtc.c
)
target_include_directories(fatfs_spi INTERFACE
        ff15/source
        sd_driver
        include
)
target_link_libraries(fatfs_spi INTERFACE
        hardware_spi
        hardware_dma
        hardware_rtc
        pico_stdlib
)
