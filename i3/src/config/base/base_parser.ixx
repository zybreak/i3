module;
#include <stdlib.h>
#include <cstring>
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
        char *path = nullptr;
        char *raw_contents = nullptr;
        char *variable_replaced_contents = nullptr;

        IncludedFile(const char *path) {
            this->path = strdup(path);
        }

        ~IncludedFile() {
            free(path);
            free(raw_contents);
            free(variable_replaced_contents);
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