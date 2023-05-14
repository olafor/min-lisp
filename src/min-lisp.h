#ifndef _MIN_LISP_H_
#define _MIN_LISP_H_

typedef struct _lisp Lisp;

Lisp *lisp_init (void);

void lisp_free (Lisp * lisp);

int lisp_run_expr (Lisp * lisp, const char * expr, int * error);

#endif /* _MIN_LISP_H_ */
