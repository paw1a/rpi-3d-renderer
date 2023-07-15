#include <cmath>
#include <limits>
#include <stack>
#include <vector>

#include "common.h"
#include "render.h"

enum class relationship {
    disjoint,
    contained,
    intersecting,
    surrounding
};

static inline float get_z(const polygon &polygon, const point2 &point) {
    return -(polygon.a * point.x + polygon.b * point.y + polygon.d) / polygon.c;
}

static inline float get_azimuth(const point2 &o, const point2 &p) {
    return atan2f(p.y - o.y, p.x - o.x);
}

static float get_angle(const point2 &o, const point2 &a, const point2 &b) {
    float result = get_azimuth(o, b) - get_azimuth(o, a);
    if (result > M_PI)
        result -= 2 * M_PI;
    if (result < -M_PI)
        result += 2 * M_PI;

    return result;
}

static bool is_inside_polygon(const point2 &point, const polygon &polygon) {
    float sum = get_angle(point, polygon.vertices[polygon.vertices.size() - 1],
                          polygon.vertices[0]);

    for (int i = 0; i < polygon.vertices.size() - 1; ++i)
        sum += get_angle(point, polygon.vertices[i], polygon.vertices[i + 1]);

    return fabsf(sum) > 1e-5f;
}

static inline bool on_segment(const point2 &p, const point2 &q, const point2 &r) {
    return q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
           q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y);
}

static int orientation(const point2 &p, const point2 &q, const point2 &r) {
    int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if (val == 0)
        return 0;
    return (val > 0) ? 1 : 2;
}

static bool check_lines_intersection(const line2 &line1, const line2 &line2) {
    int o1 = orientation(line1.begin, line1.end, line2.begin);
    int o2 = orientation(line1.begin, line1.end, line2.end);
    int o3 = orientation(line2.begin, line2.end, line1.begin);
    int o4 = orientation(line2.begin, line2.end, line1.end);

    // General case
    if (o1 != o2 && o3 != o4)
        return true;

    // Special Cases: p1, q1 and p2 are collinear and p2 lies on segment p1q1
    if (o1 == 0 && on_segment(line1.begin, line2.begin, line1.end))
        return true;

    // p1, q1 and q2 are collinear and q2 lies on segment p1q1
    if (o2 == 0 && on_segment(line1.begin, line2.end, line1.end))
        return true;

    // p2, q2 and p1 are collinear and p1 lies on segment p2q2
    if (o3 == 0 && on_segment(line2.begin, line1.begin, line2.end))
        return true;

    // p2, q2 and q1 are collinear and q1 lies on segment p2q2
    if (o4 == 0 && on_segment(line2.begin, line1.end, line2.end))
        return true;

    return false;
}

static relationship check_relationship(const polygon &polygon, const window &window) {
    uint16_t x_min = window.begin.x, x_max = window.end.x - 1;
    uint16_t y_min = window.begin.y, y_max = window.end.y - 1;
    point2 windows_vertices[4] = {{x_min, y_min}, {x_min, y_max}, {x_max, y_max}, {x_max, y_min}};

    for (int i = 0; i < polygon.vertices.size(); ++i) {
        for (int j = 0; j < 4; ++j) {
            point2 p1 = polygon.vertices[(i + 1) % polygon.vertices.size()];
            point2 p2 = polygon.vertices[i % polygon.vertices.size()];
            point2 p3 = windows_vertices[j % 4];
            point2 p4 = windows_vertices[(j + 1) % 4];
            if (check_lines_intersection({p1, p2}, {p3, p4}))
                return relationship::intersecting;
        }
    }


    if (polygon.vertices[0].x >= x_min && polygon.vertices[0].x <= x_max &&
        polygon.vertices[0].y >= y_min && polygon.vertices[0].y <= y_max)
        return relationship::contained;

    return is_inside_polygon(windows_vertices[0], polygon) ? relationship::surrounding
                                                  : relationship::disjoint;
}

static void fill_pixel(const point2 &point, std::vector<polygon> &polygons,
                       void set_pixel(point2, uint16_t)) {
    uint16_t color = polygons[0].color;
    float z_max = get_z(polygons[0], point);
    for (int i = 1; i < polygons.size(); ++i) {
        float z = get_z(polygons[i], point);
        if (z > z_max) {
            z_max = z;
            color = polygons[i].color;
        }
    }

    set_pixel(point, color);
}

