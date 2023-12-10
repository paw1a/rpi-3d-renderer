void warnock_render(display_t *display, const window &full_window, 
	const uint16_t bg_color,
  void set_pixel(display_t *, point2, uint16_t)) {
  std::stack<window> stack;
  stack.push(full_window);
  while (!stack.empty()) {
    window current_window = stack.top();
    stack.pop();
    size_t index = 0;
    size_t disjoint_cursor = 0;
    size_t surrounding_cursor = current_window.polygons.size;
    while (index < surrounding_cursor) {
      polygon polygon = current_window.polygons.data[index];
      relationship rel = check_relationship(polygon, current_window);
      if (rel == relationship::disjoint) {
        std::swap(current_window.polygons.data[index++],
             current_window.polygons.data[disjoint_cursor++]);
      } else if (rel == relationship::surrounding) {
        std::swap(current_window.polygons.data[index],
             current_window.polygons.data[--surrounding_cursor]);
      } else {
        ++index;
      }
    }
    array<polygon> visible = {current_window.polygons.data + disjoint_cursor,
                 current_window.polygons.size - disjoint_cursor};
    uint16_t window_width = current_window.end.x - current_window.begin.x;
    uint16_t window_height = current_window.end.y - current_window.begin.y;