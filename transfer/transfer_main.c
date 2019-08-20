/**
 * Driver of xsvt.cvt2
 */
#include <stdio.h>
#include <stdlib.h>
#include "xsvt/transfer/transfer.h"

#ifndef ARGI_EQ
#define ARGI_EQ(x) !strcmp(argv[i], x)
#endif
#ifndef ARGI_CHK
#define ARGI_CHK() if (i+1 >= argc) usage_and_exit(argv[0])
#endif


static void usage_and_exit(const char* path)
{
    fprintf(stderr, "Usage: %s [OPTIONS]\n", path);
    fprintf(stderr, "  -h : Show this help.\n");
    fprintf(stderr, "  -i : Input filename(csv or svm) to convert.\n");
    fprintf(stderr, "  -o : Output filename to save csv or svm format.\n");
    fprintf(stderr, "  -l : label filename for read(when csv=>svm) or write(when svm=>csv).\n");
    fprintf(stderr, "  -d : Delimiter of csv file, default is comma.\n");
    fprintf(stderr, "  -k : Has csv header at first line, 1 or 0, default is 1(has).\n");
    fprintf(stderr, "  -s2c : SVM => CSV.\n");
    fprintf(stderr, "  -c2s : CSV => SVM.\n");
    fprintf(stderr, "Example: convert in.csv to out.svm\n");
    fprintf(stderr, "  %s -c2s -i in.csv -l in_label.csv -o out.svm -d \",\" -k 1\n", path);
    fprintf(stderr, "Example: convert in.svm to out.csv\n");
    fprintf(stderr, "  %s -s2c -i in.svm -o out.csv -l out_label.csv\n\n", path);

    exit(-1);
}


int main(int argc, char* argv[], char* env[])
{
    if (argc < 3) {
        usage_and_exit(argv[0]);
    }

    const char* input = NULL;
    const char* output = NULL;
    const char* label = NULL;

    char delim = XSVT_DELIM_DEFAULT;

    int header = 1;
    int s2c = -1;
    int c2s = -1;

    for (int i = 1; i < argc; ++i) {
        if (ARGI_EQ("-h") || ARGI_EQ("--help")) {
            usage_and_exit(argv[0]);
        }
        else if (ARGI_EQ("-i")) { ARGI_CHK(); input = argv[++i]; }
        else if (ARGI_EQ("-o")) { ARGI_CHK(); output = argv[++i]; }
        else if (ARGI_EQ("-l")) { ARGI_CHK(); label = argv[++i]; }
        else if (ARGI_EQ("-k")) { ARGI_CHK(); header = (strcmp(argv[++i], "0") == 0) ? 0 : 1; }
        else if (ARGI_EQ("-d")) {
            ARGI_CHK();
            delim = argv[++i][0];
            if (ARGI_EQ("\\t")) {
                delim = '\t';
            }
        }
        else if (ARGI_EQ("-s2c")) {
            if (c2s > 0) {
                XSVT_ERROR("Option `-s2c` and `-c2s` can choose only one!\n");
                return -1;
            }
            s2c = 1;
        }
        else if (ARGI_EQ("-c2s")) {
            if (s2c > 0) {
                XSVT_ERROR("Option `-s2c` and `-c2s` can choose only one!\n");
                return -1;
            }
            c2s = 1;
        }
        else {
            XSVT_ERROR("Wrong option: \"%s\"\n", argv[i]);
            return -1;
        }
    }

    if (!input || !output || !label) {
        XSVT_ERROR("Options -i, -o and -l must be specified!\n");
        return -1;
    }

    switch (delim) {
        case XSVT_DELIM_COMMA:
        case XSVT_DELIM_TAB:
        case XSVT_DELIM_SPACE:
            break;
        default:
            XSVT_ERROR("Option -d: csv delimiter \"%c\" is not correct!\n", delim);
            return -1;
    }

    if (s2c < 0 && c2s < 0) {
        XSVT_ERROR("Option `-s2c` or `-c2s` must be specified!\n");
        return -1;
    }

    int err = xsvt_transform(input, output, label, delim, header, (s2c > 0 ? SVM_TO_CSV : CSV_TO_SVM));
    if (err) {
        BRUTE_FATAL_EXIT(err, "Failed!");
    }
    
    BRUTE_EXIT("\n");
}



