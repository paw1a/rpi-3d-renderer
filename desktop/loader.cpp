#include "loader.h"
#include <iostream>
#include <string>
#include <vector>

static std::vector<std::string> split(const std::string &s,
                                      const std::string &delimiter) {
    size_t pos_start = 0;
    size_t pos_end;
    size_t delim_len = delimiter.length();

    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

bool load_objects(std::ifstream &ifs,
                  const std::map<std::string, size_t> &material_names,
                  std::vector<object> &objects) {
    object object = {};
    size_t material_index;
    std::pair<size_t, size_t> vertices_count;
    std::pair<size_t, size_t> normals_count;

    bool flag = false;
    for (std::string line; getline(ifs, line);) {
        size_t str_index;
        while ((str_index = line.find('\t')) != std::string::npos)
            line.erase(str_index, 1);

        std::vector<std::string> tokens = split(line, " ");
        tokens.erase(remove(tokens.begin(), tokens.end(), ""), tokens.end());

        if (tokens.size() == 2 && tokens[0] == "o") {
            if (flag) {
                objects.push_back(object);
            }

            flag = true;
            object = {};
            vertices_count.first = vertices_count.second;
            normals_count.first = normals_count.second;
        }

        if (tokens.size() == 4 && tokens[0] == "v") {
            m3::vec3 vertex = {stof(tokens[1]), stof(tokens[2]),
                               stof(tokens[3])};
            object.vertices.push_back(vertex);
            ++vertices_count.second;
        }

        if (tokens.size() == 4 && tokens[0] == "vn") {
            m3::vec3 normal = {stof(tokens[1]), stof(tokens[2]),
                               stof(tokens[3])};
            object.normals.push_back(normal);
            ++normals_count.second;
        }

        if (tokens.size() >= 4 && tokens[0] == "f") {
            face face = {.material_index = material_index};

            for (size_t i = 1; i < tokens.size(); ++i) {
                std::vector<std::string> indices = split(tokens[i], "/");
                if (indices.size() == 1)
                    continue;

                if (indices.size() != 3) {
                    std::cout << "invalid object file format" << std::endl;
                    return false;
                }

                face.vertex_indices.push_back(stoull(indices[0]) - 1 -
                                              vertices_count.first);
                face.normal_index =
                    stoull(indices[2]) - 1 - normals_count.first;
            }

            object.faces.push_back(face);
        }

        if (tokens.size() == 2 && tokens[0] == "usemtl") {
            auto it = material_names.find(tokens[1]);
            if (it == material_names.end()) {
                std::cout << "material with name " << tokens[1] << " not found"
                          << std::endl;
                return false;
            }

            material_index = it->second;
        }
    }

    if (flag) {
        objects.push_back(object);
    }

    return true;
}

bool load_materials(std::ifstream &ifs,
                    std::vector<material> &materials,
                    std::map<std::string, size_t> &material_names) {
    material material = {};
    std::string material_name;
    float ns = 0;

    size_t index = 0;
    for (std::string line; getline(ifs, line);) {
        size_t str_index;
        while ((str_index = line.find('\t')) != std::string::npos)
            line.erase(str_index, 1);

        std::vector<std::string> tokens = split(line, " ");
        tokens.erase(remove(tokens.begin(), tokens.end(), ""), tokens.end());

        if (tokens.size() == 2 && tokens[0] == "newmtl") {
            if (!material_name.empty()) {
                materials.push_back({{material.color.r * ns,
                                      material.color.g * ns,
                                      material.color.b * ns}});
                material_names[material_name] = index++;
            }

            material_name = {tokens[1]};
        }

        if (tokens.size() == 2 && tokens[0] == "Ns")
            ns = stof(tokens[1]);

        if (tokens.size() == 4 && tokens[0] == "Kd")
            material.color = {stof(tokens[1]), stof(tokens[2]),
                              stof(tokens[3])};
    }

    if (!material_name.empty()) {
        materials.push_back({{material.color.r * ns,
                              material.color.g * ns,
                              material.color.b * ns}});
        material_names[material_name] = index;
    }

    if (materials.empty()) {
        std::cout << "nothing to load from material file" << std::endl;
        return false;
    }

    return true;
}

bool load_scene(std::ifstream &ifs, scene &scene) {
    std::string object_path{};
    std::string material_path{};
    for (std::string line; getline(ifs, line);) {
        size_t str_index;
        while ((str_index = line.find('\t')) != std::string::npos)
            line.erase(str_index, 1);

        std::vector<std::string> tokens = split(line, " ");
        tokens.erase(remove(tokens.begin(), tokens.end(), ""), tokens.end());

        if (tokens.size() == 2 && tokens[0] == "o")
            object_path = {tokens[1]};

        if (tokens.size() == 2 && tokens[0] == "m")
            material_path = {tokens[1]};

        if (tokens.size() == 4 && tokens[0] == "l")
            scene.lights.emplace_back(stof(tokens[1]), stof(tokens[2]),
                                      stof(tokens[3]));

        if (tokens.size() == 4 && tokens[0] == "cp")
            scene.camera.position = {stof(tokens[1]), stof(tokens[2]),
                                     stof(tokens[3])};

        if (tokens.size() == 4 && tokens[0] == "ct")
            scene.camera.target = {stof(tokens[1]), stof(tokens[2]),
                                     stof(tokens[3])};

        if (tokens.size() == 4 && tokens[0] == "cu")
            scene.camera.up = {stof(tokens[1]), stof(tokens[2]),
                                     stof(tokens[3])};
    }

    std::ifstream material_ifs(material_path, std::ios::in);
    if (!material_ifs.is_open()) {
        std::cout << "failed to open material file " << object_path << std::endl;
        return false;
    }

    std::map<std::string, size_t> material_names;
    if (!load_materials(material_ifs, scene.materials, material_names)) {
        std::cout << "failed to load materials from file " << object_path << std::endl;
        return false;
    }

    std::ifstream object_ifs(object_path, std::ios::in);
    if (!object_ifs.is_open()) {
        std::cout << "failed to open object file " << object_path << std::endl;
        return false;
    }

    if (!load_objects(object_ifs, material_names, scene.objects)) {
        std::cout << "failed to load objects from file " << object_path << std::endl;
        return false;
    }

    return true;
}
