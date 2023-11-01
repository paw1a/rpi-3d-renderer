#include "loader.h"
#include "dataset.h"
#include "allocator.h"

#include <iostream>
#include <string>
#include <vector>
#include <cstring>

struct loader_stream : public std::streambuf
{
    explicit loader_stream(const std::string &s) :
          loader_stream(const_cast<char*>(s.c_str()), s.length()) {}
    loader_stream(char *c, std::size_t l) { setg(c, c, c + l); }
};

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

struct object_info {
    size_t vertex_count;
    size_t face_count;
};

static size_t count_scene_objects(dataset &dataset) {
    const char *data = dataset.obj;
    loader_stream stream((char *)data, strlen(data));
    std::istream is(&stream);

    size_t count = 0;
    for (std::string line; getline(is, line);) {
        size_t str_index;
        while ((str_index = line.find('\t')) != std::string::npos)
            line.erase(str_index, 1);

        std::vector<std::string> tokens = split(line, " ");
        tokens.erase(remove(tokens.begin(), tokens.end(), ""), tokens.end());

        if (tokens.size() == 2 && tokens[0] == "o")
            count++;
    }

    return count;
}

static array<object_info> count_scene_object_elements(dataset &dataset,
                                                      size_t objects_count) {
    const char *data = dataset.obj;
    loader_stream stream((char *)data, strlen(data));
    std::istream is(&stream);

    array<object_info> infos = {(object_info *)allocate(objects_count * sizeof(object_info)),
                                objects_count};

    bool flag = false;
    object_info info = {};
    size_t info_index = 0;
    for (std::string line; getline(is, line);) {
        size_t str_index;
        while ((str_index = line.find('\t')) != std::string::npos)
            line.erase(str_index, 1);

        std::vector<std::string> tokens = split(line, " ");
        tokens.erase(remove(tokens.begin(), tokens.end(), ""), tokens.end());

        if (tokens.size() == 2 && tokens[0] == "o") {
            if (flag) {
                infos[info_index++] = info;
            }

            flag = true;
            info = {};
        }

        if (tokens.size() == 4 && tokens[0] == "v") {
            info.vertex_count++;
        }

        if (tokens.size() == 4 && tokens[0] == "vn") {
            info.face_count++;
        }
    }

    if (flag) {
        infos[info_index] = info;
    }

    return infos;
}

