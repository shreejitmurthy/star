#ifndef STAR_TEST_H
#define STAR_TEST_H

#include <stdio.h>
#include <stdlib.h>

typedef void (*test_func)();

typedef struct {
    const char *name;
    test_func func;
} test_case;

static test_case tests[256];
static int test_count = 0;

#if !defined(STAR_NO_COLOR)
#define _STAR_FAIL(format, ...)    printf("\033[31m[FAIL]\033[0m " format, ##__VA_ARGS__)
#define _STAR_PASS(format, ...)    printf("\033[32m[PASS]\033[0m " format, ##__VA_ARGS__)
#define _STAR_SUMMARY(format, ...) printf("\033[1m\nTechnical and Reliable Summary:\033[0m " format, ##__VA_ARGS__)
#else
#define _STAR_FAIL(format, ...)    printf("[FAIL] " format, ##__VA_ARGS__)
#define _STAR_PASS(format, ...)    printf("[PASS] " format, ##__VA_ARGS__)
#define _STAR_SUMMARY(format, ...) printf("\nTechnical and Reliable Summary: " format, ##__VA_ARGS__)
#endif /* STAR_NO_COLOR */

#define TEST(name) \
    void name(); \
    __attribute__((constructor)) \
    void register_##name() { \
        tests[test_count++] = (test_case){#name, name}; \
    } \
    void name()

#define ASS_TRUE(expr) \
    if (!(expr)) { \
        _STAR_FAIL("%s:%d: ASS_TRUE(%s) failed\n", __FILE__, __LINE__, #expr); \
        return; \
    }

#define ASS_EQ(a, b) \
    if ((a) != (b)) { \
        _STAR_FAIL("%s:%d: ASS_EQ(%s, %s) failed: %d != %d\n", __FILE__, __LINE__, #a, #b, (a), (b)); \
        return; \
    }

#if defined(STAR_NO_ENTRY)
// Test runner
static inline void star_run(int o) {
    if (o) printf("Running %d tests...\n", test_count);

    int passed = 0;
    for (int i = 0; i < test_count; i++) {
        tests[i].func();
        if (o) _STAR_PASS("%s\n", tests[i].name);
        passed++;
    }

    if (o) _STAR_SUMMARY("%d/%d tests passed\n", passed, test_count);
}
#else
int main(int argc, char** argv) {
    printf("Running %d tests...\n", test_count);

    int passed = 0;
    for (int i = 0; i < test_count; i++) {
        tests[i].func();
        _STAR_PASS("%s\n", tests[i].name);
        passed++;
    }

    _STAR_SUMMARY("%d/%d tests passed\n", passed, test_count);

    return 0;
}
#endif /* STAR_NO_ENTRY */

#endif /* STAR_TEST_H */

/*
    Revision history:                                  
        0.2.0  (2025-11-22)  Created test running and two asserts.
                             `main()` hijacking toggleable and
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
