/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * tree.c: Everything that primarily modifies the layout tree data structure.
 *
 */
#pragma once

#include <config.h>
#include "con.h"
#include <queue>

enum position_t {
    BEFORE,
    AFTER
};

template<class T>
class TreeNode {
   public:
    TreeNode<T> *parent{nullptr};
    TreeNode<T> *first_child{nullptr}, *last_child{nullptr};
    TreeNode<T> *prev_sibling{nullptr}, *next_sibling{nullptr};
    T data;
};

template<class T>
class Tree {
   protected:
    TreeNode<T> head, feet;
    void init() {
        head = TreeNode<T>{};
        feet = TreeNode<T>{};
        head.next_sibling = &feet;
        feet.prev_sibling = &head;
    }
   public:

    class iterator_base;
    class pre_order_iterator;
    class post_order_iterator;
    class children_iterator;
    class leaf_iterator;

    /// Base class for iterators, only pointers stored, no traversal logic.
    class iterator_base {
       public:
        typedef T                               value_type;
        typedef T*                              pointer;
        typedef T&                              reference;
        typedef size_t                          size_type;
        typedef ptrdiff_t                       difference_type;
        typedef std::bidirectional_iterator_tag iterator_category;

        iterator_base();
        iterator_base(TreeNode<T> *);

        T&             operator*() const;
        T*             operator->() const;

        /// Number of children of the node pointed to by the iterator.
        unsigned int number_of_children() const;

        children_iterator begin_children_iterator() const;
        children_iterator end_children_iterator() const;

        TreeNode<T> *node;
    };

    /// Depth-first iterator, first accessing the node, then its children.
    class pre_order_iterator : public iterator_base {
       public:
        pre_order_iterator();
        pre_order_iterator(TreeNode<T> *);

        bool    operator==(const pre_order_iterator&) const;
        bool    operator!=(const pre_order_iterator&) const;
        pre_order_iterator&  operator++();
        pre_order_iterator&  operator--();
        pre_order_iterator   operator++(int);
        pre_order_iterator   operator--(int);
        pre_order_iterator&  operator+=(unsigned int);
        pre_order_iterator&  operator-=(unsigned int);
    };

    /// Depth-first iterator, first accessing the children, then the node itself.
    class post_order_iterator : public iterator_base {
       public:
        post_order_iterator();
        post_order_iterator(TreeNode<T> *);

        bool    operator==(const post_order_iterator&) const;
        bool    operator!=(const post_order_iterator&) const;
        post_order_iterator&  operator++();
        post_order_iterator&  operator--();
        post_order_iterator   operator++(int);
        post_order_iterator   operator--(int);
        post_order_iterator&  operator+=(unsigned int);
        post_order_iterator&  operator-=(unsigned int);

        /// Set iterator to the first child as deep as possible down the tree.
        void descend_all();
    };

    /// Breadth-first iterator, using a queue
    class breadth_first_queued_iterator : public iterator_base {
       public:
        breadth_first_queued_iterator();
        breadth_first_queued_iterator(TreeNode<T> *);

        bool    operator==(const breadth_first_queued_iterator&) const;
        bool    operator!=(const breadth_first_queued_iterator&) const;
        breadth_first_queued_iterator&  operator++();
        breadth_first_queued_iterator   operator++(int);
        breadth_first_queued_iterator&  operator+=(unsigned int);

       private:
        std::queue<TreeNode<T> *> traversal_queue;
    };

    /// Iterator which traverses only the nodes which are siblings of each other.
    class children_iterator : public iterator_base {
       public:
        children_iterator();
        children_iterator(TreeNode<T> *);

        bool    operator==(const children_iterator&) const;
        bool    operator!=(const children_iterator&) const;
        children_iterator&  operator++();
        children_iterator&  operator--();
        children_iterator   operator++(int);
        children_iterator   operator--(int);
        children_iterator&  operator+=(unsigned int);
        children_iterator&  operator-=(unsigned int);

        TreeNode<T> *range_first() const;
        TreeNode<T> *range_last() const;
        TreeNode<T> *parent_;
       private:
        void set_parent_();
    };

    /// Iterator which traverses only the leaves.
    class leaf_iterator : public iterator_base {
       public:
        leaf_iterator();
        leaf_iterator(TreeNode<T> *, TreeNode<T> *top);

        bool    operator==(const leaf_iterator&) const;
        bool    operator!=(const leaf_iterator&) const;
        leaf_iterator&  operator++();
        leaf_iterator&  operator--();
        leaf_iterator   operator++(int);
        leaf_iterator   operator--(int);
        leaf_iterator&  operator+=(unsigned int);
        leaf_iterator&  operator-=(unsigned int);
       private:
        TreeNode<T> *top_node;
    };

