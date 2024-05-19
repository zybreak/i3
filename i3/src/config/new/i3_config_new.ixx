export module i3_config_new;

import std;
import i3_config_base;

export class NewParser : public BaseParser {
public:
    NewParser(std::string file, std::istream &stream, BaseResourceDatabase &resourceDatabase, config_load_t load_type, BaseConfigApplier &applier);
    void parse_file() override;

    std::istream &stream;
    config_load_t load_type;
};