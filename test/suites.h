#ifndef _SUITES_H_
#define _SUITES_H_

typedef enum _test_suite {
    ALL,
    ENV,
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
TestArg test_a2 = {"(- 5 3)", 2};
TestArg test_a3 = {"(% 5 2)", 1};
TestArg test_a4 = {"(= 5 2)", 0};
TestArg test_a5 = {"(= 5 5)", 1};
TestArg test_a6 = {"(< 5 2)", 0};
TestArg test_a7 = {"(<= 5 2)", 0};
TestArg test_a8 = {"(<= 5 5)", 1};
TestArg test_a9 = {"(> 20 2)", 1};
TestArg test_a10 = {"(>= 20 2)", 1};
TestArg test_a11 = {"(>= 20 20)", 1};

/* testsvit nesting */
TestArg test_b1 = {"(- 22 (+ (* 2 2) 5))", 13};
TestArg test_b2 = {"(= (* 3 3) (+ 4 (- 19 14)))", 1};
TestArg test_b3 = {"(= (% 3 2) (+ 4 5))", 0};

/* testsvit statement */
TestArg test_c1 = {"(if 0 13 4)", 4};
TestArg test_c2 = {"(if 1 13 4)", 13};
TestArg test_c3 = {"(if (> 2 1) 9 5)", 9};
TestArg test_c4 = {"(if (= 2 2) (+ 9 50) 5)", 59};
TestArg test_c5 = {"(if (= 3 2) (+ 9 50) (* 3 4))", 12};
TestArg test_c6 = {"(if (<= 1 2) (if (= 1 1) 33 66) (* 3 4))", 33};

/* testsvit env */
TestArg test_d1 = {"((defvar test 99)(test))", 99};
TestArg test_d2 = {"((defvar test (+ 10 10))(test))", 20};
TestArg test_d3 = {"((defvar test (+ 10 10)) (+ test 6))", 26};
TestArg test_d4 = {"((defvar test (+ 10 1)) (+ test test))", 22};
TestArg test_d5 = {"((defvar x 9)(defvar y 2)(+ x y))", 11};

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
    &test_a11,
    0,
};

TestArg *suite_nesting[] = {
    &test_b1,
    &test_b2,
    &test_b3,
    0
};

TestArg *suite_statement[] = {
    &test_c1,
    &test_c2,
    &test_c3,
    &test_c4,
    &test_c5,
    &test_c6,
    0
};

TestArg *suite_env[] = {
    &test_d1,
    &test_d2,
    &test_d3,
    &test_d4,
    //&test_d5,
    0
};

#endif /* _SUITES_H_ */
