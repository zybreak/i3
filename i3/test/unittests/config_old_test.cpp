#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <strstream>

#include "config_applier_adapter.h"

import i3;
import i3_config_base;
import i3_config_old;

using namespace std::literals;

class MockedResourceDatabase : public BaseResourceDatabase {
    MOCK_METHOD(char*, get_resource, (char *name, const char *fallback), (override));
};

TEST(ConfigOldTest, Foo) {
    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};
    MockedResourceDatabase resourceDatabase{};
    std::istrstream in{R""""(
font DejaVu Sans Mono 12
)""""};


    OldParser p("/tmp/foo", in, resourceDatabase, config_load_t::C_LOAD, applier);

    try {
        p.parse_file();
    } catch (std::exception &e) {
        FAIL() << "Error parsing configuration file" << e.what();
    }
}
