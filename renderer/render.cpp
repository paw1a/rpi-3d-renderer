#include "render.h"
#include <cmath>
#include <stack>

enum class relationship {
    disjoint,
    contained,
    intersecting,
    surrounding
};

static inline float get_z(const polygon &polygon, const point2 &point) {
    return -(polygon.a * point.x + polygon.b * point.y + polygon.d) / polygon.c;
}

static void set_pixel(const point2 &pixel, const uint32_t color) {
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

bool is_inside_polygon(const point2 &point, const polygon &polygon) {
    point2 a = polygon.vertices[polygon.vertices.size() - 1];
    point2 b = polygon.vertices[0];
    float sum = get_angle(point, a, b);

    for (int i = 0; i < polygon.vertices.size() - 1; ++i)
        sum += get_angle(point, polygon.vertices[i], polygon.vertices[i + 1]);

    return fabsf(sum) > std::numeric_limits<float>::epsilon();
}

static inline bool on_segment(const point2 &p, const point2 &q,
                              const point2 &r) {
    return q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
           q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y);
}

inline int orientation(const point2 &p, const point2 &q, const point2 &r) {
    int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

    if (val == 0)
        return 0; // collinear

    return (val > 0) ? 1 : 2; // clock or clock wise
}

static bool check_lines_intersect2D(const line2 &line1, const line2 &line2) {
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

static relationship check_relationship(const polygon &polygon,
                                       const window &window) {
    uint32_t xmin = window.begin.x, xmax = window.end.x - 1;
    uint32_t ymin = window.begin.y, ymax = window.end.y - 1;
    point2 window_points[4] = {
        {xmin, ymin}, {xmin, ymax}, {xmax, ymax}, {xmax, ymin}};

    for (int i = 0; i < polygon.vertices.size(); ++i) {
        for (int j = 0; j < 4; ++j) {
            point2 p1 = polygon.vertices[(i + 1) % polygon.vertices.size()];
            point2 p2 = polygon.vertices[i % polygon.vertices.size()];
            point2 p3 = window_points[j % 4];
            point2 p4 = window_points[(j + 1) % 4];
            if (check_lines_intersect2D({p1, p2}, {p3, p4}))
                return relationship::intersecting;
        }
    }

    if (polygon.vertices[0].x >= xmin && polygon.vertices[0].x <= xmax &&
        polygon.vertices[0].y >= ymin && polygon.vertices[0].y <= ymax)
        return relationship::contained;

    return is_inside_polygon(window_points[0], polygon)
               ? relationship::surrounding
               : relationship::disjoint;
}

static void fill_pixel(const point2 &point, std::vector<polygon> &polygons,
                       const uint32_t bg_color) {
    if (polygons.empty()) {
        set_pixel(point, bg_color);
        return;
    }

    uint32_t color = polygons[0].color;
    float zmax = get_z(polygons[0], point);
    for (int i = 1; i < polygons.size(); ++i) {
        float z = get_z(polygons[i], point);
        if (z > zmax) {
            zmax = z;
            color = polygons[i].color;
        }
    }

    set_pixel(point, color);
}

static void fill_window(const window &window, const uint32_t color) {
    for (uint16_t x = window.begin.x; x < window.end.x; ++x)
        for (uint16_t y = window.begin.y; y < window.end.y; ++y)
            set_pixel({x, y}, color);
}

static void split_window(std::stack<window> &stack, const window &window,
                         const std::vector<polygon> &polygons) {
    uint32_t window_width = window.end.x - window.begin.x;
    uint32_t window_height = window.end.y - window.begin.y;

    uint32_t x_split = window.begin.x + ((window.end.x - window.begin.x) / 2);
    uint32_t y_split = window.begin.y + ((window.end.y - window.begin.y) / 2);

    if (window_width > 1 && window_height > 1) {
        stack.push({{window.begin.x, window.begin.y}, {x_split, y_split}, polygons});
        stack.push({{x_split, window.begin.y}, {window.end.x, y_split}, polygons});
        stack.push({{window.begin.x, y_split}, {x_split, window.end.y}, polygons});
        stack.push({{x_split, y_split}, {window.end.x, window.end.y}, polygons});
    } else if (window_width > 1) {
        stack.push({{window.begin.x, window.begin.y}, {x_split, window.end.y}, polygons});
        stack.push({{x_split, window.begin.y}, {window.end.x, window.end.y}, polygons});
    } else {
        stack.push({{window.begin.x, window.begin.y}, {window.end.x, y_split}, polygons});
        stack.push({{window.begin.x, y_split}, {window.end.x, window.end.y}, polygons});
    }
}

static std::pair<bool, polygon> find_cover_polygon(const window &window, std::vector<polygon> &polygons) {
    point2 window_points[4] = {{window.begin.x, window.begin.y},
                           {window.begin.x, window.end.y - 1},
                           {window.end.x - 1, window.end.y - 1},
                           {window.end.x - 1, window.begin.y}};

    float zmax[4];
    for (uint32_t i = 0; i < 4; ++i)
        zmax[i] = get_z(polygons[0], window_points[i]);

    uint32_t cover_points_indices[4] = {0};
    for (uint32_t i = 1; i < polygons.size(); ++i) {
        float z[4];
        for (uint32_t j = 0; j < 4; ++j) {
            z[j] = get_z(polygons[i], window_points[j]);
            if (z[j] - zmax[j] > std::numeric_limits<float>::epsilon()) {
                zmax[j] = z[j];
                cover_points_indices[j] = i;
            }
        }
    }

    for (int i = 1; i < 4; i++) {
        if (cover_points_indices[i - 1] != cover_points_indices[i])
            return {false, {}};
    }

    return {true, polygons[cover_points_indices[0]]};
}

void warnock_render(const window &win, const uint32_t bg_color) {
    std::stack<window> stack;
    stack.push(win);
    while (!stack.empty()) {
        window cur_win = stack.top();
        stack.pop();

        std::vector<polygon> visible;
        bool split_flag = false;
        for (auto &polygon : cur_win.polygons) {
            relationship rel = check_relationship(polygon, cur_win);
            if (rel != relationship::disjoint)
                visible.push_back(polygon);

            if (rel == relationship::intersecting || rel == relationship::contained)
                split_flag = true;
        }

        uint32_t window_width = cur_win.end.x - cur_win.begin.x;
        uint32_t window_height = cur_win.end.y - cur_win.begin.y;

        if (window_width == 1 && window_height == 1) {
            fill_pixel(cur_win.begin, visible, bg_color);
        } else if (split_flag) {
            split_window(stack, cur_win, visible);
        } else {
            if (visible.empty()) {
                fill_window(cur_win, bg_color);
                return;
            }

            std::pair<bool, polygon> result = find_cover_polygon(cur_win, visible);
            if (result.first) {
                fill_window(cur_win, result.second.color);
            } else {
                split_window(stack, cur_win, visible);
            }
        }
    }
}
