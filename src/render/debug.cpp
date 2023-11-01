#include "debug.h"

std::ostream &operator<<(std::ostream &os, const point2 &point) {
    return os << '(' << point.x << "; " << point.y << ')';
}

std::ostream &operator<<(std::ostream &os, const color &color) {
    return os << '(' << (int)color.r << "; " << (int)color.g << "; "
              << (int)color.b << ')';
}

std::ostream &operator<<(std::ostream &os, const material &material) {
    return os << '(' << (int)material.color.r << "; " << (int)material.color.g
              << "; " << (int)material.color.b << ')';
}

std::ostream &operator<<(std::ostream &os, const face &face) {
    os << "face vertex indices: ";
    for (size_t i = 0; i < face.vertex_indices.size(); i++)
        os << face.vertex_indices[i] << ", ";
    os << std::endl;

    os << "face normal index: " << face.normal_index << std::endl;
    os << "face material index: " << face.material_index;

    return os;
}

std::ostream &operator<<(std::ostream &os, const object &object) {
    os << "vertices:" << std::endl;
    for (size_t i = 0; i < object.vertices.size(); i++)
        os << object.vertices[i] << std::endl;

    os << "normals:" << std::endl;
    for (size_t i = 0; i < object.normals.size(); i++)
        os << object.normals[i] << std::endl;

    os << "faces:" << std::endl;
    for (size_t i = 0; i < object.faces.size(); i++)
        os << object.faces[i] << std::endl << std::endl;

    return os;
}

std::ostream &operator<<(std::ostream &os, const polygon &polygon) {
    os << "polygon vertices: ";
    for (size_t i = 0; i < polygon.vertices.size(); i++)
        os << polygon.vertices[i] << ", ";
    os << std::endl;

    os << "polygon plane factors: " << polygon.a << ", " << polygon.b << ", "
       << polygon.c << ", " << polygon.d << std::endl;
    os << "polygon color hex: " << std::hex << polygon.color << std::dec;

    return os;
}

std::ostream &operator<<(std::ostream &os, const window &window) {
    os << "window: ";
    os << window.begin << ", " << window.end << std::endl;
    os << "window polygons:" << std::endl;
    //    for (auto &polygon : window.polygons)
    //        os << polygon << std::endl;

    return os;
}
