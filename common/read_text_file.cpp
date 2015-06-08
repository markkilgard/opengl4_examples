
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include "read_text_file.h"

char *read_text_file(const char *filename)
{
#if defined(_WIN32)
    struct _stat f_stat;
    #define statfunc _stat
#else
    struct stat f_stat;
    #define statfunc stat
#endif

    if (!filename) 
        return 0;

    if (statfunc(filename, &f_stat) == 0) {
        FILE *fp = 0;
        if (!(fp = fopen(filename, "r"))) {
            fprintf(stderr,"Cannot open \"%s\" for read!\n", filename);
            return 0;
        }

        long size = f_stat.st_size;
        char * buf = new char[size+1];

        size_t bytes;
        bytes = fread(buf, 1, size, fp);

        buf[bytes] = 0;

        fclose(fp);
        return buf;
    }

    fprintf(stderr,"Cannot open \"%s\" for stat read!\n", filename);

    return 0;
}
