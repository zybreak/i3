#pragma once

#include <xcb/xcb_xrm.h>
#include <fmt/printf.h>

import i3;

class ResourceDatabase {
    xcb_xrm_database_t *database = nullptr;
    xcb_connection_t *conn;
   public:

    explicit ResourceDatabase(xcb_connection_t *conn) : conn(conn) {
    }

    std::string get_resource(std::string &name, std::string &fallback) {
        /* Load the resource database lazily. */
        if (database == nullptr) {
            database = xcb_xrm_database_from_default(conn);

            if (database == nullptr) {
                ELOG("Failed to open the resource database.\n");

                /* Load an empty database so we don't keep trying to load the
                 * default database over and over again. */
                database = xcb_xrm_database_from_string("");

                return fallback;
            }
        }

        char *resource;
        xcb_xrm_resource_get_string(database, name.c_str(), nullptr, &resource);

        if (resource == nullptr) {
            DLOG(fmt::sprintf("Could not get resource '%s', using fallback '%s'.\n",  name, fallback));
            return fallback;
        }

        return resource;
    }

    ~ResourceDatabase() {
        if (database) {
            xcb_xrm_database_free(database);
        }
    }
};
