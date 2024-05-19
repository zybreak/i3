module;
#include <fmt/printf.h>
export module i3:autostarts;

import std;
import log;
import :startup;

/**
 * Holds a command specified by either an:
 * - exec-line
 * - exec_always-line
 * in the config (see src/config.c)
 *
 */
export struct Autostart {
    /** Command, like in command mode */
    std::string command;
    /** no_startup_id flag for start_application(). Determines whether a
     * startup notification context/ID should be created. */
    bool no_startup_id;
};

/* The list of exec-lines */
static std::vector<std::unique_ptr<Autostart>> autostarts{};

/* The list of exec_always lines */
static std::vector<std::unique_ptr<Autostart>> autostarts_always{};

export void autostart() {
    while (!autostarts.empty()) {
        auto &exec = autostarts.front();

        LOG(fmt::sprintf("auto-starting %s\n", exec->command));
        start_application(exec->command, exec->no_startup_id);

        autostarts.erase(autostarts.begin());
    }
}

export void autostart_add(std::unique_ptr<Autostart> autostart) {
    autostarts.push_back(std::move(autostart));
}

export void autostart_always_add(std::unique_ptr<Autostart> autostart) {
    autostarts_always.push_back(std::move(autostart));
}

export void autostart_always() {
    while (!autostarts_always.empty()) {
        auto &exec_always = autostarts_always.front();

        LOG(fmt::sprintf("auto-starting (always!) %s\n", exec_always->command));
        start_application(exec_always->command, exec_always->no_startup_id);

        autostarts_always.erase(autostarts_always.begin());
    }
}
