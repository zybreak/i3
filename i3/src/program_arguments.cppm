module;
#include <config.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <getopt.h>
export module program_arguments;

import std;
import log;
import i3;

export {
    // program arguments
    struct program_arguments {
        std::optional<std::filesystem::path> override_configpath{};
        bool autostart{true};
        bool new_parser{false};
        std::optional<std::filesystem::path> layout_path{};
        bool delete_layout_path{false};
        bool disable_signalhandler{false};
        bool only_check_config{false};
        bool replace_wm{false};
    };

    program_arguments parse_args(int argc, char *argv[]);
}

program_arguments parse_args(int argc, char *argv[]) {
    program_arguments args{};

    static struct option long_options[] = {
        {"no-autostart", no_argument, nullptr, 'a'},
        {"config", required_argument, nullptr, 'c'},
        {"version", no_argument, nullptr, 'v'},
        {"help", no_argument, nullptr, 'h'},
        {"layout", required_argument, nullptr, 'L'},
        {"restart", required_argument, nullptr, 0},
        {"disable-signalhandler", no_argument, nullptr, 0},
        {"user-new-parser", no_argument, nullptr, 'p'},
        {"get_socketpath", no_argument, nullptr, 0},
        {"replace", no_argument, nullptr, 'r'},
        {nullptr, 0, nullptr, 0}};
    int option_index = 0, opt;

    while ((opt = getopt_long(argc, argv, "pc:CvmaL:hld:Vr", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'a':
                LOG("Autostart disabled using -a\n");
                args.autostart = false;
                break;
            case 'L':
                args.layout_path = optarg;
                args.delete_layout_path = false;
                break;
            case 'c':
                args.override_configpath = optarg;
                break;
            case 'p':
                args.new_parser = true;
                break;
            case 'C':
                LOG("Checking configuration file only (-C)\n");
                args.only_check_config = true;
                break;
            case 'v':
                std::printf("i3 version %s © 2009 Michael Stapelberg and contributors\n", I3_VERSION);
                std::exit(EXIT_SUCCESS);
                break;
            case 'V':
                set_verbosity(true);
                break;
            case 'd':
                LOG("Enabling debug logging\n");
                set_debug_logging(true);
                break;
            case 'r':
                args.replace_wm = true;
                break;
            case 0:
                if (strcmp(long_options[option_index].name, "disable-signalhandler") == 0) {
                    args.disable_signalhandler = true;
                    break;
                } else if (strcmp(long_options[option_index].name, "get-socketpath") == 0 ||
                           strcmp(long_options[option_index].name, "get_socketpath") == 0) {
                    std::optional<std::string> socket_path = root_atom_contents("I3_SOCKET_PATH", nullptr, 0);
                    if (socket_path) {
                        std::printf("%s\n", socket_path->c_str());
                        std::exit(EXIT_SUCCESS);
                    }

                    std::exit(EXIT_FAILURE);
                } else if (strcmp(long_options[option_index].name, "restart") == 0) {
                    args.layout_path = optarg;
                    args.delete_layout_path = true;
                    break;
                }
                /* fall-through */
            default:
                std::fprintf(stderr, "Usage: %s [-c configfile] [-d all] [-a] [-v] [-V] [-C]\n", argv[0]);
                std::fprintf(stderr, "\n");
                std::fprintf(stderr, "\t-a          disable autostart ('exec' lines in config)\n");
                std::fprintf(stderr, "\t-c <file>   use the provided configfile instead\n");
                std::fprintf(stderr, "\t-C          validate configuration file and exit\n");
                std::fprintf(stderr, "\t-p          use new configuration parser\n");
                std::fprintf(stderr, "\t-d all      enable debug output\n");
                std::fprintf(stderr, "\t-L <file>   path to the serialized layout during restarts\n");
                std::fprintf(stderr, "\t-v          display version and exit\n");
                std::fprintf(stderr, "\t-V          enable verbose mode\n");
                std::fprintf(stderr, "\t--replace\n"
                                "\tReplace an existing window manager.\n");
                std::fprintf(stderr, "\n");
                std::fprintf(stderr, "\n");
                std::fprintf(stderr, "\t--get-socketpath\n"
                                "\tRetrieve the i3 IPC socket path from X11, print it, then exit.\n");
                std::fprintf(stderr, "\n");
                std::fprintf(stderr, "If you pass plain text arguments, i3 will interpret them as a command\n"
                                "to send to a currently running i3 (like old i3-msg). This allows you to\n"
                                "use nice and logical commands, such as:\n"
                                "\n"
                                "\ti3 border none\n"
                                "\ti3 floating toggle\n"
                                "\ti3 kill window\n"
                                "\n");
                std::exit(opt == 'h' ? EXIT_SUCCESS : EXIT_FAILURE);
        }
    }

    return args;
}
