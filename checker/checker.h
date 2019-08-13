/**
 *  Author: alexloser
 *  Header file of xsvt.csvchk
 */
#pragma once
#ifndef __USE_LARGEFILE64
#define __USE_LARGEFILE64
#endif
#include <stdio.h>
#include <regex.h>
#include "xsvt/checker/utils.h"

#ifdef  __cplusplus
extern "C" {
#endif


/**
 * CSV checking state, do not change these numbers, they're
 * used as array index to get error message if failed.
 */
enum XSVT_CHK_STATE
{
    XSVT_CHK_OK = 0,
    XSVT_CHK_EMPTY_FILE = 1,
    XSVT_CHK_FILE_TOO_LARGE = 2,
    XSVT_CHK_ONLY_ONE_LINE = 3,
    XSVT_CHK_ONLY_ONE_COLUMN = 4,
    XSVT_CHK_LINE_TOO_LONG = 5,
    XSVT_CHK_WRONG_FORMAT = 6,
    __XSVT_CHK_N__ = 8
};


/**
 * Define CSV delimeters, only "," "\t" and " " allowed.
 * Do not change these constants.
 */
enum XSVT_CONSTS
{
    XSVT_DELIM_COMMA = ',',
    XSVT_DELIM_TAB   = '\t',
    XSVT_DELIM_SPACE = ' ',
    XSVT_DELIM_DEFAULT = XSVT_DELIM_COMMA,
    XSVT_MAX_LINE_LEN = (1024 * 1024), // CSV line shoule not be longer than this, in fact it's very huge.
};

#ifndef XSVT_MAX_FILE_SIZE
#define XSVT_MAX_FILE_SIZE ((size_t)(4UL*1024*1024*1024)) // Max file size is 4GB
#endif

/** User defined callback while reading each line. */
typedef int(*xsvt_user_cb)(const char* line);


/**
 * CSV description struct, stores important options.
 */
typedef struct __xsvt_checker__
{
    const char* name;       // csv name for reading
    char        delim;      // delimeter of input csv
    size_t      nrows;      // number rows(lines) in input csv
    size_t      ncols;      // number columns in input csv
    uint64_t    fsize;      // file size of input csv
    FILE*       __csv__;    // internal using
    regex_t*    __rgx__;    // internal using
    int         error;      // error number in XSVT_CHK_STATE
    const char* errmsg[__XSVT_CHK_N__]; // error message or info
    xsvt_user_cb  uc;   // user callback

} __attribute__((aligned(16)))* xsvt_checker;


/** Create new xsvt_checker, return NULL if failed. */
xsvt_checker xsvt_checker_create(const char* path, const char delim, xsvt_user_cb func);


/**
 * Release xsvt_checker if created and initialized successfully.
 * Note the input argument is the pointer of xsvt_checker.
 */
void xsvt_checker_release(xsvt_checker* _d);


/**
 * Using the first line in csv to build a regex_t object.
 * The `rgx` will be used to match rest lines in csv, if matched successfully, return 0.
 */
int xsvt_build_line_regex(const char* csv_line, const size_t len, const char delim, regex_t* rgx);


/**
 * Check csv format and write number columns of `csv_line` into `ncols`.
 * Return one of the XSVT_CHK_STATE enum value.
 */
int xsvt_check_format(const char* csv_line, const size_t len, const char delim, const regex_t* rgx, size_t* ncols);


/**
 * Get last error message string.
 * Free the string after used.
 */
char* xsvt_checker_error_msg(xsvt_checker d);


/**
 * Run checking if `d` was successful initialized.
 * It is not user's interface, using xsvt_check_file instead.
 */
int xsvt_checker_execute(xsvt_checker d);


/**
 * User interface, return 0 if check passed.
 */
int xsvt_check_file(const char* csv, const char delim);



#ifdef  __cplusplus
} // end extern "C"
#endif