bool load_objects(dataset &dataset,
                  const std::map<std::string, size_t> &material_names,
                  array<object> &objects) {
    size_t material_index;
    std::pair<size_t, size_t> vertices_count;
    std::pair<size_t, size_t> normals_count;

    bool flag = false;
    size_t object_index = 0;
    size_t vertex_index = 0;
    size_t face_index = 0;
    size_t normal_index = 0;

    const char *data = dataset.obj;
    loader_stream stream((char *)data, strlen(data));
    std::istream is(&stream);

    for (std::string line; getline(is, line);) {
        size_t str_index;
        while ((str_index = line.find('\t')) != std::string::npos)
            line.erase(str_index, 1);

        std::vector<std::string> tokens = split(line, " ");
        tokens.erase(remove(tokens.begin(), tokens.end(), ""), tokens.end());

        if (tokens.size() == 2 && tokens[0] == "o") {
            if (flag) {
                object_index++;
            }

            flag = true;
            vertices_count.first = vertices_count.second;
            normals_count.first = normals_count.second;
            vertex_index = 0;
            face_index = 0;
            normal_index = 0;
        }

        if (tokens.size() == 4 && tokens[0] == "v") {
            m3::vec3 vertex = {stof(tokens[1]), stof(tokens[2]),
                               stof(tokens[3])};
            objects[object_index].vertices[vertex_index++] = vertex;
            ++vertices_count.second;
        }

        if (tokens.size() == 4 && tokens[0] == "vn") {
            m3::vec3 normal = {stof(tokens[1]), stof(tokens[2]),
                               stof(tokens[3])};
            objects[object_index].normals[normal_index++] = normal;
            ++normals_count.second;
        }

        if (tokens.size() >= 4 && tokens[0] == "f") {
            objects[object_index].faces[face_index].
                material_index = material_index;

            std::vector<std::string> indices = split(tokens[1], "/");
            objects[object_index].faces[face_index].
                normal_index = stoull(indices[2]) - 1 - normals_count.first;

            objects[object_index].faces[face_index].
                vertex_indices = array<size_t>(tokens.size());
            for (size_t i = 1; i < tokens.size(); ++i) {
                indices = split(tokens[i], "/");
                if (indices.size() == 1)
                    continue;

                if (indices.size() != 3) {
                    std::cout << "invalid object file format" << std::endl;
                    return false;
                }

                objects[object_index].faces[face_index].vertex_indices[i - 1] =
                    stoull(indices[0]) - 1 - vertices_count.first;
            }
            face_index++;
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

    return true;
}

static size_t count_scene_materials(dataset &dataset) {
    const char *data = dataset.mtl;
    loader_stream stream((char *)data, strlen(data));
    std::istream is(&stream);

    size_t count = 0;
    for (std::string line; getline(is, line);) {
        size_t str_index;
        while ((str_index = line.find('\t')) != std::string::npos)
            line.erase(str_index, 1);

        std::vector<std::string> tokens = split(line, " ");
        tokens.erase(remove(tokens.begin(), tokens.end(), ""), tokens.end());

        if (tokens.size() == 2 && tokens[0] == "newmtl")
            count++;
    }

    return count;
}

bool load_materials(dataset &dataset, array<material> &materials,
                    std::map<std::string, size_t> &material_names) {
    material material = {};
    std::string material_name;
    float ns = 0;

    const char *data = dataset.mtl;
    loader_stream stream((char *)data, strlen(data));
    std::istream is(&stream);

    size_t index = 0;
    size_t material_index = 0;
    for (std::string line; getline(is, line);) {
        size_t str_index;
        while ((str_index = line.find('\t')) != std::string::npos)
            line.erase(str_index, 1);

        std::vector<std::string> tokens = split(line, " ");
        tokens.erase(remove(tokens.begin(), tokens.end(), ""), tokens.end());

        if (tokens.size() == 2 && tokens[0] == "newmtl") {
            if (!material_name.empty()) {
                materials[material_index++] =
                    {{material.color.r * ns, material.color.g * ns,
                      material.color.b * ns}};
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
        materials[material_index++] =
            {{material.color.r * ns, material.color.g * ns,
              material.color.b * ns}};
        material_names[material_name] = index;
    }

    if (material_index == 0) {
        std::cout << "nothing to load from material file" << std::endl;
        return false;
    }

    return true;
}

static size_t count_scene_lights(dataset &dataset) {
    const char *data = dataset.scene;
    loader_stream stream((char *)data, strlen(data));
    std::istream is(&stream);

    size_t count = 0;
    for (std::string line; getline(is, line);) {
        size_t str_index;
        while ((str_index = line.find('\t')) != std::string::npos)
            line.erase(str_index, 1);

        std::vector<std::string> tokens = split(line, " ");
        tokens.erase(remove(tokens.begin(), tokens.end(), ""), tokens.end());

        if (tokens.size() == 4 && tokens[0] == "l")
            count++;
    }

    return count;
}

bool load_scene(dataset &dataset, scene &scene) {
    size_t lights_count = count_scene_lights(dataset);
    scene.lights = {(m3::vec3 *)allocate(lights_count * sizeof(m3::vec3)),
                    lights_count};

    const char *data = dataset.scene;
    loader_stream stream((char *)data, strlen(data));
    std::istream is(&stream);

    for (std::string line; getline(is, line);) {
        size_t str_index;
        while ((str_index = line.find('\t')) != std::string::npos)
            line.erase(str_index, 1);

        std::vector<std::string> tokens = split(line, " ");
        tokens.erase(remove(tokens.begin(), tokens.end(), ""), tokens.end());

        if (tokens.size() == 4 && tokens[0] == "l")
            scene.lights[--lights_count] =
                {stof(tokens[1]), stof(tokens[2]), stof(tokens[3])};

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

    size_t materials_count = count_scene_materials(dataset);
    scene.materials = {(material *)allocate(materials_count * sizeof(material)),
                       materials_count};

    std::map<std::string, size_t> material_names;
    if (!load_materials(dataset, scene.materials, material_names)) {
        std::cout << "failed to load materials for " << dataset.name
                  << std::endl;
        return false;
    }

    size_t objects_count = count_scene_objects(dataset);
    array<object_info> infos = count_scene_object_elements(
        dataset, objects_count);
    scene.objects =
        {(object *)allocate(infos.size() * sizeof(object)), infos.size()};

    for (size_t i = 0; i < infos.size(); i++) {
        std::cout << "Info " << infos[i].face_count << " " <<
            infos[i].vertex_count << std::endl;
    }

    for (size_t i = 0; i < infos.size(); i++) {
        object &obj = scene.objects[i];
        obj.faces = {(face *)allocate(infos[i].face_count * sizeof(face)),
                     infos[i].face_count};
        obj.normals = {(m3::vec3 *)allocate(infos[i].face_count * sizeof(m3::vec3)),
                       infos[i].face_count};
        obj.vertices = {(m3::vec3 *)allocate(infos[i].vertex_count * sizeof(m3::vec3)),
                        infos[i].vertex_count};
    }

    if (!load_objects(dataset, material_names, scene.objects)) {
        std::cout << "failed to load objects for " << dataset.name
                  << std::endl;
        return false;
    }

    return true;
}
