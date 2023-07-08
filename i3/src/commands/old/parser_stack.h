#ifndef BUILD_PARSER_STACK_H
#define BUILD_PARSER_STACK_H

struct stack_entry {
    /* Just a pointer, not dynamically allocated. */
    const char *identifier;
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

void clear_stack(stack &ctx);
const char *get_string(stack &stack, const char *identifier);
long get_long(stack &stack, const char *identifier);
void push_long(stack &ctx, const char *identifier, long num);
void push_string(stack &stack, const char *identifier, char *str);
void push_string_append(stack &ctx, const char *identifier, const char *str);

#endif //BUILD_PARSER_STACK_H
