/**
 * Author: alexloser
 * Create on 2019
 * xsvt.onehot encoder implemention
 */
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "xsvt/onehot/onehot.h"

extern FILE* fopen64(const char*, const char*);



xsvt_oh_coder xsvt_oh_alloc_coder()
{
    return (xsvt_oh_coder)xsvt_alloc(sizeof(struct __xsvt_oh_coder__));
}

void xsvt_oh_init_coder(xsvt_oh_coder c, size_t ncolumns)
{
    assert(c != NULL);
    assert(ncolumns > 0);
    bzero(c, sizeof(struct __xsvt_oh_coder__));

    c->__ncol__ = ncolumns;
    const size_t size = sizeof(void*) * ncolumns;

    c->__mb__ = (char*)xsvt_alloc(size * 10);
    c->__vtypes__   = (int*)          (c->__mb__ + size * 0);
    c->__cnames__   = (xsvt_oh_svec) (c->__mb__ + size * 1);
    c->__cnames_ex__= (xsvt_oh_svec) (c->__mb__ + size * 2);
    c->__nt__       = (size_t*)       (c->__mb__ + size * 3);
    c->__dvtable__  = (xsvt_oh_svec*)(c->__mb__ + size * 4);
    c->__01slices__ = (xsvt_oh_svec*)(c->__mb__ + size * 5);

    for (size_t i = 0; i < ncolumns; ++i) {
        c->__vtypes__[i] = XSVT_CVT_UNDEFINE;
    }
}


void xsvt_oh_free_coder(xsvt_oh_coder* _c)
{
    CRUDE_CHECK_PTR(_c, return);
    CRUDE_CHECK_PTR(*_c, return);

    xsvt_oh_coder c = *_c;

    if (c->__cnames__)
        for (size_t i = 0; i < c->__ncol__; ++i)
            if (c->__cnames__[i])
                free(c->__cnames__[i]);

    if (c->__cnames_ex__)
        for (size_t i = 0; i < c->__ncol__; ++i)
            if (c->__cnames_ex__[i])
                free(c->__cnames_ex__[i]);

    if (c->__dvtable__)
        for (size_t i = 0; i < c->__ncol__; ++i) {
            if (c->__dvtable__[i]) {
                for (size_t j = 0; j < c->__nt__[i]; ++j) {
                    free(c->__dvtable__[i][j]);
                }
                free(c->__dvtable__[i]);
            }
        }

    if (c->__01slices__)
        for (size_t i = 0; i < c->__ncol__; ++i) {
            if (c->__01slices__[i]) {
                for (size_t j = 0; j < c->__nt__[i]; ++j)
                    free(c->__01slices__[i][j]);
                free(c->__01slices__[i]);
            }
        }

    if (c->__mb__)
        free(c->__mb__);

    bzero(c, sizeof(struct __xsvt_oh_coder__));
    free(c);
    *_c = NULL;
}


xsvt_oh_io xsvt_oh_create_io(const char* in_xsv,
                             const char* out_xsv,
                             const char delim,
                             const int has_header,
                             const int write_header)
{
    CRUDE_CHECK_PTR(in_xsv, return NULL);
    CRUDE_CHECK_PTR(out_xsv, return NULL);

    xsvt_oh_io io = (xsvt_oh_io)xsvt_alloc(sizeof(struct __xsvt_oh_io__));

    io->name = strdup(in_xsv);

    io->in_xsv = fopen64(in_xsv, "r");
    if (!io->in_xsv) {
        XSVT_ERROR("Open %s failed!", in_xsv);
        xsvt_oh_release_io(&io);
        return NULL;
    }

    io->out_xsv = fopen64(out_xsv, "w");
    if (!io->out_xsv) {
        XSVT_ERROR("Open %s failed!", out_xsv);
        xsvt_oh_release_io(&io);
        return NULL;
    }

    io->has_head    = has_header;
    io->write_head  = write_header;
    io->delim       = delim;

    return io;
}


void xsvt_oh_release_io(xsvt_oh_io* _io)
{
    CRUDE_CHECK_PTR(_io, return);
    CRUDE_CHECK_PTR(*_io, return);

    xsvt_oh_io io = *_io;
    if (io->in_xsv) {
        fclose(io->in_xsv);
    }

    if (io->out_xsv) {
        fflush(io->out_xsv);
        fclose(io->out_xsv);
    }

    if (io->name) {
        free((char*)io->name);
    }

    bzero(io, sizeof(struct __xsvt_oh_io__));
    free(io);
    *_io = NULL;
}


