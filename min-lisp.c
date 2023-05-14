#include <stdio.c>
#include <stdint.c>

struct _lisp {

};

Lisp *lisp_init (void) {
    Lisp *lisp;

    lisp = (Lisp*) calloc (1, sizeof (Lisp));

    return lisp;
}

void lisp_free (Lisp * lisp) {
    if (lisp == 0)
        return;

    free (lisp);
}

char
lisp_evaluate (Lisp * lisp, const char op, const char a, const char b) {

}

void
lisp_parse (Lisp * lisp, const char * c) {

}

void
lisp_run_expr (Lisp * lisp, const char * c) {

}
