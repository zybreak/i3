export module i3_config_base:base_resource_database;

import std;

export class BaseResourceDatabase {
   public:

    virtual std::string get_resource(std::string_view const name, std::string_view const fallback) = 0;
    virtual ~BaseResourceDatabase() = default;
};
