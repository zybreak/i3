module i3_config_old;

import std;
import utils;

static int const EXIT_FAILURE = 1;

void clear_stack(stack &ctx) {
    for (auto &c : ctx.stack) {
        if (c.type == stack_entry::STACK_STR) {
            std::free(c.val.str);
        }
        c.identifier = nullptr;
        c.val.str = nullptr;
        c.val.num = 0;
    }
}

char const *get_string(stack &stack, char const *identifier) {
    for (auto &c : stack.stack) {
        if (c.identifier == nullptr) {
            break;
        }
        if (std::strcmp(identifier, c.identifier) == 0) {
            return c.val.str;
        }
    }
    return nullptr;
}

long get_long(stack &stack, char const *identifier) {
    for (auto &c : stack.stack) {
        if (c.identifier == nullptr) {
            break;
        }
        if (std::strcmp(identifier, c.identifier) == 0) {
            return c.val.num;
        }
    }

    return 0;
}

void push_long(stack &ctx, char const *identifier, long num) {
    for (auto &c : ctx.stack) {
        if (c.identifier != nullptr) {
            continue;
        }
        /* Found a free slot, let’s store it here. */
        c.identifier = identifier;
        c.val.num = num;
        c.type = stack_entry::STACK_LONG;
        return;
    }

    /* When we arrive here, the stack is full. This should not happen and
     * means there’s either a bug in this parser or the specification
     * contains a command with more than 10 identified tokens. */
    std::cerr << "BUG: config_parser stack full. This means either a bug "
              << "in the code, or a new command which contains more than "
              << "10 identified tokens."
              << std::endl;

    std::exit(EXIT_FAILURE);
}

/*
 * Pushes a string (identified by 'identifier') on the stack. We simply use a
 * single array, since the number of entries we have to store is very small.
 *
 */
void push_string(stack &stack, char const *identifier, char *str) {
    for (auto &c : stack.stack) {
        if (c.identifier != nullptr) {
            continue;
        }
        /* Found a free slot, let’s store it here. */
        c.identifier = identifier;
        c.val.str = str;
        c.type = stack_entry::STACK_STR;
        return;
    }

    /* When we arrive here, the stack is full. This should not happen and
     * means there’s either a bug in this parser or the specification
     * contains a command with more than 10 identified tokens. */
    std::cerr << "BUG: commands_parser stack full. This means either a bug "
              << "in the code, or a new command which contains more than "
              << "10 identified tokens."
              << std::endl;

    std::exit(EXIT_FAILURE);
}

/*
 * Pushes a string (identified by 'identifier') on the stack. We simply use a
 * single array, since the number of entries we have to store is very small.
 *
 */
void push_string_append(stack &ctx, char const *identifier, char const *str) {
    for (auto &c : ctx.stack) {
        if (c.identifier != nullptr &&
            std::strcmp(c.identifier, identifier) != 0) {
            continue;
        }
        if (c.identifier == nullptr) {
            /* Found a free slot, let’s store it here. */
            c.identifier = identifier;
            c.val.str = sstrdup(str);
            c.type = stack_entry::STACK_STR;
        } else {
            /* Append the value. */
            char *prev = c.val.str;
            sasprintf(&(c.val.str), "%s,%s", prev, str);
            std::free(prev);
        }
        return;
    }

    /* When we arrive here, the stack is full. This should not happen and
     * means there’s either a bug in this parser or the specification
     * contains a command with more than 10 identified tokens. */
    std::cerr << "BUG: config_parser stack full. This means either a bug "
              << "in the code, or a new command which contains more than "
              << "10 identified tokens."
              << std::endl;

    std::exit(EXIT_FAILURE);
}