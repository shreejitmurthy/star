/* star.h - v0.3.2
   A single-header testing suite.

   USAGE:
        Define `STAR_NO_ENTRY` in *one* source file before including this header to disable `main()` hijacking:
            #define STAR_NO_ENTRY
            #include "star.h"
        Define `STAR_NO_COLOR` to disable ASCII coloring:
        Define `STAR_NON_FATAL` so failed assertions don't abort the test entirely.
        Define `STAR_VEROBSE` or `STAR_VERBOSE_ASSERTS` for per-assert pass output.
        
        See the README.md for all features.

   LICENSE:
       See end of file for license information.
       
*/

#ifndef STAR_TEST_H
#define STAR_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*star_test_func)();

typedef struct {
    const char *name;
    star_test_func func;
} _star_test_case;

static int _star_current_failed = 0;
static size_t _star_test_count = 0;
static _star_test_case _star_tests[256];

static size_t _star_asserts_total  = 0;
static size_t _star_asserts_failed = 0;

#ifdef STAR_NON_FATAL
static const int _star_fatal = 0;
#else
static const int _star_fatal = 1;
#endif

#if defined(STAR_VERBOSE_ASSERTS) || defined(STAR_VERBOSE)
static const int _star_verbose = 1;
#else
static const int _star_verbose = 0;
#endif


