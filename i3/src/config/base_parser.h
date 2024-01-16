#pragma once

#include "base_config_applier.h"

class BaseParser {
   protected:
    BaseConfigApplier &applier;
   public:
    explicit BaseParser(BaseConfigApplier &applier) : applier(applier) {};
    BaseParser() = delete;
    virtual ~BaseParser() = default;
    virtual void parse_file() = 0;
};
