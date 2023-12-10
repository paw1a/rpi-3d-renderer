for (size_t i = 0; i < 6; i++) {
    for (size_t j = 0; j < 6; j++) {
        window = windows[j][i];
        warnock_render(&displays[j], window, BLACK, set_pixel);
        GFX_flush_block(&displays[j], window.begin.x + displays[j].width / 2,
                        window.begin.y + displays[j].height / 2,
                        displays[j].width / 3, displays[j].height / 2);
    }
}