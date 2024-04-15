#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sstream>
#include <strstream>
#include <iostream>

#include "config_applier_adapter.h"

import i3;
import i3_config_base;
import i3_config_new;

class MockX : public X {
   public:
    MockX() : X(nullptr) {
        root_depth = 32;
    };

    MOCK_METHOD(void, con_init, (Con *con), (override));
};

class MockedResourceDatabase : public BaseResourceDatabase {
    MOCK_METHOD(char*, get_resource, (char *name, const char *fallback), (override));
};

TEST(ConfigNewTest, Foo) {
    global.x = new MockX{};

    std::stringbuf output;
    std::ostream out{&output};
    ConfigApplierAdapter applier{out};

    MockedResourceDatabase resourceDatabase{};

    std::istrstream in{R""""(
foo
)""""};

    NewParser parser{in, resourceDatabase, config_load_t::C_LOAD, applier};
    try {
        parser.parse_file();
    } catch (std::exception &e) {
        FAIL() << "Error parsing configuration file" << e.what();
    }
}
