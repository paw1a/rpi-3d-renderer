#pragma once

struct dataset {
    const char *name;
    const char *scene;
    const char *obj;
    const char *mtl;
};
#define DATASETS_SIZE 6
extern const dataset datasets[];
