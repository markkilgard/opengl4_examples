
// This code generates the union creation macros for cgvector.h

#include <stdio.h>

const int needsConstBool(int cmps, int x, int y, int z, int w)
{
    int yes = 0;

    switch (cmps) {
    case 4:
        yes |= (x == w);
        yes |= (y == w);
        yes |= (z == w);
    case 3:
        yes |= (x == z);
        yes |= (y == z);
    case 2:
        yes |= (x == y);
    case 1:
        break;
    }
    return yes;
}

const char *needsConst(int cmps, int x, int y, int z, int w)
{
    int yes = needsConstBool(cmps, x, y, z, w);

    return yes ? "const " : "";
}

int newMode = 1;

char *swizzleNumbers(int cmps, int x, int y, int z, int w)
{
    static char buf[100];

    if (newMode) {
        unsigned int mask = 0;

        mask |= w;
        mask <<= 8;
        mask |= z;
        mask <<= 8;
        mask |= y;
        mask <<= 8;
        mask |= x;
        switch (cmps) {
        case 1:
            sprintf(buf, "0x%02x", mask);
            break;
        case 2:
            sprintf(buf, "0x%04x", mask);
            break;
        case 3:
            sprintf(buf, "0x%06x", mask);
            break;
        case 4:
            sprintf(buf, "0x%08x", mask);
            break;
        }
    } else {
        switch (cmps) {
    case 1:
        sprintf(buf, "%d", x);
        break;
    case 2:
        sprintf(buf, "%d,%d", x, y);
        break;
    case 3:
        sprintf(buf, "%d,%d,%d", x, y, z);
        break;
    case 4:
        sprintf(buf, "%d,%d,%d,%d", x, y, z, w);
        break;
        }
    }
    return buf;
}

char *swizzleNumbersList(int cmps, int x, int y, int z, int w)
{
    static char buf[100];

    switch (cmps) {
    case 1:
        sprintf(buf, "%d", x);
        break;
    case 2:
        sprintf(buf, "%d,%d", x, y);
        break;
    case 3:
        sprintf(buf, "%d,%d,%d", x, y, z);
        break;
    case 4:
        sprintf(buf, "%d,%d,%d,%d", x, y, z, w);
        break;
    }
    return buf;
}

char *swizzleSuffix(int cmps, int x, int y, int z, int w)
{
    static char buf[100];

    switch (cmps) {
    case 1:
        sprintf(buf, "_%c", "xyzw"[x]);
        break;
    case 2:
        sprintf(buf, "_%c##_%c", "xyzw"[x], "xyzw"[y]);
        break;
    case 3:
        sprintf(buf, "_%c##_%c##_%c", "xyzw"[x], "xyzw"[y], "xyzw"[z]);
        break;
    case 4:
        sprintf(buf, "_%c##_%c##_%c##_%c", "xyzw"[x], "xyzw"[y], "xyzw"[z], "xyzw"[w]);
        break;
    }
    return buf;
}

char *swizzleRowNames(int row, int cmps, int x, int y, int z, int w)
{
    static char buf[100];
    static const char *baseZero = "0123";
    static const char *baseOne = "1234";

    switch (cmps) {
    case 1:
        sprintf(buf, "_m%d%c, _%d%c",
            row, baseZero[x],
            row+1, baseOne[x]);
        break;
    case 2:
        sprintf(buf, "_m%d%c_m%d%c, _%d%c_%d%c",
            row, baseZero[x],
            row, baseZero[y],
            row+1, baseOne[x],
            row+1, baseOne[y]);
        break;
    case 3:
        sprintf(buf, "_m%d%c_m%d%c_m%d%c, _%d%c_%d%c_%d%c",
            row, baseZero[x],
            row, baseZero[y],
            row, baseZero[z],
            row+1, baseOne[x],
            row+1, baseOne[y],
            row+1, baseOne[z]);
        break;
    case 4:
        sprintf(buf, "_m%d%c_m%d%c_m%d%c_m%d%c, _%d%c_%d%c_%d%c_%d%c",
            row, baseZero[x],
            row, baseZero[y],
            row, baseZero[z],
            row, baseZero[w],
            row+1, baseOne[x],
            row+1, baseOne[y],
            row+1, baseOne[z],
            row+1, baseOne[w]);
        break;
    }
    return buf;
}

