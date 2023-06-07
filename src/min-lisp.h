#ifndef _MIN_LISP_H_
#define _MIN_LISP_H_

typedef struct _lisp Lisp;

Lisp *lisp_init (void);

void lisp_free (Lisp * lisp);

void lisp_print_rdt_tree (Lisp * lisp);

int lisp_run_expr (Lisp * lisp, char * expr, int * error);

#endif /* _MIN_LISP_H_ */
