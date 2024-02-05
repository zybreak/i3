export module i3_config_base:base_parser;

import :base_resource_database;
import :base_config_applier;

export {
    enum class config_load_t {
        C_VALIDATE,
        C_LOAD,
        C_RELOAD,
    };

    class BaseParser {
       protected:
        BaseConfigApplier &applier;
        BaseResourceDatabase &resourceDatabase;

       public:
        explicit BaseParser(BaseConfigApplier &applier, BaseResourceDatabase &rd)
            : applier(applier), resourceDatabase(rd){};
        BaseParser() = delete;
        virtual ~BaseParser() = default;
        virtual void parse_file() = 0;
    };
}