/**
 *  Author: alexloser
 *  Source file of xsvt.csvchk
 */
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "xsvt/checker/checker.h"


/**
 * Sometimes GCC can't find these 64bit function's declaration, so...
 * Makesure ftello64 will return 64bit integer to fitting large file.
 */
extern FILE* fopen64(const char*, const char*);
extern int fseeko64(FILE*, __off64_t, int);
extern __off64_t ftello64(FILE*);


xsvt_checker xsvt_checker_create(const char* path, const char delim, xsvt_user_cb func)
{
    assert(path);
    if (!path) {
        return NULL;
    }

    xsvt_checker d = (xsvt_checker)xsvt_alloc(sizeof(struct __xsvt_checker__));

    switch (delim) {
    case XSVT_DELIM_COMMA:
    case XSVT_DELIM_TAB:
    case XSVT_DELIM_SPACE:
        break;
    default:
        XSVT_ERROR("Error: csv delimiter \"%c\" is not correct!", delim);
        xsvt_checker_release(&d);
        return NULL;
    }

    d->name = strdup(path);
    d->delim = delim;
    d->uc = func;
    d->error = XSVT_CHK_OK;

    d->errmsg[XSVT_CHK_OK] = "No error found";
    d->errmsg[XSVT_CHK_EMPTY_FILE] = "File is empty";
    d->errmsg[XSVT_CHK_FILE_TOO_LARGE] = "File is too large, more than 4GB";
    d->errmsg[XSVT_CHK_ONLY_ONE_LINE] = "CSV has only one line";
    d->errmsg[XSVT_CHK_ONLY_ONE_COLUMN] = "CSV has only one column";
    d->errmsg[XSVT_CHK_LINE_TOO_LONG] = "Line is to long, more than 100k bytes";
    d->errmsg[XSVT_CHK_WRONG_FORMAT] = "Wrong format";

    d->__csv__ = fopen64(path, "rb");
    if (!d->__csv__) {
        XSVT_ERROR("Open file \"%s\" failed!", path);
        xsvt_checker_release(&d);
        return NULL;
    }

    d->__rgx__ = (regex_t*)xsvt_alloc(sizeof(regex_t));

    return d;
}

void xsvt_checker_release(xsvt_checker* _d)
{
    XSVT_CHKPTR(_d, return);
    XSVT_CHKPTR(*_d, return);

    xsvt_checker d = *_d;

    if (d->name) {
        free((char*)d->name);
    }

    if (d->__csv__) {
        fclose(d->__csv__);
    }

    if (d->__rgx__) {
        regfree(d->__rgx__);
        free(d->__rgx__);
    }

    bzero(d, sizeof(struct __xsvt_checker__));
    free(d);
    *_d = NULL;
}


int xsvt_build_line_regex(const char* csv_line, const size_t len, const char delim, regex_t* rgx)
{
    XSVT_CHKPTR(csv_line);
    XSVT_CHKPTR(rgx);
    XSVT_ASSERT(len > 0);
    XSVT_ASSERT(delim == XSVT_DELIM_SPACE || delim == XSVT_DELIM_COMMA || delim == XSVT_DELIM_TAB);

    char buf[32] = {0};
    snprintf(buf, sizeof(buf), "[^%c]+%c", delim, delim);

    size_t ndelim = 0;
    for (const char* pos = strchr(csv_line, delim); pos; pos = strchr(pos+1, delim)) {
        ndelim += 1;
    }

    size_t bufsize = strlen(buf);
    char* sre = (char*)xsvt_alloc(bufsize * (ndelim + 1) + 1);

    sre[0] = '^';
    for (size_t i = 0; i <= ndelim; ++i) {
        memcpy(sre + (i * bufsize) + 1, buf, bufsize);
    }

    char* pos = strrchr(sre, '+');
    if (pos) {
        *(pos + 1) = '$';
        *(pos + 2) = 0;
    }

    int err = regcomp(rgx, sre, REG_EXTENDED);
    if (err != REG_NOERROR) {
        XSVT_ERROR("Build regex_t failed with: %s", sre);
    }

    free(sre);
    return err;
}


int xsvt_check_format(const char* csv_line,
                     const size_t len,
                     const char delim,
                     const regex_t* rgx,
                     size_t* ncols)
{
    assert(csv_line);
    assert(rgx);
    assert(ncols);

    if (!len) {
        XSVT_ERROR("Empty line found!");
        return XSVT_CHK_WRONG_FORMAT;
    }
    if (csv_line[0] == delim) {
        XSVT_ERROR("First charactor shoule not be delimiter!");
        return XSVT_CHK_WRONG_FORMAT;
    }
    if (csv_line[len - 1] == delim) {
        XSVT_ERROR("Delimiter found at last position of csv line!");
        return XSVT_CHK_WRONG_FORMAT;
    }

    size_t nc = 1;
    for (char* pos = strchr(csv_line, delim); pos; pos = strchr(pos + 1, delim)) {
        nc += 1;
    }
    if (nc == 1) {
        XSVT_WARN("Only one column in csv!");
        return XSVT_CHK_ONLY_ONE_COLUMN;
    }

    if (*ncols == 0) {
        *ncols = nc;
    } else {
        if (*ncols != nc) {
            XSVT_ERROR("Number of columns is not equal to previous");
            return XSVT_CHK_WRONG_FORMAT;
        }
    }

    if (nc > XSVT_MAX_LINE_LEN / 100) {
        XSVT_WARN("Warning: found %d columns, too many!", XSVT_MAX_LINE_LEN / 100);
    }

    if (regexec(rgx, csv_line, 0, 0, 0) != REG_NOERROR) {
        return XSVT_CHK_WRONG_FORMAT;
    }

    return XSVT_CHK_OK;
}


