#include "config_parser.h"
#include "config_adapter.h"
#include "test_resource_database.h"
#include <err.h>
#include <cstdlib>

import i3;

int fd;
char filename[] = "/tmp/i3-config-parser-test-XXXXXX";

bool parse_config(struct parser_ctx &ctx, const std::string &input, const char *filename);

/*******************************************************************************
 * Code for building the stand-alone binary test.commands_parser which is used
 * by t/187-commands-parser.t.
 ******************************************************************************/
int main(int argc, char *argv[]) {

    std::string content{};
    char c;

    while (std::cin.get(c)) {
        content += c;
    }

    if ((fd = mkstemp(filename)) == -1) {
        err(EXIT_FAILURE, "mkstemp");
    }

    std::atexit([]() {
        close(fd);
        unlink(filename);
    });

    if (write(fd, content.c_str(), content.size()) == -1) {
        err(EXIT_FAILURE, "write");
    }

    ConfigApplierAdapter applier{std::cout};
    TestResourceDatabase resourceDatabase{};

    OldParser p(filename, resourceDatabase, config_load_t::C_LOAD, applier);

    p.parse_file();
}