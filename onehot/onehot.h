// Author: alexloser
// Create on 2019
// xsvt.onehot encoder header file
#pragma once
#ifndef __USE_LARGEFILE64
#define __USE_LARGEFILE64
#endif
#include <stdio.h>
#include <regex.h>
#include "xsvt/checker/checker.h"

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * Value types of csv fields, we choose DISCRETE type to encode
 * The value of each enum determines the update priority, do not change.
 */
enum XSVT_VALUE_TYPE
{
    XSVT_CVT_UNDEFINE = 0,   // Undefined or unknown value, also initialize flag
    XSVT_CVT_INTEGER  = 1,   // Integer number(no point), such as 15、0、666, even very huge value
    XSVT_CVT_FLOATING = 2,   // Floating point number types: -3.33, .0, 1e-5, 654.4321+e99
    XSVT_CVT_DISCRETE = 4    // Discrete and category types, none-numbers
};

/* Strings vector */
typedef char** xsvt_oh_svec;

/**
 * Encoder data structure
 */
typedef struct __xsvt_oh_coder__
{
    size_t          __ncol__;       // Number columns of input table(csv)
    int*            __vtypes__;     // Value types for each columns
    xsvt_oh_svec   __cnames__;     // The name of each columns (raw)
    xsvt_oh_svec   __cnames_ex__;  // The name of each columns after expanded, only for onehot
    xsvt_oh_svec*  __dvtable__;    // Values collected for each columns (raw)
    xsvt_oh_svec*  __01slices__;   // The name of each columns (raw)
    size_t*         __nt__;         // internal using
    char*           __mb__;         // internal using
} __attribute__((aligned(16)))* xsvt_oh_coder;


/**
 * Input and output descriptions, stores important options.
 */
typedef struct __xsvt_oh_io__
{
    const char* name; // Input file name
    FILE* in_xsv;     // Opened file for read input
    FILE* out_xsv;    // Opened file for write output
    int has_head;     // Input with header
    int write_head;   // Write header into output
    char delim;       // CSV delimiter, should be one of ',' ' ' '\t'
} __attribute__((aligned(16)))* xsvt_oh_io;


/** Internal using functions */
int xsvt_oh_compilex(regex_t* rgx_int, regex_t* rgx_flt);


/**
 * Create and initialize a new xsvt_oh_io object.
 * Return NULL if create failed. Check the params before using.
 */
xsvt_oh_io xsvt_oh_create_io(const char* in_xsv,
                             const char* out_xsv,
                             const char delim,
                             const int has_header,
                             const int write_header);
/**
 * Free xsvt_oh_io after created and used.
 * Note the input arg is xsvt_oh_io**, that means *_io will be destroy after calling this.
 */
void xsvt_oh_release_io(xsvt_oh_io* _io);

/** Alloc and release coder struct */
xsvt_oh_coder xsvt_oh_alloc_coder();
void xsvt_oh_free_coder(xsvt_oh_coder* _c);

/** Core algorithm */
int xsvt_oh_analyze(const xsvt_oh_io io, xsvt_oh_coder c);

/**
 * Get onehot encoding of `value` at column `k`, lookslike "0,0,1,0,0,0"
 * Call this only after xsvt_oh_analyze_onehot returned successfully.
 */
const char* xsvt_oh_get_codes(const xsvt_oh_coder c, size_t k, const char* value);

/** Output results if xsvt_oh_analyze successful */
int xsvt_oh_output(const xsvt_oh_io io, const xsvt_oh_coder c);


/** This is the user interface, packed the main steps below(error check was ignored):
 *    ...
 *    xsvt_oh_io io = xsvt_oh_alloc_io();
 *    xsvt_oh_init_io(io, ...);
 *    xsvt_oh_coder c = xsvt_oh_alloc_coder();
 *    xsvt_oh_analyze_onehot(io, c);
 *    xsvt_oh_output(io, c);
 *    xsvt_oh_free_coder(&c);
 *    xsvt_oh_release_io(&io);
 *    ...
 * Return 0 if success.
 */
int xsvt_oh_encode(const char* in_xsv,
                   const char* out_xsv,
                   const char  delim,
                   const int   has_header,
                   const int   write_header);


#ifdef  __cplusplus
} // end extern "C"
#endif


