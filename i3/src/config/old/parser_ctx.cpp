module i3_config_old;

parser_ctx::parser_ctx(OldParser *parser) {
    this->criteria_state = parser->applier.criteria_create(-1);
}

parser_ctx::~parser_ctx() {
    delete this->criteria_state;
}
