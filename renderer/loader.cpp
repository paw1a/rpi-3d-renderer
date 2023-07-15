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

bool load_objects(std::ifstream &ifs, const std::map<std::string, material> &materials,
                  std::map<std::string, object> &objects) {
    object object = {};
    material material = {};
    std::string object_name;
    for (std::string line; getline(ifs, line);) {
        size_t str_index;
        while ((str_index = line.find('\t')) != std::string::npos)
            line.erase(str_index, 1);

        std::vector<std::string> tokens = split(line, " ");
        tokens.erase(remove(tokens.begin(), tokens.end(), ""), tokens.end());

        if (tokens.size() == 2 && tokens[0] == "o") {
            if (!object_name.empty())
                objects[object_name] = object;

            object.faces.clear();
            object.vertices.clear();
            object.normals.clear();
            object_name = tokens[1];
        }

        if (tokens.size() == 4 && tokens[0] == "v") {
            vertex vertex = {stof(tokens[1]), stof(tokens[2]), stof(tokens[3])};
            object.vertices.push_back(vertex);
        }

        if (tokens.size() == 4 && tokens[0] == "vn") {
            vec3 normal = {stof(tokens[1]), stof(tokens[2]), stof(tokens[3])};
            object.normals.push_back(normal);
        }

        if (tokens.size() >= 4 && tokens[0] == "f") {
            face face = {.material = material};

            for (size_t i = 1; i < tokens.size(); ++i) {
                std::vector<std::string> indices = split(tokens[i], "/");
                if (indices.size() == 1)
                    continue;

                if (indices.size() != 3) {
                    std::cout << "invalid object file format\n";
                    return false;
                }

                face.vertex_indices.push_back(stoull(indices[0]) - 1);
                face.normal_index = stoull(indices[2]) - 1;
            }

            object.faces.push_back(face);
        }

        if (tokens.size() == 2 && tokens[0] == "usemtl") {
            auto it = materials.find(tokens[1]);
            if (it == materials.end()) {
                std::cout << "material with name " << tokens[1] << " not found" << std::endl;
                return false;
            }

            material = it->second;
        }
    }

    if (!object_name.empty())
        objects[object_name] = object;

    return true;
}

bool load_materials(std::ifstream &ifs, std::map<std::string, material> &materials) {
    material material = {};
    std::string material_name;
    float ns = 0;

    for (std::string line; getline(ifs, line);) {
        size_t str_index;
        while ((str_index = line.find('\t')) != std::string::npos)
            line.erase(str_index, 1);

        std::vector<std::string> tokens = split(line, " ");
        tokens.erase(remove(tokens.begin(), tokens.end(), ""), tokens.end());

        if (tokens.size() == 2 && tokens[0] == "newmtl") {
            if (!material_name.empty())
                materials[material_name] = {{material.color.r * ns,
                                            material.color.g * ns,
                                            material.color.b * ns}};

            material_name = tokens[1];
        }

        if (tokens.size() == 2 && tokens[0] == "Ns")
            ns = stof(tokens[1]);

        if (tokens.size() == 4 && tokens[0] == "Kd")
            material.color = {stof(tokens[1]), stof(tokens[2]), stof(tokens[3])};
    }

    if (!material_name.empty())
        materials[material_name] = {{material.color.r * ns,
                                    material.color.g * ns,
                                    material.color.b * ns}};

    return true;
}
