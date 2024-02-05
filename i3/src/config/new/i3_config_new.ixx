module;
#include <iostream>
export module i3_config_new;

import i3_config_base;

export class NewParser : public BaseParser {
public:
    NewParser(BaseResourceDatabase &resourceDatabase, std::istream *stream, config_load_t load_type, BaseConfigApplier &applier);
    void parse_file() override;

    std::istream *stream;
    config_load_t load_type;
};