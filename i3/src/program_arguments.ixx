module;
#include <string>
export module i3:program_arguments;

export {
    // program arguments
    struct program_arguments {
        std::string override_configpath{};
        bool autostart{true};
        bool new_parser{false};
        std::string layout_path{};
        bool delete_layout_path{false};
        bool disable_signalhandler{false};
        bool only_check_config{false};
    };

    program_arguments parse_args(int argc, char *argv[]);
}