void xsvt_oh_get_column(const xsvt_oh_coder c, size_t column, int* type, char** colname, char** colname_ex)
{
    assert(c);
    assert(column < c->__ncol__);
    assert(type || colname || colname_ex);

    if (type) {
        *type = c->__vtypes__[column];
    }

    if (colname) {
        *colname = c->__cnames__[column];
    }

    if (c->__cnames_ex__) {
        if (colname_ex) {
            *colname_ex = c->__cnames_ex__[column];
        }
    }

}

static int __xsvt_2scb(const void* x, const void* y)
{
    return strcmp(*(char**)x, *(char**)y);
}

const char* xsvt_oh_get_codes(const xsvt_oh_coder c, size_t k, const char* value)
{
    assert(c && value);
    assert(c->__dvtable__);
    assert(c->__01slices__);

    const size_t size = c->__nt__[k];

    if (c->__dvtable__[k] && XSVT_CVT_DISCRETE == c->__vtypes__[k]) {
        xsvt_oh_svec pos = bsearch(&value, c->__dvtable__[k], size, sizeof(char*), __xsvt_2scb);
        if (!pos) {
            return NULL;
        }
        ptrdiff_t idx = pos - c->__dvtable__[k];
        return c->__01slices__[k][idx];
    }

    return NULL;
}

void xsvt_oh_set_column(xsvt_oh_coder c, size_t column, int type, const char* name)
{
    assert(c);
    assert(column < c->__ncol__);

    if (name) {
        if (c->__cnames__[column]) {
            free(c->__cnames__[column]);
        }
        c->__cnames__[column] = strdup(name);
    }

    if (type) {
        c->__vtypes__[column] = type;
    }
}


int xsvt_oh_add_discrete_value(xsvt_oh_coder c, size_t column, const char* value, const size_t len)
{
    assert(c);
    assert(value);

    if (column >= c->__ncol__) {
        XSVT_ERROR("Wrong column number at %lu", column);
        return column;
    }

    assert(value);
    if (len == 0) {
        XSVT_ERROR("Found empty field as discrete value!");
        return EINVAL;
    }

    #pragma push_macro("dvc")
    #define dvc c->__dvtable__[column]

    size_t size = c->__nt__[column];
    void* pos = bsearch(&value, dvc, size, sizeof(char*), __xsvt_2scb);

    if (pos == NULL) {
        size_t capacity = ((size & 0xFFFFFFF0) + 16) * sizeof(char*);
        assert(capacity > size);
        dvc = (xsvt_oh_svec)realloc(dvc, capacity);
        dvc[size] = (char*)xsvt_alloc(len + 1);
        bcopy(value, dvc[size], len);
        size += 1;
        c->__nt__[column] = size;
        qsort(dvc, size, sizeof(char*), __xsvt_2scb);
    }

    #pragma pop_macro("dvc")
    return 0;
}

// TODO: optimize and rebuild this shit!
static void xsvt_oh_build_codes_detail(xsvt_oh_coder c, size_t k)
{
    assert(c && k < c->__ncol__);
    assert(c->__cnames_ex__[k] == NULL);
    assert(c->__01slices__[k] == NULL);

    const char* colname = c->__cnames__[k];
    const size_t nraw = strlen(colname);
    size_t len = 0;
    size_t bufsize = 0;

    // Expand column names
    #pragma push_macro("dvc")
    #define dvc c->__dvtable__[k]

    for (size_t i = 0; i < c->__nt__[k]; ++i) {
        bufsize += nraw + strlen(dvc[i]) + 2;  // "-" and "," need extra space for joint string
    }
    char* buf = (char*)xsvt_alloc(bufsize + 1);

    if (XSVT_CVT_DISCRETE == c->__vtypes__[k]) {
        for (size_t i = 0; i < c->__nt__[k]; ++i) {
            len += (size_t)sprintf(buf + len, "%s-%s,", colname, dvc[i]);
        }
        if (len) {  // strip lase comma
            buf[len - 1] = 0;
        }
    }
    else {
        strcpy(buf, c->__cnames__[k]);
    }

    c->__cnames_ex__[k] = buf;
    #pragma pop_macro("dvc")

    if (XSVT_CVT_DISCRETE != c->__vtypes__[k]) {
        return;
    }

    // Expand discrete values into one-hot encoding
    #pragma push_macro("zoi")
    #define zoi c->__01slices__[k]
    zoi = (xsvt_oh_svec)xsvt_alloc(sizeof(char*) * (c->__nt__[k]));
    // Sorry, here need some black-magic ...
    for (size_t i = 0; i < c->__nt__[k]; ++i) {
        bufsize = c->__nt__[k] * 2;
        buf = (char*)xsvt_alloc(bufsize + 1);
        for (size_t x = 0; x < bufsize; ++x) {
            buf[x] = (x & 0x01) ? ',' : '0';
        }
        buf[i << 1] = '1';
        buf[bufsize - 1] = 0;  // strip last comma
        zoi[i] = buf;
    }
    #pragma pop_macro("zoi")

}


