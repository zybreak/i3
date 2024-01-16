//
// Created by zybreak on 2024-01-07.
//
#pragma once

class BaseResourceDatabase {
   public:

    virtual char* get_resource(char *name, const char *fallback) = 0;
    virtual ~BaseResourceDatabase() = default;
};
