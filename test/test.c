#include <stdio.h>
#include <string.h>

#include "../src/min-lisp.h"

void assert_expr_result (const char *expr, int expected_result);

int
main () {
    assert_expr_result ("(+ 2 3)", 5);
    assert_expr_result ("(% 5 2)", 1);
    assert_expr_result ("(< 5 2)", 0);
    assert_expr_result ("(> 5 2)", 1);
    assert_expr_result ("(- 9 (+ (* 2 2) 3))", 2);
}

void
assert_expr_result (const char * expr, int expected_result) {
    Lisp *lisp;
    int result;
    int err;

    lisp = lisp_init ();

    result = lisp_run_expr (lisp, expr, &err);
     
    lisp_free (lisp);

    printf ("<< TEST %s >>\n\n",
            (result == expected_result && !err) ? "SUCCEEDED" : "FAILED");
}

