#ifndef BUILD_AUTOSTARTS_H
#define BUILD_AUTOSTARTS_H

#include <string>
#include <vector>
#include <memory>

/**
 * Holds a command specified by either an:
 * - exec-line
 * - exec_always-line
 * in the config (see src/config.c)
 *
 */
struct Autostart {
    /** Command, like in command mode */
    std::string command;
    /** no_startup_id flag for start_application(). Determines whether a
     * startup notification context/ID should be created. */
    bool no_startup_id;
};

void autostart();
void autostart_add(std::unique_ptr<Autostart> autostart);
void autostart_always_add(std::unique_ptr<Autostart> autostart);
void autostart_always();

#endif //BUILD_AUTOSTARTS_H