int xsvt_oh_build_codes(xsvt_oh_coder c)
{
    assert(c);

    for (size_t k = 0; k < c->__ncol__; ++k) {
        switch (c->__vtypes__[k]) {
        case XSVT_CVT_DISCRETE:
            if (0 == c->__nt__[k]) {
                return EINVAL;
            }
            break;
        case XSVT_CVT_FLOATING:
        case XSVT_CVT_INTEGER:
            continue;
        case XSVT_CVT_UNDEFINE:
        default:
            return EINVAL;
        }
    }

    for (size_t k = 0; k < c->__ncol__; ++k) {
        xsvt_oh_build_codes_detail(c, k);
    }

    return 0;
}



int xsvt_oh_compilex(regex_t* rgx_int, regex_t* rgx_flt)
{
    assert(rgx_int && rgx_flt);

    const char* sre_int = "^[-+]?[0-9]+$";
    const char* sre_flt = "^[-+]?[\\.0-9]+[eE]?[-+]?[0-9]*$";

    XSVT_INFO("regex int: \"%s\"", sre_int);
    XSVT_INFO("regex flt: \"%s\"", sre_flt);

    int err1 = regcomp(rgx_int, sre_int, REG_EXTENDED);
    int err2 = regcomp(rgx_flt, sre_flt, REG_EXTENDED);

    return err1 == REG_NOERROR ? err2 : err1;
}


void xsvt_oh_show_vtypes(xsvt_oh_coder c)
{
    for (size_t k = 0; k < c->__ncol__; ++k) {
        switch (c->__vtypes__[k]) {
        case XSVT_CVT_DISCRETE: XSVT_INFO("coltype-%lu %s", k, "DISCRETE");break;
        case XSVT_CVT_INTEGER:  XSVT_INFO("coltype-%lu %s", k, "INT");     break;
        case XSVT_CVT_FLOATING: XSVT_INFO("coltype-%lu %s", k, "FLOAT");   break;
        case XSVT_CVT_UNDEFINE: XSVT_INFO("coltype-%lu %s", k, "UNKNOWN"); break;
        default:
            break;
        }
    }
}

#ifndef NDEBUG
void xsvt_oh_show_mappings(xsvt_oh_coder c)
{
    for (int i = 0; i < c->__ncol__; ++i) {
        CRUDE_FPRINT("[DEBUG] Col-%d %-13s: ", i, c->__cnames__[i]);
        if (c->__nt__[i]) {
            for (int j = 0; j < c->__nt__[i] && j < 5; ++j)
                CRUDE_FPRINT("[%s]=>[%s]  ", c->__dvtable__[i][j], c->__01slices__[i][j]);
            CRUDE_FPRINT("...\n");
        }
        else {
            CRUDE_FPRINT("%s\n", ((XSVT_CVT_INTEGER == c->__vtypes__[i]) ? "{INTEGER}" : "{FLOATING}"));
        }
        fflush(stdout);
    }
}
#else
#define xsvt_oh_show_mappings(c)
#endif


