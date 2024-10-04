export module i3:autostarts;

import std;

class ApplicationLauncher;

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

export void autostart(ApplicationLauncher &applicationLauncher);

export void autostart_add(std::unique_ptr<Autostart> autostart);

export void autostart_always_add(std::unique_ptr<Autostart> autostart);

export void autostart_always(ApplicationLauncher &applicationLauncher);