char* xsvt_checker_error_msg(xsvt_checker d)
{
    XSVT_ASSERT(d && d->errmsg);
    char buf[128] = { 0 };

    switch (d->error) {
        case XSVT_CHK_OK:
            snprintf(buf, sizeof(buf)-1, "CSV Check Passed!");
            break;

        case XSVT_CHK_EMPTY_FILE:
        case XSVT_CHK_FILE_TOO_LARGE:
        case XSVT_CHK_ONLY_ONE_LINE:
        case XSVT_CHK_LINE_TOO_LONG:
        case XSVT_CHK_ONLY_ONE_COLUMN:
            snprintf(buf, sizeof(buf)-1, "CSV Check Failed: %s!", d->errmsg[d->error]);
            break;

        case XSVT_CHK_WRONG_FORMAT:
            snprintf(buf, sizeof(buf)-1, "CSV Check Failed: %s at line %lu", d->errmsg[d->error], d->nrows);
            break;

        default:
            snprintf(buf, sizeof(buf)-1, "CSV Check Failed: %s!", "Unknown Error Type");
            break;
    };

    return strdup(buf);
}


int xsvt_checker_execute(xsvt_checker d)
{
    XSVT_CHKPTR(d);
    XSVT_CHKPTR(d->__csv__);

    rewind(d->__csv__);
    fseeko64(d->__csv__, 0, SEEK_END);
    d->fsize = ftello64(d->__csv__);
    char* line = NULL;

    if (d->fsize > XSVT_MAX_FILE_SIZE) {
        d->error = XSVT_CHK_FILE_TOO_LARGE; // huge file, more than 4GB.
        goto _CHECK_DESC_RETURN_;
    }
    if (d->fsize == 0) {
        d->error = XSVT_CHK_EMPTY_FILE; // empty file
        goto _CHECK_DESC_RETURN_;
    }

    const size_t capacity = XSVT_MAX_LINE_LEN;
    line = (char*)xsvt_alloc(capacity);

    rewind(d->__csv__);
    size_t len = xsvt_readline(d->__csv__, line, capacity);

    if (len == XSVT_READLINE_EMPTY) {
        d->error = XSVT_CHK_WRONG_FORMAT;
        goto _CHECK_DESC_RETURN_;
    }
    if (len == XSVT_READLINE_OVERFLOW) {
        d->error = XSVT_CHK_LINE_TOO_LONG;
        goto _CHECK_DESC_RETURN_;
    }

    XSVT_INFO("Init line length: %lu", len);

    if (xsvt_build_line_regex(line, capacity, d->delim, d->__rgx__)) {
        d->error = XSVT_CHK_WRONG_FORMAT;
        goto _CHECK_DESC_RETURN_;
    }

    rewind(d->__csv__);

    while(xsvt_readline_neof(d->__csv__, line, capacity, &len)) {
        ++d->nrows;
        if (len == XSVT_READLINE_OVERFLOW) {
            d->error = XSVT_CHK_LINE_TOO_LONG;
            goto _CHECK_DESC_RETURN_;
        }
        d->error = xsvt_check_format(line, len, d->delim, d->__rgx__, &d->ncols);
        if (d->error != XSVT_CHK_OK) {
            break;
        }
        if (d->uc) {
            d->error = d->uc(line);
            if (d->error != XSVT_CHK_OK) {
                break;
            }
        }
    };

    if (d->error == XSVT_CHK_OK && d->nrows == 1) {
        d->error = XSVT_CHK_ONLY_ONE_LINE;   // only one line, maybe just header, right or wrong?
    }

    _CHECK_DESC_RETURN_:
    if (line) {
        free(line);
    }
    rewind(d->__csv__);

    return d->error;
}


int xsvt_check_file(const char* csv, const char delim)
{
    XSVT_CHKPTR(csv, return EINVAL);
    XSVT_INFO("Input:      %s", csv);

    if (delim == XSVT_DELIM_TAB) {
        XSVT_INFO("Delimeter:  Tab");
    } else {
        XSVT_INFO("Delimeter:  \"%c\"", delim);
    }

    xsvt_checker d = xsvt_checker_create(csv, delim, NULL);
    if (NULL == d) {
        XSVT_ERROR("xsvt_checker_create failed!");
        xsvt_checker_release(&d);
        return -1;
    }

    int err = xsvt_checker_execute(d);

    if (d->fsize > 1024 * 1024) {
        XSVT_INFO("Filesize: %.3fMB", d->fsize / (double)(1024*1024));
    }
    else if (d->fsize > 1024) {
        XSVT_INFO("Filesize: %.3fKB", d->fsize / (double)(1024));
    }
    else {
        XSVT_INFO("Filesize: %lu bytes", d->fsize);
    }

    if (XSVT_CHK_OK == err) {
        XSVT_INFO("Lines:    %lu", d->nrows);
        XSVT_INFO("Columns:  %lu", d->ncols);
    }

    char* errmsg = xsvt_checker_error_msg(d);
    XSVT_INFO("%s", errmsg);
    free(errmsg);

    xsvt_checker_release(&d);

    return err;
}




