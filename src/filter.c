#include "filter.h"

#include <memory.h>

void* filter(const void* begin, const void* end, unsigned size,
    unsigned (*remove_if)(const void* elem, void* context), void* context,
    void* result) {
    const char* p = begin;
    char *q = result;
    while (p != end) {
        if (!remove_if(p, context)) {
            memcpy(q, p, size);
            q += size;
        }
        p += size;
    }
    return q;
}

#ifdef UNITTEST_
#include <stdio.h>

static unsigned remove_odd(const void* elem, void* context) {
    const int* ptr = elem;
    const unsigned result = *ptr % 2;
    fprintf(context, "passed %d%s\n", *ptr, (result ? "; filter out" : ""));
    return result;
}

int main() {
    int array[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int* begin = array;
    int* const end = filter(array, array + 10, sizeof(int), remove_odd, stdout, array);
    while (begin != end) {
        printf("%d ", *begin);
        begin += 1;
    }
    puts("");
    return !(end == array + 5);
}
#endif
