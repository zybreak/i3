module;
#include <xcb/xcb_xrm.h>
#include <fmt/printf.h>
#include <cassert>
export module i3:resource_database;

import std;
import i3_config_base;
import log;

export class ResourceDatabase : public BaseResourceDatabase {
    xcb_xrm_database_t *database = nullptr;
    xcb_connection_t *conn;

  public:
    explicit ResourceDatabase(xcb_connection_t *conn)
        : conn(conn) {
        assert(conn != nullptr);
    }

    std::string get_resource(std::string_view const name, std::string_view const fallback) override {
        /* Load the resource database lazily. */
        if (database == nullptr) {
            database = xcb_xrm_database_from_default(conn);

            if (database == nullptr) {
                ELOG("Failed to open the resource database.\n");

                /* Load an empty database so we don't keep trying to load the
                 * default database over and over again. */
                database = xcb_xrm_database_from_string("");

                return fallback.data();
            }
        }

        char *resource;
        xcb_xrm_resource_get_string(database, name.data(), nullptr, &resource);

        if (resource == nullptr) {
            DLOG(fmt::sprintf("Could not get resource '%s', using fallback '%s'.\n", name, fallback));
            return fallback.data();
        }

        std::string resource_str = resource;

        free(resource);

        return resource_str;
    }

    ~ResourceDatabase() override {
        if (database != nullptr) {
            xcb_xrm_database_free(database);
        }
    }
};
