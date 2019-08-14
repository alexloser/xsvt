/**
 *  Author: alexloser
 *  Create on 2019
 *  Source file of xsvt.cvt2
 */
#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include "xsvt/transfer/transfer.h"

#define XSVT_TINY_DOUBLE (1.0e-20)

#define XSVT_SVM_PATTERN ("^[-+]?[\\.0-9]+[eE]?[-+]?[0-9]* ([0-9]+:[-+]?[\\.0-9]+[eE]?[-+]?[0-9]* ?)+$")


xsvt_transfer xsvt_transfer_create(const char* input,
                                   const char* output,
                                   const char* label,
                                   const char delim,
                                   const int csvheader,
                                   const int direction,
                                   xsvt_transfer_user_cb uc)
{
    XSVT_CHKPTR(input, return NULL);
    XSVT_CHKPTR(output, return NULL);
    XSVT_CHKPTR(label, return NULL);
    XSVT_ASSERT(delim, return NULL);

    xsvt_transfer t = (xsvt_transfer)xsvt_alloc(sizeof(struct __xsvt_transfer__));

    t->input = strdup(input);
    t->output = strdup(output);
    t->label = strdup(label);
    t->_delim = delim;
    t->_header = csvheader;
    t->_dir = direction;
    t->_uc = uc;

    t->_fin = fopen64(input, "rb");
    if (!t->_fin) {
        xsvt_transfer_destroy(&t);
        XSVT_ERROR("Open %s for read failed!", input);
        return NULL;
    }

    t->_fout = fopen64(output, "wb");
    if (!t->_fout) {
        xsvt_transfer_destroy(&t);
        XSVT_ERROR("Open %s for write failed!", output);
        return NULL;
    }

    return t;
}


void xsvt_transfer_destroy(xsvt_transfer* _t)
{
    XSVT_CHKPTR(_t, return);
    XSVT_CHKPTR(*_t, return);

    xsvt_transfer t = *_t;

    if (t->input)
        free((char*)t->input);

    if (t->output)
        free((char*)t->output);

    if (t->label)
        free((char*)t->label);

    if (t->_fin)
        fclose(t->_fin);

    if (t->_fout) {
        fflush(t->_fout);
        fclose(t->_fout);
    }

    bzero(t, sizeof(struct __xsvt_transfer__));
    free(t);

    *_t = NULL;
}

static int xsvt_transfer_compilex(regex_t* rgx_int, regex_t* rgx_flt)
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

static size_t load_label4_c2s(xsvt_transfer t, double** _label_vec)
{
    XSVT_CHKPTR(_label_vec, return 0);
    XSVT_ASSERT(t->_dir == CSV_TO_SVM);
    if (*_label_vec) {
        XSVT_WARN("`_label_vec` should be NULL before initialize");
    }

    char line[BUFSIZ] = {0};
    size_t size = 0;

    FILE* f = fopen(t->label, "rb");
    if (!f) {
        XSVT_ERROR("Open `%s` failed!", t->label);
        return 0;
    }
    while (XSVT_GETLINE(line, sizeof(line), f)) {
        ++size;
    }
    if (!size) {
        XSVT_ERROR("Load label file failed!");
        fclose(f);
        return 0;
    }

    *_label_vec = (double*)xsvt_alloc(size * sizeof(double));

    rewind(f);
    for (int i = 0; XSVT_GETLINE(line, sizeof(line), f); ++i) {
        (*_label_vec)[i] = atof(line);
    }

    fclose(f);
    return size;
}

