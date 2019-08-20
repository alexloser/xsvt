/**
 *  Author: alexloser
 *  Main file of xsvt.csvchk
 */
#include <assert.h>
#include <stdlib.h>
#include "xsvt/checker/checker.h"

#ifndef ARGI_EQ
#define ARGI_EQ(x) !strcmp(argv[i], x)
#endif
#ifndef ARGI_CHK
#define ARGI_CHK() if (i+1 >= argc) usage_and_exit(argv[0])
#endif

static void usage_and_exit(const char* path)
{
    fprintf(stderr, "Usage: %s [OPTIONS]\n", path);
    fprintf(stderr, "  -i : input csv file to check.\n");
    fprintf(stderr, "  -d : delimiter of csv file, default is comma.\n");
    fprintf(stderr, "  -h : show this help.\n\n");
    exit(-1);
}


int main(int argc, char* argv[])
{
    if (argc < 2) {
        usage_and_exit(argv[0]);
        return 0;
    }

    const char* csv = NULL;
    char delim = XSVT_DELIM_DEFAULT;

    for (int i = 1; i < argc; ++i) {
        if (ARGI_EQ("-i")) {
            ARGI_CHK();
            csv = argv[++i];
        }
        else if (ARGI_EQ("-d")) {
            ARGI_CHK();
            delim = argv[++i][0];
            if (ARGI_EQ("\\t")) {
                delim = '\t';
            }
        }
        else {
            usage_and_exit(argv[0]);
        }
    }

    switch (delim) {
    case XSVT_DELIM_COMMA:
    case XSVT_DELIM_TAB:
    case XSVT_DELIM_SPACE:
        break;
    default:
        XSVT_ERROR("Error: csv delimiter \"%c\" is not correct!\n", delim);
        return -1;
    }

    if (!csv) {
        BRUTE_FATAL_EXIT(-1, "Error: can not find csv file path from main arguments!\n");
    }

    int err = xsvt_check_file(csv, delim);
    BRUTE_FATAL_EXIT_IF(err, err, "\n");

    BRUTE_EXIT("\n");
}