#if !defined(STAR_NO_COLOR)
#define _STAR_FAIL(format, ...)                             \
    do {                                                    \
        fprintf(stderr, "\033[1;31m[FAIL]\033[0m \033[2m%s:%d\033[0m: ",  \
                __FILE__, __LINE__);                        \
        fprintf(stderr, format "\n", ##__VA_ARGS__);        \
    } while (0)

#define _STAR_TEST_FAIL(format, ...)                        \
    do {                                                    \
        fprintf(stderr, "\033[1;31m[TEST FAILED]\033[0m "); \
        fprintf(stderr, format "\n", ##__VA_ARGS__);        \
    } while (0)

#define _STAR_PASS(format, ...)         printf("\033[1;32m[PASS]\033[0m " format "\n", ##__VA_ARGS__)
#define _STAR_TEST_PASS(format, ...)    printf("\033[1;32m[TEST PASSED]\033[0m " format "\n", ##__VA_ARGS__)
#define _STAR_SUMMARY(format, ...)      printf("\n\033[1mTechnical and Reliable Summary:\033[0m " format "\n", ##__VA_ARGS__)
#define _STAR_CUSTOM(msg) "\033[36m" msg "\033[0m"
#else
#define _STAR_FAIL(format, ...)                      \
    do {                                             \
        fprintf(stderr, "[FAIL] %s:%d: ",            \
                __FILE__, __LINE__);                 \
        fprintf(stderr, format "\n", ##__VA_ARGS__); \
    } while (0)

#define _STAR_TEST_FAIL(format, ...)                 \
    do {                                             \
        fprintf(stderr, "[TEST FAILed] ");           \
        fprintf(stderr, format "\n", ##__VA_ARGS__); \
    } while (0)

#define _STAR_PASS(format, ...)         printf("[PASS] " format "\n", ##__VA_ARGS__)
#define _STAR_TEST_PASS(format, ...)    printf("[TEST PASSED] " format "\n", ##__VA_ARGS__)
#define _STAR_SUMMARY(format, ...)      printf("\nTechnical and Reliable Summary: " format "\n", ##__VA_ARGS__)
#endif /* STAR_NO_COLOR */

// Test "Constructor"
#define TEST(name)                                                        \
    void name();                                                          \
    __attribute__((constructor))                                          \
    void register_##name() {                                              \
        _star_tests[_star_test_count++] = (_star_test_case){#name, name}; \
    }                                                                     \
    void name()

/* ASSERTS */
static inline int __star_nearly_equal(double a, double b) {
    if (a == b) return 1;
    double diff = fabs(a - b);
    double norm = fabs(a) + fabs(b);
    double scale = DBL_EPSILON * norm;
    if (scale < DBL_MIN) scale = DBL_MIN;
    return diff < scale;
}

static inline bool __assert_eq(double a, double b, bool negate) {
    _star_asserts_total++;

    bool equal = __star_nearly_equal(a, b);
    bool ok = negate ? !equal : equal;

    if (!ok) {
        _star_asserts_failed++;
        _star_current_failed = 1;
    }

    return ok;
}

static inline bool __assert_streq(char* a, char* b, bool negate) {
    _star_asserts_total++;

    bool equal = strcmp((a), (b)) == 0 ? true : false;
    bool ok = negate ? !equal : equal;

    if (!ok) {
        _star_asserts_failed++;
        _star_current_failed = 1;
    }

    return ok;
}

static inline bool __assert_kindaeq(double a, double b, double n, bool negate) {
    bool equal = (fabs((a) - (b)) <= n) ? true : false;
    bool ok = negate ? !equal : equal;

    if (!ok) {
        _star_asserts_failed++;
        _star_current_failed = 1;
    }

    return ok;
}

/* MACROS */
// Equality & Inequality
#define ASS_EQ(a, b)                                                      \
    do {                                                                  \
        if (!__assert_eq((double)(a), (double)(b), false)) {              \
            _STAR_FAIL("ASS_EQ(%s, %s) failed: %lf != %lf",               \
                       #a, #b, (double)(a), (double)(b));                 \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_EQ(%s, %s) passed: %lf == %lf",               \
                       #a, #b, (double)(a), (double)(b));                 \
        }                                                                 \
    } while (0)

#define ASS_EQM(a, b, m)                                                  \
    do {                                                                  \
        if (!__assert_eq((double)(a), (double)(b), false)) {              \
            _STAR_FAIL("ASS_EQM(%s, %s) %s", #a, #b, _STAR_CUSTOM(m));    \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_EQM(%s, %s) passed: %lf == %lf",              \
                       #a, #b, (double)(a), (double)(b));                 \
        }                                                                 \
    } while (0)

#define ASS_NEQ(a, b)                                                     \
    do {                                                                  \
        if (!__assert_eq((double)(a), (double)(b), true)) {               \
            _STAR_FAIL("ASS_NEQ(%s, %s) failed: %lf == %lf",              \
                       #a, #b, (double)(a), (double)(b));                 \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_NEQ(%s, %s) passed: %lf != %lf",              \
                       #a, #b, (double)(a), (double)(b));                 \
        }                                                                 \
    } while (0)

#define ASS_NEQM(a, b, m)                                                 \
    do {                                                                  \
        if (!__assert_eq((double)(a), (double)(b), true)) {               \
            _STAR_FAIL("ASS_NEQM(%s, %s) %s", #a, #b, _STAR_CUSTOM(m));   \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_NEQM(%s, %s) passed: %lf != %lf",             \
                       #a, #b, (double)(a), (double)(b));                 \
        }                                                                 \
    } while (0)

#define ASS_STREQ(a, b)                                                   \
    do {                                                                  \
        if (!__assert_streq(a, b, false)) {                               \
            _STAR_FAIL("ASS_STREQ(%s, %s) failed: %s != %s",              \
                       #a, #b, (a), (b));                                 \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_STREQ(%s, %s) passed: %s = %s",               \
                       #a, #b, (a), (b));                                 \
        }                                                                 \
    } while (0)

#define ASS_STREQM(a, b, m)                                               \
    do {                                                                  \
        if (!__assert_streq(a, b, false)) {                               \
            _STAR_FAIL("ASS_STREQM(%s, %s) %s",                           \
                #a, #b, _STAR_CUSTOM(m));                                 \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_STREQM(%s, %s) passed: %s = %s",              \
                       #a, #b, (a), (b));                                 \
        }                                                                 \
    } while (0)


#define ASS_STRNEQ(a, b)                                                  \
    do {                                                                  \
        if (!__assert_streq(a, b, true)) {                                \
            _STAR_FAIL("ASS_STREQ(%s, %s) failed: %s = %s",               \
                       #a, #b, (a), (b));                                 \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_STREQ(%s, %s) passed: %s != %s",              \
                       #a, #b, (a), (b));                                 \
        }                                                                 \
    } while (0)

#define ASS_STRNEQM(a, b, m)                                              \
    do {                                                                  \
        if (!__assert_streq(a, b, true)) {                                \
            _STAR_FAIL("ASS_STNREQM(%s, %s) %s",                          \
                #a, #b, _STAR_CUSTOM(m));                                 \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_STNREQM(%s, %s) passed: %s != %s",            \
                       #a, #b, (a), (b));                                 \
        }                                                                 \
    } while (0)

// Look at this song and dance I have to do... (easily preventable if I wasn't stubborn)
#define ASS_KINDAEQ(a, b, dptr)                                           \
    do {                                                                  \
        _star_asserts_total++;                                            \
        double n = 6.9;                                                   \
        if ((dptr) != NULL) {                                             \
            const double *tmp__ = (const double *)(dptr);                 \
            n = *tmp__;                                                   \
        }                                                                 \
        if (!__assert_kindaeq((a), (b), n, false)) {                      \
            _STAR_FAIL("ASS_KINDAEQ(%s, %s) failed: %lf !≈ %lf (degree %lf)", \
                       #a, #b, (double)(a), (double)(b), n);              \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_KINDAEQ(%s, %s) passed: %lf ≈ %lf (degree %lf)", \
                       #a, #b, (double)(a), (double)(b), n);              \
        }                                                                 \
    } while (0)

#define ASS_KINDAEQM(a, b, dptr, m)                                       \
    do {                                                                  \
        _star_asserts_total++;                                            \
        double n = 6.9;                                                   \
        if ((dptr) != NULL) {                                             \
            const double *tmp__ = (const double *)(dptr);                 \
            n = *tmp__;                                                   \
        }                                                                 \
        if (!__assert_kindaeq((a), (b), n, false)) {                      \
            _STAR_FAIL("ASS_KINDAEQM(%s, %s) %s",                         \
                #a, #b, _STAR_CUSTOM(m));                                 \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_KINDAEQM(%s, %s) passed: %lf ≈ %lf (degree %lf)", \
                       #a, #b, (double)(a), (double)(b), n);              \
        }                                                                 \
    } while (0)


#define ASS_KINDANEQ(a, b, dptr)                                          \
    do {                                                                  \
        _star_asserts_total++;                                            \
        double n = 6.9;                                                   \
        if ((dptr) != NULL) {                                             \
            const double *tmp__ = (const double *)(dptr);                 \
            n = *tmp__;                                                   \
        }                                                                 \
        if (__assert_kindaeq((a), (b), n, true)) {                        \
            _STAR_FAIL("ASS_KINDAEQ(%s, %s) failed: %lf ≈ %lf (degree %lf)", \
                       #a, #b, (double)(a), (double)(b), n);              \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_KINDAEQ(%s, %s) passed: %lf !≈ %lf (degree %lf)", \
                       #a, #b, (double)(a), (double)(b), n);              \
        }                                                                 \
    } while (0)

#define ASS_KINDANEQM(a, b, dptr, m)                                      \
    do {                                                                  \
        _star_asserts_total++;                                            \
        double n = 6.9;                                                   \
        if ((dptr) != NULL) {                                             \
            const double *tmp__ = (const double *)(dptr);                 \
            n = *tmp__;                                                   \
        }                                                                 \
        if (!__assert_kindaeq((a), (b), n, true)) {                       \
            _STAR_FAIL("ASS_KINDANEQM(%s, %s) %s",                        \
                #a, #b, _STAR_CUSTOM(m));                                 \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_KINDANEQM(%s, %s) passed: %lf ≈ %lf (degree %lf)", \
                       #a, #b, (double)(a), (double)(b), n);              \
        }                                                                 \
    } while (0)

#define ASS_TRUE(expr)                                                    \
    do {                                                                  \
        _star_asserts_total++;                                            \
        if (!(expr)) {                                                    \
            _STAR_FAIL("ASS_TRUE(%s) failed", #expr);                     \
            _star_asserts_failed++;                                       \
            _star_current_failed = 1;                                     \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_TRUE(%s) passed", #expr);                     \
        }                                                                 \
    } while (0)

#define ASS_TRUEM(expr, m)                                                \
    do {                                                                  \
        _star_asserts_total++;                                            \
        if (!(expr)) {                                                    \
            _STAR_FAIL("ASS_TRUE(%s) %s", #expr, _STAR_CUSTOM(m));        \
            _star_asserts_failed++;                                       \
            _star_current_failed = 1;                                     \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_TRUE(%s) passed", #expr);                     \
        }                                                                 \
    } while (0)

#define ASS_FALSE(expr)                                                   \
    do {                                                                  \
        _star_asserts_total++;                                            \
        if ((expr)) {                                                     \
            _STAR_FAIL("ASS_FALSE(%s) failed", #expr);                    \
            _star_asserts_failed++;                                       \
            _star_current_failed = 1;                                     \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_FALSE(%s) passed", #expr);                    \
        }                                                                 \
    } while (0)

#define ASS_FALSEM(expr, m)                                               \
    do {                                                                  \
        _star_asserts_total++;                                            \
        if ((expr)) {                                                     \
            _STAR_FAIL("ASS_FALSE(%s) %s", #expr, _STAR_CUSTOM(m));       \
            _star_asserts_failed++;                                       \
            _star_current_failed = 1;                                     \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_FALSE(%s) passed", #expr);                    \
        }                                                                 \
    } while (0)

#define ASS_IS(a, b)                                                      \
    do {                                                                  \
        _star_asserts_total++;                                            \
        if (memcmp(&(a), &(b), sizeof((a)))) {                            \
            _STAR_FAIL("ASS_IS(%s, %s) failed", #a, #b);                  \
            _star_asserts_failed++;                                       \
            _star_current_failed = 1;                                     \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_IS(%s, %s) passed", #a, #b);                  \
        }                                                                 \
    } while (0)

#define ASS_ISM(a, b, m)                                                  \
    do {                                                                  \
        _star_asserts_total++;                                            \
        if (memcmp(&(a), &(b), sizeof((a)))) {                            \
            _STAR_FAIL("ASS_IS(%s, %s) %s", #a, #b, _STAR_CUSTOM(m));     \
            _star_asserts_failed++;                                       \
            _star_current_failed = 1;                                     \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_IS(%s, %s) passed", #a, #b);                  \
        }                                                                 \
    } while (0)

#define ASS_ISNT(a, b)                                                    \
    do {                                                                  \
        _star_asserts_total++;                                            \
        if (!memcmp(&(a), &(b), sizeof((a)))) {                           \
            _STAR_FAIL("ASS_ISNT(%s, %s) failed", #a, #b);                \
            _star_asserts_failed++;                                       \
            _star_current_failed = 1;                                     \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_ISNT(%s, %s) passed", #a, #b);                \
        }                                                                 \
    } while (0)

#define ASS_ISNTM(a, b, m)                                                \
    do {                                                                  \
        _star_asserts_total++;                                            \
        if (!memcmp(&(a), &(b), sizeof((a)))) {                           \
            _STAR_FAIL("ASS_ISNT(%s, %s) %s", #a, #b, _STAR_CUSTOM(m));   \
            _star_asserts_failed++;                                       \
            _star_current_failed = 1;                                     \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_ISNT(%s, %s) passed", #a, #b);                \
        }                                                                 \
    } while (0)

// Null / None / Undefined
#define ASS_ISNULL(expr)                                                  \
    do {                                                                  \
        _star_asserts_total++;                                            \
        if ((expr) != NULL) {                                             \
            _STAR_FAIL("ASS_ISNULL(%s) failed", #expr);                   \
            _star_asserts_failed++;                                       \
            _star_current_failed = 1;                                     \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_ISNULL(%s) passed", #expr);                   \
        }                                                                 \
    } while (0)

#define ASS_ISNULLM(expr, m)                                              \
    do {                                                                  \
        _star_asserts_total++;                                            \
        if ((expr) != NULL) {                                             \
            _STAR_FAIL("ASS_ISNULL(%s) %s", #expr, _STAR_CUSTOM(m));      \
            _star_asserts_failed++;                                       \
            _star_current_failed = 1;                                     \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_ISNULL(%s) passed", #expr);                   \
        }                                                                 \
    } while (0)

#define ASS_ISNTNULL(expr)                                                \
    do {                                                                  \
        _star_asserts_total++;                                            \
        if ((expr) == NULL) {                                             \
            _STAR_FAIL("ASS_ISNTNULL(%s) failed", #expr);                 \
            _star_asserts_failed++;                                       \
            _star_current_failed = 1;                                     \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_ISNTNULL(%s) passed", #expr);                 \
        }                                                                 \
    } while (0)

#define ASS_ISNTNULLM(expr, m)                                            \
    do {                                                                  \
        _star_asserts_total++;                                            \
        if ((expr) == NULL) {                                             \
            _STAR_FAIL("ASS_ISNTNULL(%s) %s", #expr, _STAR_CUSTOM(m));    \
            _star_asserts_failed++;                                       \
            _star_current_failed = 1;                                     \
            if (_star_fatal) return;                                      \
        } else if (_star_verbose) {                                       \
            _STAR_PASS("ASS_ISNTNULL(%s) passed", #expr);                 \
        }                                                                 \
    } while (0)

/* Run Functionality */
#if defined(STAR_NO_ENTRY)
static inline int star_run(int o) {
    if (o) printf("\033[1mRunning %zu tests...\033[0m\n", _star_test_count);

    int passed_tests = 0;
    int failed_tests = 0;

    for (int i = 0; i < (int)_star_test_count; i++) {
        _star_current_failed = 0;

        size_t before_total  = _star_asserts_total;
        size_t before_failed = _star_asserts_failed;

        _star_tests[i].func();

        size_t test_total  = _star_asserts_total  - before_total;
        size_t test_failed = _star_asserts_failed - before_failed;
        size_t test_passed = test_total - test_failed;

        if (_star_current_failed) {
            _STAR_TEST_FAIL("%s: %zu/%zu assertions passed (%zu failed)", _star_tests[i].name, test_passed, test_total, test_failed);
            failed_tests++;
        } else {
            _STAR_TEST_PASS("%s: %zu/%zu assertions passed", _star_tests[i].name, test_passed, test_total);
            passed_tests++;
        }
    }

    size_t total_passed_asserts = _star_asserts_total - _star_asserts_failed;

    if (o) _STAR_SUMMARY("%d/%zu tests passed, %d failed " "(%zu/%zu assertions passed)", 
        passed_tests, _star_test_count, failed_tests, total_passed_asserts, _star_asserts_total);

    return failed_tests ? 1 : 0;
}
#else
int main(int argc, char** argv) {
    printf("\033[1mRunning %zu tests...\033[0m\n", _star_test_count);

    int passed_tests = 0;
    int failed_tests = 0;

    for (int i = 0; i < (int)_star_test_count; i++) {
        _star_current_failed = 0;

        size_t before_total  = _star_asserts_total;
        size_t before_failed = _star_asserts_failed;

        _star_tests[i].func();

        size_t test_total  = _star_asserts_total  - before_total;
        size_t test_failed = _star_asserts_failed - before_failed;
        size_t test_passed = test_total - test_failed;

        if (_star_current_failed) {
            _STAR_TEST_FAIL("%s: %zu/%zu assertions passed (%zu failed)", _star_tests[i].name, test_passed, test_total, test_failed);
            failed_tests++;
        } else {
            _STAR_TEST_PASS("%s: %zu/%zu assertions passed", _star_tests[i].name, test_passed, test_total);
            passed_tests++;
        }
    }

    size_t total_passed_asserts = _star_asserts_total - _star_asserts_failed;

    _STAR_SUMMARY("%d/%zu tests passed, %d failed " "(%zu/%zu assertions passed)", 
        passed_tests, _star_test_count, failed_tests, total_passed_asserts, _star_asserts_total);

    return failed_tests ? 1 : 0;
}
#endif /* STAR_NO_ENTRY */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* STAR_TEST_H */

/*
    Revision history:
        0.5.1  (2025-11-24)  Custom message color is now normal intensity cyan.   
        0.5.0  (2025-11-24)  Changes:
                                - 0.4.1: `STAR_VERBOSE` also works in addition to `STAR_VERBOSE_ASSERTS`.
                                - 0.4.2: Now using epsilon-based floating point comparison.
                                - 0.4.3: Bold coloring for color-enabled output on assert outcome (FAIL, PASS).
                                - 0.5.0: Support for custom messages on failed assertions. Custom messages have
                                         high intensity cyan color.
        0.4.0  (2025-11-23)  Many changes:
                                - 0.3.2: automatic file-line display in _STAR_FAIL + top-file description.
                                - 0.3.1: global test count is now size_t, user-irrelevant identifiers
                                         now prefixed with `_star`.
                                - 0.3.3: Non-fatal option to *not* abort after failing an assert.
                                - 0.3.4: Verbose asserts to indicate which asserts passed (even if test failed).
                                - 0.3.5: More informative output with passed tests and asserts.
                                - 0.4.0: Added object and null checking asserts.
        0.3.0  (2025-11-22)  Added more equality + bool asserts.                      
        0.2.0  (2025-11-22)  Created test running and two asserts. `main()` hijacking toggleable and
                             ASCII coloring also added.
        0.1.0  (2025-11-22)  First push.
*/

/*
    MIT License

    Copyright (c) 2025 Shreejit Murthy

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

*/
