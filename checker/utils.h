#pragma once
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "crude-log.h"

#ifndef XSVT_LOG
#define XSVT_DEBUG  CRUDE_DEBUG_V3
#define XSVT_INFO   CRUDE_INFO_V2
#define XSVT_WARN   CRUDE_WARN_V2
#define XSVT_ERROR  CRUDE_ERROR_V2
#define XSVT_CHKPTR CRUDE_CHECK_PTR
#define XSVT_ASSERT CRUDE_ASSERT
#endif

#ifndef XSVT_INLINE
#define XSVT_INLINE static __inline__
#endif

#ifdef  XSVT_USE_LOCKED_VERIO
#define XSVT_GETLINE fgets
#else
extern char* fgets_unlocked(char*, int, FILE*);
#define XSVT_GETLINE fgets_unlocked
#endif

#ifndef XSVT_FIO_STATE
#define XSVT_FIO_STATE
#define XSVT_READLINE_EMPTY     ((size_t) 0)
#define XSVT_READLINE_EOF       ((size_t)-1)
#define XSVT_READLINE_OVERFLOW  ((size_t)-2)
#endif//XSVT_FIO_STATE

/** Better fgets for xsvt */
XSVT_INLINE size_t xsvt_readline(FILE* file, char* buf, size_t bufsize)
{
    if (!XSVT_GETLINE(buf, bufsize, file)) {
        return XSVT_READLINE_EOF;
    }

    size_t len = __builtin_strlen(buf);

    if (buf[len - 1] == '\n') {
        buf[--len] = 0;
        if (len && buf[len - 1] == '\r') {
            buf[--len] = 0;
        }
    } else if (!feof(file)) {
        return XSVT_READLINE_OVERFLOW;
    }

    return len ? len : XSVT_READLINE_EMPTY;
}

/** Better fgets for xsvt */
XSVT_INLINE int xsvt_readline_neof(FILE* file, char* buf, size_t bufsize, size_t* _len)
{
    *_len = xsvt_readline(file, buf, bufsize);
    return *_len != XSVT_READLINE_EOF;
}


/**
 * A simple c-string holder, just like string-piece or string-slice,
 * but, it's not ...
 */
typedef struct __xsvt_slice__
{
    char* begin;
    char* end;
} xsvt_slice;

// Get string length
#ifndef xsvt_slice_size
#define xsvt_slice_size(s) ((s).end - (s).begin)
#endif


/**
 * Tokenize the string `str` into slices by `delim`.
 * The `str` will be changed, so it can Not be literal or const string!!!
 * Return number of tokens in `slices` if success, or 0 if failed.
 */
XSVT_INLINE size_t xsvt_tokenize(char* str, size_t len, char delim, xsvt_slice slices[], size_t slices_size)
{
    if (!str || !len || !slices) {
        XSVT_ERROR("Wrong arguments!");
        return 0;
    }
    char* begin = str;
    char* end = NULL;
    int nrest = (int)len;
    int i = 0;
    while (nrest > 0 && i < slices_size) {
        end = (char*)(memchr(begin, delim, nrest));
        slices[i].begin = begin;
        if (end) {
            nrest -= end - begin + 1;
            slices[i].end = end;
        } else {
            nrest = 0;
            slices[i].end = str + len;
        }
        begin = end + 1;
        i += 1;
    }
    for (int k = 0; k < i; ++k) {
        if (slices[k].begin != slices[k].end) {
            if (*slices[k].begin == '\"') {
                slices[k].begin += 1;
            }
            if (*(slices[k].end - 1) == '\"') {
                slices[k].end -= 1;
            }
            if (slices[k].begin > slices[k].end) {
                slices[k].begin = slices[k].end;
            }
        }
        *(slices[k].end) = 0;
    }
    return i;
}


XSVT_INLINE void* xsvt_alloc(size_t nbytes)
{
    assert(nbytes);
    void* a = calloc(nbytes, 1);
    if (NULL == a) {
        XSVT_ERROR("Alloc memory failed: %luB", nbytes);
    }
    return a;
}

XSVT_INLINE void* xsvt_realloc(void* p, size_t new_size)
{
    assert(p);
    assert(new_size);
    void* a = realloc(p, new_size);
    if (NULL == a) {
        XSVT_ERROR("Realloc memory failed: %luB", new_size);
    }
    return a;
}

