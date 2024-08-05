

#include "bmp.h"
#include "io.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define OPTIONS "i:o:h"
// Note: this assignment doesn’t use : in the third argument to getopt
int main(int argc, char **argv) {
    char *filename;
    char *filenameout;
    int opt = 0;
    int casebit = 0;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i':
            casebit = (casebit | 1);
            filename = optarg;
            break;
        case 'o':
            casebit = (casebit | 2);
            filenameout = optarg;
            break;
        case 'h': casebit = (casebit | 4); break;
        }
    }
    //move here
    if (casebit != 3) {
        if (casebit == 1) {
            printf("colorb:  -o option is required\n");
        }
        if (casebit == 2) {
            printf("colorb:  -i option is required\n");
        }
        casebit = 4;
    }
    if (casebit & (1 << 0)) {
        //printf("option i with %s as a parameter.\n", filename);

        Buffer *b = read_open(filename);
        assert(b != NULL);
        BMP *bmp = bmp_create(b);
        assert(bmp != NULL);
        bmp_reduce_palette(bmp); //here
        Buffer *wb = write_open(filenameout);
        bmp_write(bmp, wb);
        read_close(&b);
        bmp_free(&bmp);
        write_close(&wb);
    }
    if (casebit & (1 << 2)) {
        printf("Usage: colorb -i infile -o outfile\n");
    }
}
