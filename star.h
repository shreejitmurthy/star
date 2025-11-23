/* star.h - v0.3.2
   A single-header testing suite.

   USAGE:
        Define `STAR_NO_ENTRY` in *one* source file before including this header to disable `main()` hijacking:
            #define STAR_NO_ENTRY
            #include "star.h"
        Define `STAR_NO_COLOR` in *one* source file before including this header to disable ASCII coloring:
            #define STAR_NO_COLOR
            #include "star.h"
        
        See the README.md for all features.

   LICENSE:
       See end of file for license information.
       
*/

#ifndef STAR_TEST_H
#define STAR_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*star_test_func)();

typedef struct {
    const char *name;
    star_test_func func;
} _star_test_case;

static _star_test_case _star_tests[256];
static size_t _star_test_count = 0;

#if !defined(STAR_NO_COLOR)
#define _STAR_FAIL(format, ...)                          \
    do {                                                 \
        fprintf(stderr, "\033[31m[FAIL]\033[0m %s:%d: ", \
                __FILE__, __LINE__);                     \
        fprintf(stderr, format, ##__VA_ARGS__);          \
    } while (0)

#define _STAR_PASS(format, ...)    printf("\033[32m[PASS]\033[0m " format, ##__VA_ARGS__)
#define _STAR_SUMMARY(format, ...) printf("\033[1m\nTechnical and Reliable Summary:\033[0m " format, ##__VA_ARGS__)
#else
#define _STAR_FAIL(format, ...)                          \
    do {                                                 \
        fprintf(stderr, "[FAIL] %s:%d: ",                \
                __FILE__, __LINE__);                     \
        fprintf(stderr, format, ##__VA_ARGS__);          \
    } while (0)

#define _STAR_PASS(format, ...)    printf("[PASS] " format, ##__VA_ARGS__)
#define _STAR_SUMMARY(format, ...) printf("\nTechnical and Reliable Summary: " format, ##__VA_ARGS__)
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

// Equality & Inequality
#define ASS_EQ(a, b)                                                      \
    if ((a) != (b)) {                                                     \
        _STAR_FAIL("ASS_EQ(%s, %s) failed: %lf != %lf\n",                 \
            #a, #b, (a), (b));                                            \
        return;                                                           \
    }


#define ASS_NEQ(a, b)                                                     \
    if ((a) == (b)) {                                                     \
        _STAR_FAIL("ASS_NEQ(%s, %s) failed: %lf == %lf\n",                \
            #a, #b, (a), (b));                                            \
        return;                                                           \
    }

// Look at this song and dance I have to do... (easily preventable if I wasn't stubborn)
#define ASS_KINDAEQ(a, b, dptr)                                                        \
    do {                                                                               \
        double n = 6.9;                                                                \
        if ((dptr) != NULL) {                                                          \
            const double *tmp__ = (const double *)(dptr);                              \
            n = *tmp__;                                                                \
        }                                                                              \
        if (!(fabs((a) - (b)) <= n)) {                                                 \
            _STAR_FAIL("ASS_KINDAEQ(%s, %s) failed: %lf !≈ %lf (degree %lf)\n",        \
                      #a, #b, (a), (b), n);                                            \
            return;                                                                    \
        }                                                                              \
    } while (0)


#define ASS_KINDANEQ(a, b, dptr)                                                      \
    do {                                                                              \
        double n = 6.9;                                                               \
        if ((dptr) != NULL) {                                                         \
            const double *tmp__ = (const double *)(dptr);                             \
            n = *tmp__;                                                               \
        }                                                                             \
        if ((fabs((a) - (b)) <= n)) {                                                 \
            _STAR_FAIL("ASS_KINDAEQ(%s, %s) failed: %lf ≈ %lf (degree %lf)\n",        \
                      #a, #b, (a), (b), n);                                           \
            return;                                                                   \
        }                                                                             \
    } while (0)

// Boolean / Truthiness
#define ASS_TRUE(expr)                                                    \
    if (!(expr)) {                                                        \
        _STAR_FAIL("ASS_TRUE(%s) failed\n", #expr);                       \
        return;                                                           \
    }

#define ASS_FALSE(expr)                                                   \
    if ((expr)) {                                                         \
        _STAR_FAIL("ASS_FALSE(%s) failed\n", #expr);                      \
        return;                                                           \
    }

/* Run Functionality */
#if defined(STAR_NO_ENTRY)
static inline void star_run(int o) {
    if (o) printf("Running %zu tests...\n", _star_test_count);

    int passed = 0;
    for (int i = 0; i < _star_test_count; i++) {
        tests[i].func();
        if (o) _STAR_PASS("%s\n", tests[i].name);
        passed++;
    }

    if (o) _STAR_SUMMARY("%d/%zu tests passed\n", passed, _star_test_count);
}
#else
int main(int argc, char** argv) {
    printf("Running %zu tests...\n", _star_test_count);

    int passed = 0;
    for (int i = 0; i < _star_test_count; i++) {
        _star_tests[i].func();
        _STAR_PASS("%s\n", _star_tests[i].name);
        passed++;
    }

    _STAR_SUMMARY("%d/%zu tests passed\n", passed, _star_test_count);

    return 0;
}
#endif /* STAR_NO_ENTRY */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* STAR_TEST_H */

/*
    Revision history:     
        0.3.2  (2025-11-23)  Minor changes: automatic file-line display in _STAR_FAIL + top-file description.
        0.3.1  (2025-11-23)  Minor changes: global test count is now size_t, user-irrelevant identifiers
                             now prefixed with `_star`.
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
