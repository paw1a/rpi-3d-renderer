#include "pipeline.h"
#include "common.h"
#include <cmath>
#include <map>
#include <string>

static void move_points(std::vector<point3> &points, const vec3 &diff) {
    for (auto &point : points) {
        point.x += diff.x;
        point.y += diff.y;
        point.z += diff.z;
    }
}

static void scale_points(std::vector<point3> &points, const float factor) {
    for (auto &point : points) {
        point.x *= factor;
        point.y *= factor;
        point.z *= factor;
    }
}

static inline float scalar_product(const vec3 &vec1, const vec3 &vec2) {
    return (vec1.x * vec2.x) + (vec1.y * vec2.y) + (vec1.z * vec2.z);
}

static inline float magnitude(const vec3 &vec) {
    return (sqrtf(powf(vec.x, 2) + powf(vec.y, 2) + powf(vec.z, 2)));
}

static inline float cos_angle(const vec3 &vec1, const vec3 &vec2) {
    return scalar_product(vec1, vec2) / (magnitude(vec1) * magnitude(vec2));
}

static bool solve_slae(std::vector<float> &res_column,
                       std::vector<std::vector<float>> matrix,
                       std::vector<float> free_factor_column) {
    size_t rows_count = matrix.size();
    size_t columns_count = matrix[0].size();

    for (size_t k = 0; k < columns_count; k++) {
        float max = abs(matrix[k][k]);
        int row_index = k;

        for (size_t i = k + 1; i < rows_count; i++) {
            if (abs(matrix[i][k]) > max) {
                max = abs(matrix[i][k]);
                row_index = i;
            }
        }

        std::swap(matrix[k], matrix[row_index]);
        std::swap(free_factor_column[k], free_factor_column[row_index]);

        for (size_t i = k + 1; i < rows_count; i++) {
            float c = matrix[i][k] / matrix[k][k];

            for (size_t j = k; j < columns_count; j++)
                matrix[i][j] -= c * matrix[k][j];

            free_factor_column[i] -= c * free_factor_column[k];
        }
    }

    if (abs(matrix[rows_count - 1][columns_count - 1]) <=
        std::numeric_limits<float>::epsilon())
        return false;

    for (int i = rows_count - 1; i >= 0; i--) {
        float sum = 0;
        for (size_t j = i + 1; j < columns_count; j++)
            sum += matrix[i][j] * res_column[j];

        res_column[i] = (free_factor_column[i] - sum) / matrix[i][i];
    }

    return true;
}

static void compute_plane_equation(const std::vector<vertex> &vertices,
                                   polygon &polygon) {
    polygon.a = polygon.b = polygon.c = 0;
    polygon.d = 1000;
    for (size_t i = 0; i < vertices.size(); ++i) {
        for (size_t j = i + 1; j < vertices.size(); ++j) {
            for (size_t k = j + 1; k < vertices.size(); ++k) {
                std::vector<float> res(3, 0.);
                std::vector<std::vector<float>> matrix = {
                    {vertices[i].x, vertices[i].y, vertices[i].z},
                    {vertices[j].x, vertices[j].y, vertices[j].z},
                    {vertices[k].x, vertices[k].y, vertices[k].z}};
                std::vector<float> free_factor_column(3, -polygon.d);

                if (solve_slae(res, matrix, free_factor_column)) {
                    polygon.a = res[0];
                    polygon.b = res[1];
                    polygon.c = res[2];
                }
            }
        }
    }
}

void adjust_data_to_display(std::map<std::string, object> &objects) {
    float xmin = 0., xmax = 0., ymin = 0., ymax = 0.;

    for (auto const &[name, object] : objects) {
        for (auto &vertex : object.vertices) {
            if (vertex.x < xmin)
                xmin = vertex.x;

            if (vertex.x > xmax)
                xmax = vertex.x;

            if (vertex.y < ymin)
                ymin = vertex.y;

            if (vertex.y > ymax)
                ymax = vertex.y;
        }
    }

    vec3 diff = {-xmin, -ymin, 0.};
    for (auto &[name, object] : objects)
        move_points(object.vertices, diff);

    xmax -= xmin;
    ymax -= ymin;

    size_t display_width = SCREEN_WIDTH, display_height = SCREEN_HEIGHT;

    float scale_coef_x = display_width / xmax;
    float scale_coef_y = display_height / ymax;
    for (auto &[name, object] : objects) {
        scale_points(object.vertices, std::min(scale_coef_x, scale_coef_y));
    }
}

static uint16_t material_to_rgb565(const material &material, const std::vector<vec3> &lights,
                                   const vec3 &normal) {
    float light_percent = 0.0;
    for (auto &light : lights) {
        float percent = cos_angle(light, normal);
        if (percent > 0)
            light_percent += percent;
    }

    if (light_percent <= 0.)
        return material_color_to_rgb565({0.0, 0.0, 0.0});

    material_color color = {material.color.r * light_percent,
                            material.color.g * light_percent,
                            material.color.b * light_percent};

    float max_primary_color = std::fmax(color.r / 255, std::fmax(color.g / 255, color.b / 255));
    if (max_primary_color > 1.0)
        color = {color.r / max_primary_color,
                 color.g / max_primary_color,
                 color.b / max_primary_color};

    return material_color_to_rgb565(color);
}

bool preprocess_objects(const std::map<std::string, object> &objects, const std::vector<vec3> &lights,
                        std::vector<polygon> &polygons) {
    for (auto const &[name, object] : objects) {
        for (auto &face : object.faces) {
            std::vector<vertex> vertices;
            for (auto &index : face.vertex_indices)
                vertices.push_back(object.vertices[index]);

            polygon polygon;
            for (auto &vertex : vertices) {
                auto x = static_cast<uint16_t>(vertex.x);
                auto y = static_cast<uint16_t>(vertex.y);
                polygon.vertices.push_back({x, y});
            }

            polygon.color = material_to_rgb565(face.material, lights,
                                               object.normals[face.normal_index]);
            compute_plane_equation(vertices, polygon);
            polygons.push_back(polygon);
        }
    }

    return true;
}
