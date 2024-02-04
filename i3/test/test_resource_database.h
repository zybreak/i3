#pragma once

#include "base_resource_database.h"
#include <cstring>

class TestResourceDatabase : public BaseResourceDatabase {
    char* get_resource(char *name, const char *fallback) override {
        return strdup(fallback);
    }
};
