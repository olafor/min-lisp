#include <stdio.h>
#include <string.h>

#include "../src/min-lisp.h"
#include "../src/utils.h"
#include "suites.h"

int assert_expr_result (char *expr, int expected_result);

int run_suite (char * name, TestArg * suite[]);

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
        run_suite ("EVAL", suite_eval);
    }
    if (ts == ALL || ts == NESTING) {
        run_suite ("NESTING", suite_nesting);
    }
    if (0 && (ts == ALL || ts == STATEMENT)) {
        run_suite ("STATEMENT", suite_statement);
    }
}

int
assert_expr_result (char * expr, int expected_result) {
    Lisp *lisp;
    int result;
    int err;

    lisp = lisp_init ();

    result = lisp_run_expr (lisp, expr, &err);
     
    lisp_free (lisp);

    if (result != expected_result && !err) {
        printf ("ERROR: %s\n\texpected: %d\n\tresult: %d\n\n",
                expr, expected_result, result);
        return 0;
    }
    return 1;
}

int run_suite (char * name, TestArg * suite[]) {
    printf ("\nKör TestSvit %s\n", name);
    for (int i = 0; suite[i] != 0; ++i) {
        if (!assert_expr_result (suite[i]->expr, suite[i]->expected_result)) {
            printf ("\t>> TestSvit falerade!\n");
            return 0;
        }
    }
    printf ("\t>> TestSvit gick igenom\n");
    return 1;
}
