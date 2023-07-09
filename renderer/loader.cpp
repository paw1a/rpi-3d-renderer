#include "loader.h"
#include <vector>
#include <string>
#include <fstream>

static std::vector<std::string> split(const std::string &s, const std::string &delimiter) {
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

bool load_objects(std::ifstream &ifs, std::map<std::string, object> &objects) {
    object object;
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
            color color = {(float)rand() / (float)RAND_MAX,
                           (float)rand() / (float)RAND_MAX,
                           (float)rand() / (float)RAND_MAX};
            face face = {.color = color};

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
    }

    if (!object_name.empty())
        objects[object_name] = object;

    return true;
}
