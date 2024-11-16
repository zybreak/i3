module;
#include <nlohmann/json.hpp>
export module i3:dump_node;

class Con;

export nlohmann::json dump_node(Con* con, bool inplace_restart);
