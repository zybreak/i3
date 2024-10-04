module;
#include <fmt/printf.h>
module i3;

import std;
import log;

/* The list of exec-lines */
static std::vector<std::unique_ptr<Autostart>> autostarts{};

/* The list of exec_always lines */
static std::vector<std::unique_ptr<Autostart>> autostarts_always{};

void autostart(ApplicationLauncher &applicationLauncher) {
    while (!autostarts.empty()) {
        auto &exec = autostarts.front();

        LOG(fmt::sprintf("auto-starting %s\n", exec->command));
        applicationLauncher.start_application(exec->command, exec->no_startup_id);

        autostarts.erase(autostarts.begin());
    }
}

void autostart_add(std::unique_ptr<Autostart> autostart) {
    autostarts.push_back(std::move(autostart));
}

void autostart_always_add(std::unique_ptr<Autostart> autostart) {
    autostarts_always.push_back(std::move(autostart));
}

void autostart_always(ApplicationLauncher &applicationLauncher) {
    while (!autostarts_always.empty()) {
        auto &exec_always = autostarts_always.front();

        LOG(fmt::sprintf("auto-starting (always!) %s\n", exec_always->command));
        applicationLauncher.start_application(exec_always->command, exec_always->no_startup_id);

        autostarts_always.erase(autostarts_always.begin());
    }
}
