export module i3_config_base:base_resource_database;

export class BaseResourceDatabase {
   public:

    virtual char* get_resource(char *name, const char *fallback) = 0;
    virtual ~BaseResourceDatabase() = default;
};
