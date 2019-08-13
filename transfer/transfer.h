/**
 *  Author: alexloser
 *  Create on 2019
 *  Header file of xsvt.cvt2
 */
#pragma once
#ifndef __USE_LARGEFILE64
#define __USE_LARGEFILE64
#endif
#include <stdio.h>
#include "xsvt/checker/checker.h"

#ifdef  __cplusplus
extern "C" {
#endif


enum XSVT_TRANS_DIRECTION
{
    CSV_TO_SVM = 0,
    SVM_TO_CSV = 1
};

typedef int(*xsvt_transfer_user_cb)(size_t i, size_t j, const char* value);


/**
 *
 */
typedef struct __xsvt_transfer__
{
    const char* input;
    const char* output;
    const char* label;
    char       _delim;
    int        _header;
    int        _dir;
    FILE*      _fin;
    FILE*      _fout;
    size_t     _nrows;
    size_t     _ncols;
    uint64_t   _fsize;
    xsvt_transfer_user_cb  _uc;
} __attribute__((aligned(16)))* xsvt_transfer;


/**
 *
 */
xsvt_transfer xsvt_transfer_create(const char* input,
                                   const char* output,
                                   const char* label,
                                   const char  delim,
                                   const int   csvheader,
                                   const int   direction,
                                   xsvt_transfer_user_cb uc);

/**
 *
 */
void xsvt_transfer_destroy(xsvt_transfer* _t);

/**
 *
 */
int  xsvt_svm2csv(xsvt_transfer t);
int  xsvt_csv2svm(xsvt_transfer t);


/**
 *
 */
int xsvt_transform( const char* input,
                    const char* output,
                    const char* label,
                    const char  delim,
                    const int   csvheader,
                    const int   direction  );



#ifdef  __cplusplus
} // end extern "C"
#endif

