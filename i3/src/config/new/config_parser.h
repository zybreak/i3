#pragma once

#include "configuration.h"
#include "criteria_state.h"
#include "config/base_parser.h"
#include "base_config_applier.h"
#include "resource_database.h"

void start_config_error_nagbar(bool has_errors);

class NewParser : public BaseParser {
private:
    BaseConfigApplier &applier;

public:
    NewParser(const ResourceDatabase& resourceDatabase, std::istream *stream, config_load_t load_type, BaseConfigApplier &applier);
    parse_file_result_t parse_file();

    std::istream *stream;
    config_load_t load_type;
    ResourceDatabase resourceDatabase;
};