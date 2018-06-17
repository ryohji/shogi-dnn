#ifndef FILTER_H_
#define FILTER_H_

/**
 * Filter out the elements in the range of [begin, end), if passing function
 * returns non-zero value on remove_if(elem, context). Each element have the
 * `size' bytes. Filtered elements are stored in result, and filter function
 * returns the end boundary -- [result, returned void*).
 */
void* filter(const void* begin, const void* end, unsigned size,
    unsigned (*remove_if)(const void* elem, void* context), void* context,
    void* result);

#endif