char *swizzleColNames(int col, int cmps, int x, int y, int z, int w)
{
    static char buf[100];
    static const char *baseZero = "0123";
    static const char *baseOne = "1234";

    switch (cmps) {
    case 1:
        sprintf(buf, "_m%c%d, _%c%d",
            baseZero[x], col,
            baseOne[x], col+1);
        break;
    case 2:
        sprintf(buf, "_m%c%d_m%c%d, _%c%d_%c%d",
            baseZero[x], col,
            baseZero[y], col,
            baseOne[x], col+1,
            baseOne[y], col+1);
        break;
    case 3:
        sprintf(buf, "_m%c%d_m%c%d_m%c%d, _%c%d_%c%d_%c%d",
            baseZero[x], col,
            baseZero[y], col,
            baseZero[z], col,
            baseOne[x], col+1,
            baseOne[y], col+1,
            baseOne[z], col+1);
        break;
    case 4:
        sprintf(buf, "_m%c%d_m%c%d_m%c%d_m%c%d, _%c%d_%c%d_%c%d_%c%d",
            baseZero[x], col,
            baseZero[y], col,
            baseZero[z], col,
            baseZero[w], col,
            baseOne[x], col+1,
            baseOne[y], col+1,
            baseOne[z], col+1,
            baseOne[w], col+1);
        break;
    }
    return buf;
}

const char *swizzleParams[] = {
    "_x",
    "_x,_y",
    "_x,_y,_z",
    "_x,_y,_z,_w"
};
const char *swizzleVariant[] = {
    "",
    "X",
    "XY",
    "XYZ",
    "XYZW"
};

int biggest(int x, int y, int z, int w)
{
    int biggest = x;

    if (y > biggest) {
        biggest = y;
    }
    if (z > biggest) {
        biggest = z;
    }
    if (w > biggest) {
        biggest = w;
    }
    return biggest;
}

int main(int argc, char **argv)
{
    int size, ocmps, x, y, z, w;
    //int rows, cols;
    int row, col;

    for (size=1; size<=4; size++) {
        printf("#define __CG_SWIZZLES_%s(N,%s)", swizzleVariant[size], swizzleParams[size-1]);
        for (ocmps=1; ocmps<=4; ocmps++) {
            int xvals = size;
            for (x=0; x<size; x++) {
                int yvals = (ocmps >= 2) ? size : 1;
                for (y=0; y<yvals; y++) {
                    int zvals = (ocmps >= 3) ? size : 1;
                    for (z=0; z<zvals; z++) {
                        int wvals = (ocmps >= 4) ? size : 1;
                        for (w=0; w<wvals; w++) {
                            int big = biggest(x,y,z,w);
                            if (big == size-1) {
                                printf(" \\\n        %scgVectorSwizzle<T,N,%d,%s> %s;",
                                    needsConst(ocmps, x, y, z, w),
                                    ocmps,
                                    swizzleNumbers(ocmps, x, y, z, w),
                                    swizzleSuffix(ocmps, x, y, z, w));
                            }
                        }
                    }
                }
            }
        }
        printf("\n\n");
    }

    col = 1;
    row = 1;

    for (row=1; row<=4; row++) {
        for (col=1; col<=4; col++) {
            size = col;
            printf("#define __CG_MATRIX_SWIZZLE_%dx%d()", row, col);
            for (ocmps=1; ocmps<=4; ocmps++) {
                int xvals = size;
                for (x=0; x<size; x++) {
                    int yvals = (ocmps >= 2) ? size : 1;
                    for (y=0; y<yvals; y++) {
                        int zvals = (ocmps >= 3) ? size : 1;
                        for (z=0; z<zvals; z++) {
                            int wvals = (ocmps >= 4) ? size : 1;
                            for (w=0; w<wvals; w++) {
                                int big = biggest(x,y,z,w);
                                if (big == size-1) {
                                    if (needsConstBool(ocmps, x, y, z, w)) {
                                        // Nothing for now
                                    } else {
                                        printf(" \\\n        %s__CG_MATRIX_ROW_SWIZZLE%d(%d, %s) %s;",
                                            needsConst(ocmps, x, y, z, w),
                                            ocmps,
                                            row-1,
                                            swizzleNumbersList(ocmps, x, y, z, w),
                                            swizzleRowNames(row-1, ocmps, x, y, z, w));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        ///
            size = row;
            for (ocmps=1; ocmps<=4; ocmps++) {
                int xvals = size;
                for (x=0; x<size; x++) {
                    int yvals = (ocmps >= 2) ? size : 1;
                    for (y=0; y<yvals; y++) {
                        int zvals = (ocmps >= 3) ? size : 1;
                        for (z=0; z<zvals; z++) {
                            int wvals = (ocmps >= 4) ? size : 1;
                            for (w=0; w<wvals; w++) {
                                int big = biggest(x,y,z,w);
                                if (big == size-1) {
                                    if (needsConstBool(ocmps, x, y, z, w) || ocmps==1) {
                                        // Nothing for now
                                    } else {
                                        printf(" \\\n        %s__CG_MATRIX_COL_SWIZZLE%d(%d, %s) %s;",
                                            needsConst(ocmps, x, y, z, w),
                                            ocmps,
                                            col-1,
                                            swizzleNumbersList(ocmps, x, y, z, w),
                                            swizzleColNames(col-1, ocmps, x, y, z, w));
                                    }
                                }
                            }
                        }
                    }
                }
            }
            printf("\n\n");
        }
    }

    return 0;
}
