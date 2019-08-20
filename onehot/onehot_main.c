/**
 * Author: alexloser
 * Create on 2019
 * Driver of xsvt.onehot
 */
#include <stdio.h>
#include <stdlib.h>
#include "xsvt/onehot/onehot.h"

#ifndef ARGI_EQ
#define ARGI_EQ(x) !strcmp(argv[i], x)
#endif
#ifndef ARGI_CHK
#define ARGI_CHK() if (i+1 >= argc) usage_and_exit(argv[0])
#endif


static void usage_and_exit(const char* path)
{
    fprintf(stderr, "Usage: %s [OPTIONS]\n", path);
    fprintf(stderr, "  -h : Show this help\n");
    fprintf(stderr, "  -i : Input filename(.csv) to encode\n");
    fprintf(stderr, "  -o : Output filename(.csv) to save results\n");
    fprintf(stderr, "  -d : Delimiter of csv file\n");
    fprintf(stderr, "  -k : Has csv header at first line, 1 or 0, default is 1(has)\n");
    fprintf(stderr, "  -w : Write header in output file, 1 or 0, default is 1(write)\n");
    fprintf(stderr, "  -c : Check csv format, only check, no encoding work!\n");
    fprintf(stderr, "Example:\n");
    fprintf(stderr, "    %s -i input.csv -c\n", path);
    fprintf(stderr, "    %s -i input.csv -o out.csv -d \",\" -w 1 -k 1\n", path);
    fprintf(stderr, "    %s -i input.csv -o out.csv -w 1 -k 0\n", path);
    fprintf(stderr, "Notice:\n");
    fprintf(stderr, "    Using -c option to check the format before encoding is a good idea!!!\n\n");
    exit(-1);
}


int main(int argc, char* argv[], char* env[])
{
    int i = 1;
    if (argc < 2 || ARGI_EQ("-h") || ARGI_EQ("--help")) {
        usage_and_exit(argv[0]);
    }

    const char* in_csv = NULL;
    const char* out_csv = NULL;

    char delim = XSVT_DELIM_DEFAULT;

    int has_head = 1;
    int write_head = 1;
    int only_check = 0;

    for (i = 1; i < argc; ++i) {
        if (ARGI_EQ("-h") || ARGI_EQ("--help")) {
            usage_and_exit(argv[0]);
        }
        else if (ARGI_EQ("-i")) {
            ARGI_CHK();
            in_csv = argv[++i];
        }
        else if (ARGI_EQ("-o")) {
            ARGI_CHK();
            out_csv = argv[++i];
        }
        else if (ARGI_EQ("-d")) {
            ARGI_CHK();
            delim = argv[++i][0];
            if (ARGI_EQ("\\t")) { delim = '\t'; }
        }
        else if (ARGI_EQ("-k")) {
            ARGI_CHK();
            has_head = (strcmp(argv[++i], "0") == 0) ? 0 : 1;
        }
        else if (ARGI_EQ("-w")) {
            ARGI_CHK();
            write_head = (strcmp(argv[++i], "0") == 0) ? 0 : 1;
        }
        else if (ARGI_EQ("-c")) {
            only_check = 1;
        }
        else {
            XSVT_ERROR("Wrong option: \"%s\"\n", argv[i]);
            return -1;
        }
    }

    if (!in_csv || !out_csv) {
        XSVT_ERROR("-i and -o options must be specified!\n");
        return -1;
    }

    int err = 0;
    if (only_check) {
        err = xsvt_check_file(in_csv, delim);
        return err;
    }

    err = xsvt_oh_encode(in_csv, out_csv, delim, has_head, write_head);
    if (err) {
        BRUTE_FATAL_EXIT(err, "Failed!");
    } 

    BRUTE_EXIT("Onehot encoding finished, see %s\n", out_csv);

}



