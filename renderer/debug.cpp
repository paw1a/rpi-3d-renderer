#include "debug.h"

std::ostream &operator<<(std::ostream &os, const point3 &point) {
    return os  << '(' << point.x << "; " << point.y << "; " << point.z << ')';
}

std::ostream &operator<<(std::ostream &os, const face &face) {
    os << "face vertex indices: ";
    for (auto &vertex_index : face.vertex_indices)
        os << vertex_index << ", ";
    os << std::endl;

    os << "face normal index: " << face.normal_index << std::endl;
    os << "face color: " << face.color;

    return os;
}

std::ostream &operator<<(std::ostream &os, const object &object) {
    os << "vertices:" << std::endl;
    for (auto &vertex : object.vertices)
        os << vertex << std::endl;

    os << "normals:" << std::endl;
    for (auto &normal : object.normals)
        os << normal << std::endl;

    os << "faces:" << std::endl;
    for (auto &face : object.faces)
        os << face << std::endl << std::endl;

    return os;
}
