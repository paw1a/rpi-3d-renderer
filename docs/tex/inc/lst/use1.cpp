for (size_t i = 0; i < 6; i++) {
    warnock_render(&displays[i],
       {{-displays[i].width / 2, -displays[i].height / 2},
        {displays[i].width / 2, displays[i].height / 2},
        state.polygons},
       BLACK, set_pixel);
    GFX_flush(&displays[i]);
}
