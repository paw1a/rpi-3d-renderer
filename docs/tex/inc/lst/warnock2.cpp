    if (window_width == 1 && window_height == 1) {
      if (visible.size == 0) {
        set_pixel(display, current_window.begin, bg_color);
      } else {
        fill_pixel(display, current_window.begin, visible, set_pixel);
      }
    } else if (surrounding_cursor != disjoint_cursor) {
      split_window(stack, current_window, visible);
    } else {
      if (visible.size == 0) {
        fill_window(display, current_window, bg_color, set_pixel);
        continue;
      }
      std::pair<bool, polygon> result =
        find_cover_polygon(current_window, visible);
      if (result.first) {
        fill_window(display, current_window, result.second.color, set_pixel);
      } else {
        split_window(stack, current_window, visible);
      }
    }
  }
}