void fill_window(const window &window, const uint16_t color,
                 void set_pixel(point2, uint16_t)) {
    for (uint16_t x = window.begin.x; x < window.end.x; ++x) {
        for (uint16_t y = window.begin.y; y < window.end.y; ++y) {
            set_pixel({x, y}, color);
        }
    }
}

void split_window(std::stack<window> &stack, const window &window,
                  std::vector<polygon> &polygons) {
    uint16_t x_split = window.begin.x + ((window.end.x - window.begin.x) >> 1);
    uint16_t y_split = window.begin.y + ((window.end.y - window.begin.y) >> 1);

    uint16_t window_width = window.end.x - window.begin.x;
    uint16_t window_height = window.end.y - window.begin.y;

    if (window_width > 1 && window_height > 1) {
        stack.push(
                {{window.begin.x, window.begin.y}, {x_split, y_split}, polygons});
        stack.push(
                {{x_split, window.begin.y}, {window.end.x, y_split}, polygons});
        stack.push(
                {{window.begin.x, y_split}, {x_split, window.end.y}, polygons});
        stack.push(
                {{x_split, y_split}, {window.end.x, window.end.y}, polygons});
    } else if (window_width > 1) {
        stack.push({{window.begin.x, window.begin.y},
                    {x_split, window.end.y},
                    polygons});
        stack.push({{x_split, window.begin.y},
                    {window.end.x, window.end.y},
                    polygons});
    } else {
        stack.push({{window.begin.x, window.begin.y},
                    {window.end.x, y_split},
                    polygons});
        stack.push({{window.begin.x, y_split},
                    {window.end.x, window.end.y},
                    polygons});
    }
}

std::pair<bool, polygon> find_cover_polygon(const window &window, std::vector<polygon> &polygons) {
    auto window_end_x = static_cast<uint16_t>(window.end.x - 1);
    auto window_end_y = static_cast<uint16_t>(window.end.y - 1);

    point2 window_vertices[4] = {
            {window.begin.x, window.begin.y},
            {window.begin.x, window_end_y},
            {window_end_x, window_end_y},
            {window_end_x, window.begin.y}};

    size_t polygon_indices[4] = {0};
    float z_max[4];

    for (uint32_t i = 0; i < 4; ++i)
        z_max[i] = get_z(polygons[0], window_vertices[i]);

    for (uint32_t i = 1; i < polygons.size(); ++i) {
        float z[4];

        for (uint32_t j = 0; j < 4; ++j) {
            z[j] = get_z(polygons[i], window_vertices[j]);

            if (z[j] - z_max[j] > std::numeric_limits<float>::epsilon()) {
                z_max[j] = z[j];
                polygon_indices[j] = i;
            }
        }
    }

    for (int i = 1; i < 4; ++i) {
        if (polygon_indices[i - 1] != polygon_indices[i])
            return {false, {}};
    }

    return {true, polygons[polygon_indices[0]]};
}

void warnock_render(const window &full_window, const uint16_t bg_color,
                    void set_pixel(point2, uint16_t)) {
    std::stack<window> stack;
    stack.push(full_window);

    while (!stack.empty()) {
        window current_window = stack.top();
        stack.pop();

        std::vector<polygon> visible;
        bool split_flag = false;
        for (auto &polygon : current_window.polygons) {
            relationship rel = check_relationship(polygon, current_window);

            if (rel != relationship::disjoint)
                visible.push_back(polygon);

            if (rel == relationship::contained ||
                rel == relationship::intersecting)
                split_flag = true;
        }

        uint16_t window_width = current_window.end.x - current_window.begin.x;
        uint16_t window_height = current_window.end.y - current_window.begin.y;

        if (window_width == 1 && window_height == 1) {
            if (visible.empty()) {
                set_pixel(current_window.begin, bg_color);
            } else {
                fill_pixel(current_window.begin, visible, set_pixel);
            }
        } else if (split_flag) {
            split_window(stack, current_window, visible);
        } else {
            if (visible.empty()) {
                fill_window(current_window, bg_color, set_pixel);
                continue;
            }

            std::pair<bool, polygon> result = find_cover_polygon(current_window, visible);
            if (result.first) {
                fill_window(current_window, result.second.color, set_pixel);
            } else {
                split_window(stack, current_window, visible);
            }
        }
    }
}