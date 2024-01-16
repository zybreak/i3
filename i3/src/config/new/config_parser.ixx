module;
#include "base_parser.h"
#include "base_config_applier.h"
#include "base_resource_database.h"
export module i3_config_new;

export class NewParser : public BaseParser {
public:
    NewParser(BaseResourceDatabase &resourceDatabase, std::istream *stream, config_load_t load_type, BaseConfigApplier &applier);
    void parse_file() override;

    std::istream *stream;
    config_load_t load_type;
};