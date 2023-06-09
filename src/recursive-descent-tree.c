#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "recursive-descent-tree.h"

struct _rdt_node {
    RdtNode *child_a;
    RdtNode *child_b;
    RdtNode *child_c;
    RdtNode *parent;
    char *op;
    char *res;
    char *statement;
    char *val_a;
    char *val_b;
    char *val_c;
    int level;
};

RdtNode *
rdt_node_init (RdtNode * parent, int level) {
    RdtNode *node;

    node = (RdtNode*) calloc (1, sizeof (RdtNode));

    node->parent = parent;
    node->level = level;
    node->child_a = 0;
    node->child_b = 0;
    node->child_c = 0;
    node->op = 0;
    node->res = 0;
    node->statement = 0;
    node->val_a = 0;
    node->val_b = 0;
    node->val_c = 0;

    return node;
}

void
rdt_node_free (RdtNode * node) {
    if (!node)
        return;

    if (node->child_a)
        rdt_node_free (node->child_a);
    if (node->child_b)
        rdt_node_free (node->child_b);
    if (node->child_c)
        rdt_node_free (node->child_c);
    if (node->op)
        free (node->op);
    if (node->statement)
        free (node->statement);
    if (node->val_a)
        free (node->val_a);
    if (node->val_b)
        free (node->val_b);
    if (node->val_c)
        free (node->val_c);
    if (node->res)
        free (node->res);

    free (node);
    node = 0;
}

RdtNode *
rdt_node_add_child (RdtNode * node) {
    if (!node->child_a && !node->val_a) {
        node->child_a = rdt_node_init (node, node->level + 5);
        return node->child_a;
    }
    else if (!node->child_b && !node->val_b) {
        node->child_b = rdt_node_init (node, node->level + 5);
        return node->child_b;
    }
    else if (!node->child_c && !node->val_c) {
        node->child_c = rdt_node_init (node, node->level + 5);
        return node->child_c;
    }
    else
        return 0;
}

RdtNode *
rdt_node_get_parent (RdtNode * node) {
    return node->parent;
}

void
rdt_node_add_statement (RdtNode * node, char *statement) {
    node->statement = strdup (statement);
}

void
rdt_node_add_op (RdtNode * node, char *op) {
    node->op = strdup (op);
}

void
rdt_node_add_val (RdtNode * node, LispValue * lv) {
    int *i = 0;

    if (!node->val_a && !node->child_a) {
        node->val_a = (char*) calloc (30, sizeof (char));
        i = (int*)lisp_value_get (lv);
        sprintf (node->val_a, "%d", *i);
    }
    else if (!node->val_b && !node->child_b) {
        node->val_b = (char*) calloc (30, sizeof (char));
        i = (int*)lisp_value_get (lv);
        sprintf (node->val_b, "%d", *i);
    }
    else if (node->statement && !node->val_c && !node->child_c) {
        node->val_c = (char*) calloc (30, sizeof (char));
        i = (int*)lisp_value_get (lv);
        sprintf (node->val_c, "%d", *i);
    }
}

void rdt_node_add_res (RdtNode * node, LispValue * lv) {
    int *i = 0;

    node->res = (char*) calloc (30, sizeof (char));
    i = (int*)lisp_value_get (lv);
    sprintf (node->res, "%d", *i);
}

void
rdt_node_print_tree (RdtNode * node) {
    printf ("%*c\n", node->level, '(');

    if (node->statement)
        printf ("%*s\n", node->level + 5, node->statement);
    if (node->op)
        printf ("%*s\n", node->level + 5, node->op);

    if (node->child_a)
        rdt_node_print_tree (node->child_a);
    else if (node->val_a)
        printf ("%*s\n", node->level + 5, node->val_a);

    if (node->child_b)
        rdt_node_print_tree (node->child_b);
    else if (node->val_b)
        printf ("%*s\n", node->level + 5, node->val_b);

    if (node->child_c)
        rdt_node_print_tree (node->child_c);
    else if (node->val_c)
        printf ("%*s\n", node->level + 5, node->val_c);

    printf ("%*c -> %s\n", node->level, ')', node->res);
}
