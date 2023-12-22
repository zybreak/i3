/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * move.c: Moving containers into some direction.
 *
 */
module;

#include <config.h>

export module i3:move;

export {

/**
 * Moves the given container in the given direction
 *
 */
void tree_move(Con *con, direction_t direction);

/**
 * This function detaches 'con' from its parent and inserts it either before or
 * after 'target'.
 *
 */
void insert_con_into(Con *con, Con *target, position_t position);
}