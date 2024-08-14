#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

////////////////////////////////////////////////////////////////

class Base {
public:
	Base(std::string s) : s(s) {};
    std::string s{};
	virtual ~Base() = default;

};

class Obj : public Base
{
public:
	Obj(int i, std::string s) : Base(s), i(i) {}
	int i = 0;
};
/*
namespace nlohmann {
	template <>
	struct adl_serializer<Base> {
        static void to_json(json &j, Base const o) {
            j["_type"] = "base";
        }

        static void from_json(json const &j, Base o) {
            //j.at("_type").get<std::string>().c_str()).from_json(j, o);
        }
    };
}
 */
void to_json(json &j, Base const o) {
    j["s"] = o.s;
}

void from_json(json const &j, Base o) {
    o.s = j.at("s").get<std::string>();
}

Base from_json(json const &j) {
    return Base("1");
}

namespace nlohmann
{
template <>
struct adl_serializer<Base>
{
    static Base from_json(const json& j)
    {
        std::string v = j["s"];
        return Base{""};
    }

    // Here's the catch! You must provide a to_json method! Otherwise, you
    // will not be able to convert person to json, since you fully
    // specialized adl_serializer on that type
    static void to_json(json& j, Base p)
    {
        j["s"] = p.s;
    }
};
} // namespace nlohmann

TEST(JsonTest, Foo) {
    std::vector<Base> v{};
	v.push_back(Base("1"));
	v.push_back(Obj(5, "2"));
	v.push_back(Base("3"));
	v.push_back(Obj(10, "4"));

	json j = v;
    
    std::string s = j.dump();
    
    std::cout << j << std::endl;

    std::vector<Base> v2{};
    
    json j2 = json::parse(s);
    
    v2 = j2.template get<std::vector<Base>>();
    
    for (auto &o : v2) {
        std::cout << o.s << std::endl;
    }
}
