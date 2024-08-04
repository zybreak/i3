export module i3_config_new;

import std;
import i3_config_base;

export class NewParser : public BaseParser {
public:
    std::istream &stream;
    
    NewParser(std::string file, std::istream &stream, BaseResourceDatabase &resourceDatabase, BaseConfigApplier &applier);
    void parse_file() override;
};