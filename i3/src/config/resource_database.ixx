module;
#include <cstring>
#include <xcb/xcb_xrm.h>
#include <fmt/printf.h>
export module i3:resource_database;

import i3_config_base;
import log;

export class ResourceDatabase : public BaseResourceDatabase {
    xcb_xrm_database_t *database = nullptr;
    xcb_connection_t *conn;
   public:

    explicit ResourceDatabase(xcb_connection_t *conn) : conn(conn) {
    }

    char* get_resource(char *name, const char *fallback) override {
        if (conn == nullptr) {
            return nullptr;
        }

        /* Load the resource database lazily. */
        if (database == nullptr) {
            database = xcb_xrm_database_from_default(conn);

            if (database == nullptr) {
                ELOG("Failed to open the resource database.\n");

                /* Load an empty database so we don't keep trying to load the
                 * default database over and over again. */
                database = xcb_xrm_database_from_string("");

                return strdup(fallback);
            }
        }

        char *resource;
        xcb_xrm_resource_get_string(database, name, nullptr, &resource);

        if (resource == nullptr) {
            DLOG(fmt::sprintf("Could not get resource '%s', using fallback '%s'.\n",  name, fallback));
            resource = strdup(fallback);
        }

        return resource;
    }

    ~ResourceDatabase() override {
        if (database != nullptr) {
            xcb_xrm_database_free(database);
        }
    }
};
