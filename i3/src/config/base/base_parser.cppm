module;
#include <cstdlib>
export module i3_config_base:base_parser;

import std;
import :base_resource_database;
import :base_config_applier;

export {
    enum class config_load_t {
        C_VALIDATE,
        C_LOAD,
        C_RELOAD,
    };

    /**
     * List entry struct for an included file.
     *
     */
    class IncludedFile {
       public:
        std::string path{};
        std::string raw_contents{};
        std::string variable_replaced_contents{};

        IncludedFile(const std::string path) : path(path) {}
    };

    class BaseParser {
       public:
        std::vector<IncludedFile> included_files{};
        BaseConfigApplier &applier;
        BaseResourceDatabase &resourceDatabase;
        explicit BaseParser(BaseConfigApplier &applier, BaseResourceDatabase &rd)
            : applier(applier), resourceDatabase(rd){};
        BaseParser() = delete;
        virtual ~BaseParser() = default;
        virtual void parse_file() = 0;
    };
}