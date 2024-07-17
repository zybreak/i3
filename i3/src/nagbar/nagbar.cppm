module;
#include <unistd.h>
#include <sys/types.h>
export module i3:nagbar;

import std;

using std::uint32_t;
using std::uint16_t;

export {
    enum class bar_type_t {
        TYPE_ERROR,
        TYPE_WARNING
    };

    class button_t {
       public:
        std::string label{};
        std::string action{};
        int16_t x{};
        uint16_t width{};
        bool terminal{false};

        button_t() = default;

        button_t(std::string label, std::string action, bool terminal) : label(std::move(label)), action(std::move(action)), terminal(terminal) {}
    };

    /**
     * Starts an i3-nagbar instance with the given parameters. Takes care of
     * handling SIGCHLD and killing i3-nagbar when i3 exits.
     *
     * The resulting PID will be stored in *nagbar_pid and can be used with
     * kill_nagbar() to kill the bar later on.
     *
     */
    void start_nagbar(pid_t *nagbar_pid,
                      std::vector<button_t> &buttons,
                      std::string &prompt,
                      std::string &pattern,
                      bar_type_t bar_type,
                      bool position_on_primary);

    /**
     * Kills the i3-nagbar process, if nagbar_pid != -1.
     *
     * If wait_for_it is set (restarting i3), this function will waitpid(),
     * otherwise, ev is assumed to handle it (reloading).
     *
     */
    void kill_nagbar(pid_t nagbar_pid, bool wait_for_it);
}