#include <gtest/gtest.h>
import i3;
import std;

using namespace std::string_literals;

TEST(UtilTest, addArgumentWithoutOptArgOrOptName) {
    std::vector<std::string> argv{"/bin/foo","-c","/etc/foo.conf","-v"};
    
    auto argv_new = add_argument(argv, "-a", nullptr, nullptr);

    auto concat = std::accumulate(
        argv_new.begin(),
        argv_new.end(),
        std::string{},
        [](const std::string& acc, const std::string& str) {
          return acc.empty() ? str : acc + ' ' + str;
        }
    );

    ASSERT_EQ(argv_new.size(), 5);
    ASSERT_EQ(concat, "/bin/foo -c /etc/foo.conf -v -a");
}

TEST(UtilTest, addArgumentOptName) {
    std::vector<std::string> argv{"/bin/foo","-c","/etc/foo.conf","-v"};
    
    auto argv_new = add_argument(argv, "-a", nullptr, "--add");
    
    auto concat = std::accumulate(
        argv_new.begin(),
        argv_new.end(),
        std::string{},
        [](const std::string& acc, const std::string& str) {
            return acc.empty() ? str : acc + ' ' + str;
        }
    );

    ASSERT_EQ(argv_new.size(), 5);
    ASSERT_EQ(concat, "/bin/foo -c /etc/foo.conf -v -a");
}

TEST(UtilTest, addArgumentOptValue) {
    std::vector<std::string> argv{"/bin/foo","-c","/etc/foo.conf","-v"};

    auto argv_new = add_argument(argv, "-a", "foo", nullptr);

    auto concat = std::accumulate(
            argv_new.begin(),
            argv_new.end(),
            std::string{},
            [](const std::string& acc, const std::string& str) {
                return acc.empty() ? str : acc + ' ' + str;
            }
    );

    ASSERT_EQ(argv_new.size(), 6);
    ASSERT_EQ(concat, "/bin/foo -c /etc/foo.conf -v -a foo");
}

TEST(UtilTest, replaceArgumentWithoutOptName) {
    std::vector<std::string> argv{"/bin/foo","-c","/etc/foo.conf","-v"};

    auto argv_new = add_argument(argv, "-c", "foo", nullptr);

    auto concat = std::accumulate(
            argv_new.begin(),
            argv_new.end(),
            std::string{},
            [](const std::string& acc, const std::string& str) {
                return acc.empty() ? str : acc + ' ' + str;
            }
    );

    ASSERT_EQ(argv_new.size(), 4);
    ASSERT_EQ(concat, "/bin/foo -v -c foo");
}

TEST(UtilTest, replaceArgument) {
    std::vector<std::string> argv{"/bin/foo","--config","/etc/foo.conf","-v"};

    auto argv_new = add_argument(argv, "-c", "foo", "--config");

    auto concat = std::accumulate(
            argv_new.begin(),
            argv_new.end(),
            std::string{},
            [](const std::string& acc, const std::string& str) {
                return acc.empty() ? str : acc + ' ' + str;
            }
    );

    ASSERT_EQ(argv_new.size(), 4);
    ASSERT_EQ(concat, "/bin/foo -v -c foo");
}
