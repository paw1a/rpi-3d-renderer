#!/usr/bin/env python3

import os
import re

models_dir = "models"
regex = re.compile('(.*scene)')

scenes = []
names = []
for root, dirs, files in os.walk(models_dir):
    for file in files:
        if regex.match(file):
            f = open(os.path.join("models", file), 'r')
            scenes.append(f.read())
            name = file.split('.')[0]
            names.append(name)
            print(name)
            f.close()

cpp_file = open('src/dataset.cpp', 'w')
h_file = open('src/dataset.h', 'w')

h_declaration = '''#pragma once

struct dataset {
    const char *name;
    const char *scene;
    const char *obj;
    const char *mtl;
};
'''
h_file.write(h_declaration)
h_file.write(f'#define DATASETS_SIZE {len(scenes)}\n')
h_file.write('extern const dataset datasets[];\n')

cpp_file.write('#include \"dataset.h\"\n')
cpp_file.write('const dataset datasets[] = {\n')

for i in range(len(scenes)):
    scene = scenes[i]
    name = names[i]

    lines = scene.split('\n')
    obj_path = None
    mtl_path = None
    for line in lines:
        line = line.strip()
        if len(line) != 0:
            if line[0] == 'o':
                obj_path = line.split(' ')[1]
            if line[0] == 'm':
                mtl_path = line.split(' ')[1]

    if obj_path is None or mtl_path is None:
        print('.obj or .mtl files not found in scene description')

    obj_file = open(obj_path, 'r')
    obj = obj_file.read()
    obj_file.close()
    mtl_file = open(mtl_path, 'r')
    mtl = mtl_file.read()
    mtl_file.close()

    cpp_file.write(f'{{.name = \"{name}\", .scene = R\"X({scene})X\",'
                   f'.obj = R\"X({obj})X\", .mtl = R\"X({mtl})X\"}},')

cpp_file.write('};\n')