int xsvt_csv2svm(xsvt_transfer t)
{
    XSVT_CHKPTR(t, return EINVAL);
    XSVT_CHKPTR(t->_fin, return EINVAL);
    XSVT_CHKPTR(t->_fout, return EINVAL);
    XSVT_ASSERT(t->_dir == CSV_TO_SVM);

    xsvt_checker d = xsvt_checker_create(t->input, t->_delim, NULL);
    XSVT_CHKPTR(d, xsvt_checker_release(&d); return EINVAL);

    int err = xsvt_checker_execute(d);

    if (d->fsize > 1024 * 1024) {
        XSVT_INFO("Filesize: %.3fMB", d->fsize / (double)(1024*1024));
    } else if (d->fsize > 1024) {
        XSVT_INFO("Filesize: %.3fKB", d->fsize / (double)(1024));
    } else {
        XSVT_INFO("Filesize: %lu bytes", d->fsize);
    }
    if (XSVT_CHK_OK == err) {
        XSVT_INFO("Lines:    %lu", d->nrows);
        XSVT_INFO("Columns:  %lu", d->ncols);
    }

    const char* errmsg = xsvt_checker_error_msg(d);
    XSVT_INFO("%s", errmsg);
    free((char*)errmsg);

    size_t ncols = d->ncols;
    xsvt_checker_release(&d);

    const size_t capacity = XSVT_MAX_LINE_LEN;
    char* line = (char*)xsvt_alloc(capacity);

    const size_t slices_size = (XSVT_MAX_LINE_LEN / 32);
    xsvt_slice*  slices = (xsvt_slice*)xsvt_alloc(XSVT_MAX_LINE_LEN / 32);
    XSVT_ASSERT(slices_size > 1000);

    size_t nlines = 0;

    rewind(t->_fout);
    rewind(t->_fin);
    if (t->_header) { // skip csv head line
        xsvt_readline(t->_fin, line, capacity);
        nlines += 1;
    }

    regex_t rgx_int = {0}, rgx_flt = {0};
    err = xsvt_transfer_compilex(&rgx_int, &rgx_flt);
    XSVT_ASSERT(err == REG_NOERROR);

    double* label_vec = NULL;
    const size_t nlabels = load_label4_c2s(t, &label_vec);

    size_t len = 0;
    size_t idx = 0;

    while (xsvt_readline_neof(t->_fin, line, capacity, &len)) {
        nlines += 1;
        if (len == XSVT_READLINE_OVERFLOW) {
            XSVT_ERROR("Too huge line length at %lu", nlines);
            err = -1; break;
        }
        if (len == XSVT_READLINE_EMPTY) {
            XSVT_ERROR("Empty line or wrong format line at %lu", nlines);
            err = -1; break;
        }

        if (ncols != xsvt_tokenize(line, len, t->_delim, slices, slices_size)) {
            XSVT_ERROR("Number of columns at line %lu is not equal to previous!!!", nlines);
            err = -1; break;
        }

        if (idx+1 > nlabels) {
            XSVT_ERROR("Total lines in label file: %lu, buf current line in csv: %lu", nlabels, idx+1);
            err = ERANGE; goto _CSV2SVM_RETURN_;
        }

        fprintf(t->_fout, "%g ", label_vec[idx++]);

        for (int k = 0; k < ncols; ++k) {
            const char* s = slices[k].begin;
            if (regexec(&rgx_int, s, 0, 0, 0) != REG_NOERROR) {
                if (regexec(&rgx_flt, s, 0, 0, 0) != REG_NOERROR) {
                    XSVT_ERROR("CSV field: `%s` at line %lu is invalid(not numeric value)!", s, nlines);
                    err = EDOM; goto _CSV2SVM_RETURN_;
                }
            }
            if (fabs(atof(s)) < XSVT_TINY_DOUBLE) {
                continue;
            }
            fprintf(t->_fout, "%d:%s ", (k + 1), s);
        }
        fseeko64(t->_fout, -1, SEEK_CUR);
        fwrite("\n", 1, 1, t->_fout);
    }

    if (idx != nlabels) {
        XSVT_WARN("Total lines in label file: %lu, buf total lines in csv: %lu", nlabels, idx);
    } else {
        XSVT_INFO("Total lines in svm file: %lu", idx);
    }

    _CSV2SVM_RETURN_:
    if (label_vec)
        free(label_vec);
    if (line)
        free(line);
    if (slices)
        free(slices);
    regfree(&rgx_int);
    regfree(&rgx_flt);

    return err;
}


static void save_label4_s2c(xsvt_transfer t)
{
    XSVT_CHKPTR(t, return);
    XSVT_ASSERT(t->_dir == SVM_TO_CSV);

    char line[BUFSIZ] = { 0 };

    FILE* fsvm = fopen(t->label, "w");
    rewind(t->_fin);
    while (XSVT_GETLINE(line, sizeof(line), t->_fin)) {
        fprintf(fsvm, "%f\n", atof(line));
    }

    fflush(fsvm);
    fclose(fsvm);
}


