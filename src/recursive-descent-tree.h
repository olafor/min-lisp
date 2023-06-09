#ifndef _RECURSIVE_DESCENT_TREE_H_
#define _RECURSIVE_DESCENT_TREE_H_

#include "lisp-value.h"

typedef struct _rdt_node RdtNode;

RdtNode *rdt_node_init (RdtNode * parent, int level);

void rdt_node_free (RdtNode * node);

RdtNode *rdt_node_add_child (RdtNode * node);

RdtNode *rdt_node_get_parent (RdtNode * node);

void rdt_node_add_statement (RdtNode * node, char * statement);

void rdt_node_add_op (RdtNode * node, char * op);

void rdt_node_add_val (RdtNode * node, LispValue * lv);

void rdt_node_add_res (RdtNode * node, LispValue * lv);

void rdt_node_print_tree (RdtNode * node);

#endif /* _RECURSIVE_DESCENT_TREE_H_ */
