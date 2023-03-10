#include "autostarts.h"
#include <vector>
#include <memory>
#include "log.h"
#include <startup.h>

/* The list of exec-lines */
std::vector<std::unique_ptr<Autostart>> autostarts{};

/* The list of exec_always lines */
std::vector<std::unique_ptr<Autostart>> autostarts_always{};

void autostart() {
    while (!autostarts.empty()) {
        auto &exec = autostarts.front();

        LOG(fmt::sprintf("auto-starting %s\n",  exec->command));
        start_application(exec->command, exec->no_startup_id);

        autostarts.erase(autostarts.begin());
    }
}

void autostart_always() {
    while (!autostarts_always.empty()) {
        auto &exec_always = autostarts_always.front();

        LOG(fmt::sprintf("auto-starting (always!) %s\n",  exec_always->command));
        start_application(exec_always->command, exec_always->no_startup_id);

        autostarts_always.erase(autostarts_always.begin());
    }
}
