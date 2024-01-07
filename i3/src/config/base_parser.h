#pragma once

#include "base_config_applier.h"

enum parse_file_result_t {
    PARSE_FILE_FAILED = -1,
    PARSE_FILE_SUCCESS = 0,
    PARSE_FILE_CONFIG_ERRORS = 1,
};

class BaseParser {
   protected:
    BaseConfigApplier &applier;
   public:
    explicit BaseParser(BaseConfigApplier &applier) : applier(applier) {};
    BaseParser() = delete;
    virtual ~BaseParser() = default;
    virtual parse_file_result_t parse_file() = 0;
};
