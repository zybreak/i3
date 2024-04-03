module;
#include <stdlib.h>
#include <cstring>
#include <string>
export module i3_config_base:base_parser;

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
        char *raw_contents = nullptr;
        std::string variable_replaced_contents{};

        IncludedFile(const std::string path) {
            this->path = path;
        }

        ~IncludedFile() {
            free(raw_contents);
        }
    };

    class BaseParser {
       public:
        BaseConfigApplier &applier;
        BaseResourceDatabase &resourceDatabase;
        explicit BaseParser(BaseConfigApplier &applier, BaseResourceDatabase &rd)
            : applier(applier), resourceDatabase(rd){};
        BaseParser() = delete;
        virtual ~BaseParser() = default;
        virtual void parse_file() = 0;
    };
}