int xsvt_oh_analyze(const xsvt_oh_io io, xsvt_oh_coder c)
{
    CRUDE_CHECK_PTR(io, return EINVAL);
    CRUDE_CHECK_PTR(c, return EINVAL);
    assert(0 == ferror(io->in_xsv));

    // First, check csv.
    xsvt_checker d = xsvt_checker_create(io->name, io->delim, NULL);
    if (NULL == d) {
        xsvt_checker_release(&d);
        return -1;
    }

    int err = xsvt_checker_execute(d);
    XSVT_INFO("Filesize: %.3fKB", d->fsize / (double)(1024));
    XSVT_INFO("Lines:    %lu", d->nrows);
    XSVT_INFO("Columns:  %lu", d->ncols);
    char* errmsg = xsvt_checker_error_msg(d);
    XSVT_INFO("%s", errmsg);
    free(errmsg);
    const size_t ncols = d->ncols;
    xsvt_checker_release(&d);
    if (err != XSVT_CHK_OK) {
        return err;
    }

    // Second, read the csv and get column names, column types.
    xsvt_oh_init_coder(c, ncols);

    size_t capacity = XSVT_MAX_LINE_LEN;
    char* line = (char*)xsvt_alloc(capacity);
    xsvt_slice* slice_arr = (xsvt_slice*)xsvt_alloc(ncols * sizeof(xsvt_slice));
    regex_t rgx_int, rgx_flt;
    if (xsvt_oh_compilex(&rgx_int, &rgx_flt) != REG_NOERROR) {
        XSVT_WARN("REGEX_ERROR!");
    }

    rewind(io->in_xsv);
    if (!io->has_head) {    // No header, using COLN-XXXX as column name
        for (int i = 0; i < ncols; ++i) {
            sprintf(line, "COL%d", i + 1);
            xsvt_oh_set_column(c, i, XSVT_CVT_UNDEFINE, line);
            XSVT_DEBUG("colname-%u: %s", (unsigned)i, c->__cnames__[i]);
        }
    }
    else {    // Has header, use it
        xsvt_readline(io->in_xsv, line, capacity);
        if (ncols != xsvt_tokenize(line, strlen(line), io->delim, slice_arr, ncols)) {
            XSVT_ERROR("Tokenize not correct!");
            err = XSVT_CHK_WRONG_FORMAT;
            goto _ANALYZE_RETURN_;
        }
        for (size_t i = 0; i < ncols; ++i) {
            xsvt_oh_set_column(c, i, XSVT_CVT_UNDEFINE, slice_arr[i].begin);
            XSVT_DEBUG("colname-%u: %s", (unsigned)i, c->__cnames__[i]);
        }
    }

    #define rgx_int_match(k) (REG_NOERROR == regexec(&rgx_int, slice_arr[k].begin, 0, 0, 0))
    #define rgx_flt_match(k) (REG_NOERROR == regexec(&rgx_flt, slice_arr[k].begin, 0, 0, 0))

    // Read entire csv file at first time, ensure type of each column.
    while (xsvt_readline(io->in_xsv, line, capacity) != XSVT_READLINE_EOF) {
        xsvt_tokenize(line, strlen(line), io->delim, slice_arr, ncols);
        for (size_t k = 0; k < ncols; ++k) {
            int type_prev, type_cur; xsvt_oh_get_column(c, k, &type_prev, NULL, NULL);
            if (type_prev == XSVT_CVT_DISCRETE)
                continue;
            if (rgx_int_match(k))
                type_cur = XSVT_CVT_INTEGER;
            else if (rgx_flt_match(k))
                type_cur = XSVT_CVT_FLOATING;
            else
                type_cur = XSVT_CVT_DISCRETE;
            if (type_cur > type_prev)
                xsvt_oh_set_column(c, k, type_cur, NULL);
        }
    }

    #undef rgx_int_match
    #undef rgx_flt_match

    xsvt_oh_show_vtypes(c);

    rewind(io->in_xsv);
    if (io->has_head) { // skip header
        xsvt_readline(io->in_xsv, line, capacity);
    }
    // Last, parse the values .
    // Read entire csv file again, parse and store all discrete-values into
    // searching-table of xsvt_oh_coder, preparing for onehot-encoding（xsvt_oh_build_codes)
    size_t num = io->has_head ? 1 : 0;
    while (xsvt_readline(io->in_xsv, line, capacity) != XSVT_READLINE_EOF) {
        num += 1;
        xsvt_tokenize(line, strlen(line), io->delim, slice_arr, ncols);
        for (size_t k = 0; k < ncols; ++k) {
            int type; xsvt_oh_get_column(c, k, &type, NULL, NULL);
            if (type != XSVT_CVT_DISCRETE) {
                continue;
            }
            err = xsvt_oh_add_discrete_value(c, k, slice_arr[k].begin, xsvt_slice_size(slice_arr[k]));
            if (err) {
                XSVT_ERROR("Analysis failed at line %lu", num);
                goto _ANALYZE_RETURN_;
            }
        }
    }

    // Now we can build one-hot coding
    if (xsvt_oh_build_codes(c) != 0) {
        XSVT_ERROR("Build onehot codes failed, csv data maybe not correct!");
    } else {
        xsvt_oh_show_mappings(c);
    }

    _ANALYZE_RETURN_:
    regfree(&rgx_int);
    regfree(&rgx_flt);
    free(line);
    free(slice_arr);
    XSVT_INFO("Analysis finished");
    return err;
}