static size_t xsvt_svm_check_max_column(const char* name, regex_t* rgx_svm, size_t* _nlines)
{
    XSVT_CHKPTR(name, return 0);
    FILE* f = fopen64(name, "rb");
    XSVT_CHKPTR(f, XSVT_ERROR("Open `%s` failed!", name); return 0);

    size_t maxcol = 0;
    size_t len = 0, nlines = 0;

    const size_t capacity = XSVT_MAX_LINE_LEN;
    char* line = (char*)xsvt_alloc(capacity);

    while (xsvt_readline_neof(f, line, capacity, &len)) {
        nlines += 1;
        if (len == XSVT_READLINE_OVERFLOW) {
            XSVT_ERROR("Too huge line length at %lu", nlines);
            maxcol = 0; break;
        }
        if (len == XSVT_READLINE_EMPTY) {
            XSVT_ERROR("Empty line or wrong format line at %lu", nlines);
            maxcol = 0; break;
        }
        if (regexec(rgx_svm, line, 0, 0, 0) != REG_NOERROR) {
            XSVT_ERROR("Wrong format line at %lu in `%s`", nlines, name);
            maxcol = 0; break;
        }
        for (char* pos = strchr(line, ' '); pos; pos = strchr(pos+1, ' '))
            if (atol(pos) > maxcol)
                maxcol = atol(pos);
    }

    if (_nlines) {
        *_nlines = nlines;
    }
    free(line);
    fclose(f);
    return maxcol;
}

int xsvt_svm2csv(xsvt_transfer t)
{
    XSVT_CHKPTR(t, return EINVAL);
    XSVT_CHKPTR(t->_fin, return EINVAL);
    XSVT_CHKPTR(t->_fout, return EINVAL);
    XSVT_ASSERT(t->_dir == SVM_TO_CSV);

    regex_t rgx_svm = {0};
    int err = regcomp(&rgx_svm, XSVT_SVM_PATTERN, REG_EXTENDED);
    XSVT_ASSERT(err == REG_NOERROR);

    size_t nrows = 0;
    size_t ncols = xsvt_svm_check_max_column(t->input, &rgx_svm, &nrows);
    if (!ncols) {
        regfree(&rgx_svm);
        XSVT_ERROR("Format of svm file is wrong!");
        return -1;
    } else {
        XSVT_INFO("Max column of svm file: %lu", ncols);
        XSVT_INFO("Number lines of svm file: %lu", nrows);
    }

    const size_t capacity = XSVT_MAX_LINE_LEN;
    char* line = (char*)xsvt_alloc(capacity);

    size_t len = 0;

    rewind(t->_fin);
    rewind(t->_fout);
    while (xsvt_readline_neof(t->_fin, line, capacity, &len)) {
        if (line[len - 1] == ' ') {
            line[--len] = 0;
        }
        size_t idx = 1, nzb = 1; // the col index of libsvm is start from 1, not 0, so...
        for (const char* pk = strchr(line, ' '); pk; pk = strchr(pk + 1, ' ')) {
            nzb = atoi(pk);
            assert(nzb >= 1);
            for (; idx < nzb; ++idx) {
                fprintf(t->_fout, "0%c", t->_delim);
            }
            idx = nzb + 1;
            const char* pv = strchr(pk, ':');
            fprintf(t->_fout, "%f%c", atof(pv + 1), t->_delim);
        }
        fseeko64(t->_fout, -1, SEEK_CUR);
        fwrite("\n", 1, 1, t->_fout);
    }

    if (line) {
        free(line);
    }
    regfree(&rgx_svm);

    save_label4_s2c(t);

    return 0;
}


int xsvt_transform( const char* input, const char* output,  const char* label,
                    const char  delim, const int csvheader, const int direction )
{
    XSVT_CHKPTR(input, return EINVAL);
    XSVT_CHKPTR(output, return EINVAL);
    XSVT_CHKPTR(label, return EINVAL);
    XSVT_ASSERT(delim, return EINVAL);

    XSVT_INFO("Input:  %s", input);
    XSVT_INFO("Output: %s", output);
    XSVT_INFO("Label:  %s", label);
    XSVT_INFO("Delimeter: \"%c\"", delim);
    XSVT_WARN("Has header: %s", (csvheader ? "True" : "False"));
    XSVT_WARN("The first line will be skipped because specified `has header` option");
    XSVT_INFO("Direction %s", (direction == CSV_TO_SVM ? "csv=>svm" : "svm=>csv"));
    assert(direction == CSV_TO_SVM || direction == SVM_TO_CSV);

    xsvt_transfer t = xsvt_transfer_create(input, output, label, delim, csvheader, direction, NULL);
    if (NULL == t) {
        XSVT_ERROR("Create xsvt_transfer failed!");
        xsvt_transfer_destroy(&t);
        return -1;
    }

    int err = 0;

    if (direction == CSV_TO_SVM) {
        err = xsvt_csv2svm(t);
    } else {
        err = xsvt_svm2csv(t);
    }

    if (err) {
        BRUTE_LOG(ERROR, "Transform failed");
    } else {
        XSVT_INFO("Transform finished!");
    }

    xsvt_transfer_destroy(&t);
    return 0;
}





