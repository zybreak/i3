#pragma once

#include "config/base_parser.h"
#include "../base_config_applier.h"
#include "../resource_database.h"

class NewParser : public BaseParser {
public:
    NewParser(const ResourceDatabase& resourceDatabase, std::istream *stream, config_load_t load_type, BaseConfigApplier &applier);
    parse_file_result_t parse_file() override;

    std::istream *stream;
    config_load_t load_type;
    ResourceDatabase resourceDatabase;
};