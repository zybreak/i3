module i3_config_old;

parser_ctx::parser_ctx(OldParser *parser) : parser(parser) {
    this->criteria_state = parser->applier.criteria_create(-1);
}