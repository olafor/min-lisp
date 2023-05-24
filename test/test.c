#include <stdio.h>
#include <string.h>

#include "../src/utils.h"
#include "../src/min-lisp.h"

typedef enum _test_suite {
    ALL,
    EVAL,
    NESTING,
    STATEMENT
} TestSuite;

void assert_expr_result (char *expr, int expected_result);

int
main (int argc, char *argv[]) {
    TestSuite ts = ALL;

    if (argc == 2) {
        if (strcmp (argv[1], "eval") == 0)
            ts = EVAL;
        else if (strcmp (argv[1], "nesting") == 0)
            ts = NESTING;
        else if (strcmp (argv[1], "statement") == 0)
            ts = STATEMENT;
    }

    if (ts == ALL || ts == EVAL) {
        assert_expr_result ("(+ 2 3)", 5);
        assert_expr_result ("(% 5 2)", 1);
        assert_expr_result ("(= 5 2)", 0);
        assert_expr_result ("(= 5 5)", 1);
        assert_expr_result ("(< 5 2)", 0);
        assert_expr_result ("(<= 5 2)", 0);
        assert_expr_result ("(<= 5 5)", 1);
        assert_expr_result ("(> 20 2)", 1);
        assert_expr_result ("(>= 20 2)", 1);
        assert_expr_result ("(>= 2 2)", 1);
    }
    if (ts == ALL || ts == NESTING) {
        assert_expr_result ("(- 22 (+ (* 2 2) 5))", 13);
    }
    if (0 && (ts == ALL || ts == STATEMENT)) {
        assert_expr_result ("(if (> 2 1) 9 5)", 9);
    }
}

void
assert_expr_result (char * expr, int expected_result) {
    Lisp *lisp;
    int result;
    int err;

    lisp = lisp_init ();

    result = lisp_run_expr (lisp, expr, &err);
     
    lisp_free (lisp);

    printf ("<< TEST %s >>\n",
            (result == expected_result && !err) ? "SUCCEEDED" : "FAILED");
    printf ("   expected: %d\n     result: %d\n\n", expected_result, result);
    if (err)
        printf ("error code: %d\n", err);
}

