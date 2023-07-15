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

static uint16_t material_to_rgb565(material material) {
}

bool preprocess_objects(const std::map<std::string, object> &objects,
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

            polygon.color = material_to_rgb565(face.material);
            compute_plane_equation(vertices, polygon);
            polygons.push_back(polygon);
        }
    }

    return true;
}