int xsvt_oh_output(const xsvt_oh_io io, const xsvt_oh_coder c)
{
    CRUDE_CHECK_PTR(io, return EINVAL);
    CRUDE_CHECK_PTR(io->out_xsv, return EINVAL);
    CRUDE_CHECK_PTR(c, return EINVAL);

    int err = 0;
    #pragma push_macro("dvc")
    #pragma push_macro("zoi")
    #define dvc c->__dvtable__[k]
    #define zoi c->__01slices__[k]

    const size_t capacity = XSVT_MAX_LINE_LEN;
    char* line = (char*)xsvt_alloc(capacity);
    rewind(io->in_xsv);
    rewind(io->out_xsv);

    if (io->has_head) { // skip head
        xsvt_readline(io->in_xsv, line, capacity);
        if (io->write_head) {   // dump expanded header
            for (size_t k = 0; k < c->__ncol__; ++k) {
                fprintf(io->out_xsv, "%s,", c->__cnames_ex__[k]);
            }
            fseeko64(io->out_xsv, -1, SEEK_CUR);
            fwrite("\n", 1, 1, io->out_xsv);
        }
    }

    xsvt_slice* slice_arr = (xsvt_slice*)xsvt_alloc(c->__ncol__ * sizeof(xsvt_slice));

    // Read entire csv file and write results into out_xsv at the same time.
    while ( xsvt_readline(io->in_xsv, line, capacity) != XSVT_READLINE_EOF ) {
        size_t ncols = xsvt_tokenize(line, strlen(line), io->delim, slice_arr, c->__ncol__);
        assert(c->__ncol__ == ncols);

        for (size_t k = 0; k < ncols; ++k) {
            int type;
            xsvt_oh_get_column(c, k, &type, NULL, NULL);
            if (type != XSVT_CVT_DISCRETE) {
                fprintf(io->out_xsv, "%s,", slice_arr[k].begin);
            } else {
                const char* code = xsvt_oh_get_codes(c, k, slice_arr[k].begin);
                if (code) {
                    fprintf(io->out_xsv, "%s,", code);
                } else {
                    XSVT_ERROR("Can not find code: `%s`, output will be failed!", slice_arr[k].begin);
                    err = ENODATA;
                }
            }
        }

        fseeko64(io->out_xsv, -1, SEEK_CUR);
        fwrite("\n", 1, 1, io->out_xsv);
    }

    fflush(io->out_xsv);

    #pragma pop_macro("zoi")
    #pragma pop_macro("dvc")
    free(line);
    free(slice_arr);
    return err;
}

int xsvt_oh_encode(const char* in_xsv,
                   const char* out_xsv,
                   const char  delim,
                   const int   has_header,
                   const int   write_header)
{
    CRUDE_CHECK_PTR(in_xsv,  return EINVAL);
    CRUDE_CHECK_PTR(out_xsv, return EINVAL);
    CRUDE_ASSERT(delim,   return EINVAL);

    XSVT_INFO("Input csv : %s", in_xsv);
    XSVT_INFO("Output csv: %s", out_xsv);
    XSVT_INFO("Delimiter : \"%c\"", delim);
    XSVT_WARN("Has header: %s", (has_header ? "True" : "False"));
    XSVT_INFO("Dup header: %s", (write_header ? "True" : "False"));

    int err = 0;
    xsvt_oh_io io = xsvt_oh_create_io(in_xsv, out_xsv, delim, has_header, write_header);
    if (NULL == io) {
        XSVT_ERROR("Create onehot io failed!");
        err = ENOENT;
    }
    else {
        xsvt_oh_coder c = xsvt_oh_alloc_coder();
        err = xsvt_oh_analyze(io, c);
        if (err) {
            XSVT_ERROR("CSV analyze failed!");
        } else {
            err = xsvt_oh_output(io, c);
            if (err)
                XSVT_ERROR("Onehot output failed!");
            else
                XSVT_INFO("Onehot output finished!");
        }
        xsvt_oh_free_coder(&c);
    }

    xsvt_oh_release_io(&io);
    return err;
}