    Tree() {
        init();
    }
    pre_order_iterator insert_after(const iterator_base& position, const T& x) {
    }
    pre_order_iterator insert_before(const iterator_base& position, const T& x) {
        if(position.node==nullptr) {
            position.node=feet; // Backward compatibility: when calling insert_before on a null node,
                                   // insert_before before the feet.
        }
        auto tmp = new TreeNode<T>();
        tmp->data = new T(x);
        tmp->first_child=nullptr;
        tmp->last_child=nullptr;

        tmp->parent=position.node->parent;
        tmp->next_sibling=position.node;
        tmp->prev_sibling=position.node->prev_sibling;
        position.node->prev_sibling=tmp;

        if(tmp->prev_sibling==nullptr) {
            if(tmp->parent) {  // when inserting nodes at the head, there is no parent
                tmp->parent->first_child = tmp;
            }
        } else {
            tmp->prev_sibling->next_sibling=tmp;
        }
        return pre_order_iterator(tmp);
    }
    pre_order_iterator append_child(const iterator_base& position, const T& x) {
    }
    pre_order_iterator prepend_child(const iterator_base& position, const T& x) {
    }
    pre_order_iterator set_root(const T& x) {
        return insert_before(pre_order_iterator(feet), x);
    }
    inline pre_order_iterator root() const {
        return pre_order_iterator(head->next_sibling);
    }
    void clear() {
        if(head) {
            while(head->next_sibling!=feet) {
                erase(pre_order_iterator(head->next_sibling));
            }
        }
    }
    template<class iter>
    iter erase(iter it) {
        TreeNode<T> *cur=it.node;
        assert(cur!=head);
        iter ret=it;
        ++ret;

        erase_children(it);

        if(cur->prev_sibling==0) {
            cur->parent->first_child=cur->next_sibling;
        } else {
            cur->prev_sibling->next_sibling=cur->next_sibling;
        }

        if(cur->next_sibling==0) {
            cur->parent->last_child=cur->prev_sibling;
        } else {
            cur->next_sibling->prev_sibling=cur->prev_sibling;
        }

        delete cur->data;
        delete cur;
        return ret;
    }

    void erase_children(const iterator_base& it) {
        if(it.node==0) return;

        TreeNode<T> *cur=it.node->first_child;
        TreeNode<T> *prev=0;

        while(cur!=0) {
            prev=cur;
            cur=cur->next_sibling;
            erase_children(pre_order_iterator(prev));
            delete prev->data;
            delete prev;
        }
        it.node->first_child=0;
        it.node->last_child=0;
    }
};

/**
 * Initializes the tree by creating the root node, adding all RandR outputs
 * to the tree (that means randr_init() has to be called before) and
 * assigning a workspace to each RandR output.
 *
 */
void tree_init(const xcb_get_geometry_reply_t *geometry);

/**
 * Opens an empty container in the current container
 *
 */
Con *tree_open_con(Con *con, i3Window *window);

/**
 * Splits (horizontally or vertically) the given container by creating a new
 * container which contains the old one and the future ones.
 *
 */
void tree_split(Con *con, orientation_t orientation);

/**
 * Moves focus one level up. Returns true if focus changed.
 *
 */
bool level_up();

/**
 * Moves focus one level down. Returns true if focus changed.
 *
 */
bool level_down();

/**
 * Renders the tree, that is rendering all outputs using render_con() and
 * pushing the changes to X11 using x_push_changes().
 *
 */
void tree_render();

/**
 * Changes focus in the given direction
 *
 */
void tree_next(Con *con, direction_t direction);

/**
 * Get the previous / next sibling
 *
 */
Con *get_tree_next_sibling(Con *con, position_t direction);

/**
 * Closes the given container including all children.
 * Returns true if the container was killed or false if just WM_DELETE was sent
 * and the window is expected to kill itself.
 *
 * The dont_kill_parent flag is specified when the function calls itself
 * recursively while deleting a containers children.
 *
 */
bool tree_close_internal(Con *con, kill_window_t kill_window, bool dont_kill_parent);

/**
 * Loads tree from ~/.i3/_restart.json (used for in-place restarts).
 *
 */
bool tree_restore(const std::string_view  path, const xcb_get_geometry_reply_t *geometry);

/**
 * tree_flatten() removes pairs of redundant split containers, e.g.:
 *       [workspace, horizontal]
 *   [v-split]           [child3]
 *   [h-split]
 * [child1] [child2]
 * In this example, the v-split and h-split container are redundant.
 * Such a situation can be created by moving containers in a direction which is
 * not the orientation of their parent container. i3 needs to create a new
 * split container then and if you move containers this way multiple times,
 * redundant chains of split-containers can be the result.
 *
 */
void tree_flatten(Con *child);
