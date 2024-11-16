export module i3_config_old:parser_stack;

export {
    struct stack_entry {
        /* Just a pointer, not dynamically allocated. */
        char const *identifier;
        enum {
            STACK_STR = 0,
            STACK_LONG = 1,
        } type;
        union {
            char *str;
            long num;
        } val;
    };

    struct stack {
        stack_entry stack[10];
    };

    void clear_stack(stack & ctx);
    char const *get_string(stack & stack, char const *identifier);
    long get_long(stack & stack, char const *identifier);
    void push_long(stack & ctx, char const *identifier, long num);
    void push_string(stack & stack, char const *identifier, char *str);
    void push_string_append(stack & ctx, char const *identifier, char const *str);
}
