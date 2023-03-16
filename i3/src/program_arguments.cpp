#include "program_arguments.h"
#include "util.h"
#include "i3.h"
#include "root_atom_contents.h"

#include <getopt.h>

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
            {"get-socketpath", no_argument, nullptr, 0},
            {"get_socketpath", no_argument, nullptr, 0},
            {nullptr, 0, nullptr, 0}};
    int option_index = 0, opt;

    start_argv = argv;

    while ((opt = getopt_long(argc, argv, "pc:CvmaL:hld:V", long_options, &option_index)) != -1) {
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
                printf("i3 version %s © 2009 Michael Stapelberg and contributors\n", I3_VERSION);
                exit(EXIT_SUCCESS);
                break;
            case 'V':
                set_verbosity(true);
                break;
            case 'd':
                LOG("Enabling debug logging\n");
                set_debug_logging(true);
                break;
            case 0:
                if (strcmp(long_options[option_index].name, "disable-signalhandler") == 0) {
                    args.disable_signalhandler = true;
                    break;
                } else if (strcmp(long_options[option_index].name, "get-socketpath") == 0 ||
                           strcmp(long_options[option_index].name, "get_socketpath") == 0) {
                    char *socket_path = root_atom_contents("I3_SOCKET_PATH", nullptr, 0);
                    if (socket_path) {
                        printf("%s\n", socket_path);
                        /* With -O2 (i.e. the buildtype=debugoptimized meson
                         * option, which we set by default), gcc 9.2.1 optimizes
                         * away socket_path at this point, resulting in a Leak
                         * Sanitizer report. An explicit free helps: */
                        free(socket_path);
                        exit(EXIT_SUCCESS);
                    }

                    exit(EXIT_FAILURE);
                } else if (strcmp(long_options[option_index].name, "restart") == 0) {
                    args.layout_path = optarg;
                    args.delete_layout_path = true;
                    break;
                }
                /* fall-through */
            default:
                fprintf(stderr, "Usage: %s [-c configfile] [-d all] [-a] [-v] [-V] [-C]\n", argv[0]);
                fprintf(stderr, "\n");
                fprintf(stderr, "\t-a          disable autostart ('exec' lines in config)\n");
                fprintf(stderr, "\t-c <file>   use the provided configfile instead\n");
                fprintf(stderr, "\t-C          validate configuration file and exit\n");
                fprintf(stderr, "\t-p          use new configuration parser\n");
                fprintf(stderr, "\t-d all      enable debug output\n");
                fprintf(stderr, "\t-L <file>   path to the serialized layout during restarts\n");
                fprintf(stderr, "\t-v          display version and exit\n");
                fprintf(stderr, "\t-V          enable verbose mode\n");
                fprintf(stderr, "\n");
                fprintf(stderr, "\t--get-socketpath\n"
                                "\tRetrieve the i3 IPC socket path from X11, print it, then exit.\n");
                fprintf(stderr, "\n");
                fprintf(stderr, "If you pass plain text arguments, i3 will interpret them as a command\n"
                                "to send to a currently running i3 (like old i3-msg). This allows you to\n"
                                "use nice and logical commands, such as:\n"
                                "\n"
                                "\ti3 border none\n"
                                "\ti3 floating toggle\n"
                                "\ti3 kill window\n"
                                "\n");
                exit(opt == 'h' ? EXIT_SUCCESS : EXIT_FAILURE);
        }
    }

    return args;
}
