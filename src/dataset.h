#pragma once

struct dataset {
    const char *name;
    const char *scene;
    const char *obj;
    const char *mtl;
};
#define DATASETS_SIZE 5
extern const dataset datasets[];
