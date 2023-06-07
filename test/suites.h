#ifndef _SUITES_H_
#define _SUITES_H_

typedef enum _test_suite {
    ALL,
    EVAL,
    NESTING,
    STATEMENT
} TestSuite;

typedef struct _test_args {
    char *expr;
    int expected_result;
} TestArg;

/* HÄR FÖLJER SAMTLIGA TESTFALL */

/* testsvit eval */
TestArg test_a1 = {"(+ 2 3)", 5};
TestArg test_a2 = {"(% 5 2)", 1};
TestArg test_a3 = {"(= 5 2)", 0};
TestArg test_a4 = {"(= 5 5)", 1};
TestArg test_a5 = {"(< 5 2)", 0};
TestArg test_a6 = {"(<= 5 2)", 0};
TestArg test_a7 = {"(<= 5 5)", 1};
TestArg test_a8 = {"(> 20 2)", 1};
TestArg test_a9 = {"(>= 20 2)", 1};
TestArg test_a10 = {"(>= 20 2)", 1};

/* testsvit nesting */
TestArg test_b1 = {"(- 22 (+ (* 2 2) 5))", 13};

/* testsvit statement */
TestArg test_c2 = {"(if (> 2 1) 9 5)", 9};

/* HÄR FÖLJER TESTSVITERNA */

TestArg *suite_eval[] = {
    &test_a1,
    &test_a2,
    &test_a3,
    &test_a4,
    &test_a5,
    &test_a6,
    &test_a7,
    &test_a8,
    &test_a9,
    &test_a10,
    0,
};

TestArg *suite_nesting[] = {
    &test_b1,
    0
};

TestArg *suite_statement[] = {
    &test12,
    0
};

#endif /* _SUITES_H_ */
