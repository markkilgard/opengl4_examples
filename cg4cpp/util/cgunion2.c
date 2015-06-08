
#include <assert.h>
#include <string.h>
#include <stdio.h>

static char *COMP[4] = {
  "X",
  "Y_NOT_X",
  "Z_NOT_XY",
  "W_NOT_XYZ"
};

static char *comp[4] = {
  "_x",
  "_y",
  "_z",
  "_w",
};

static char *arg[4] = {
  "_x,_y,_z,_w",
  "_y,_z,_w",
  "_z,_w",
  "_w",
};

static char *arg2[4] = {
  "_x",
  "_x,_y",
  "_x,_y,_z",
  "_x,_y,_z,_w",
};

void swiz_string(int count, unsigned int v)
{
  int i;

  printf("%s", comp[v & 0x3]);
  v >>= 2;
  for (i=1; i<count; i++) {
    int a = v & 0x3;
    printf("##%s", comp[a]);
    v >>= 2;
  }
  printf("; ");
}

const char *mask(int first_comp, int count, unsigned int v)
{
  static char str[100];
  static char *name[4] = {
    "00",
    "01",
    "02",
    "03",
  };
  int i;
//printf("mask(%d,%d,0x%x)\n", first_comp, count, v);
  str[0] = '\0';
  for (i=count-1; i>=0; i--) {
    int a = (v >> (2*i)) & 0x3;
    //printf("a=%d, i=%d, v=%d, first_comp=%d\n", a, i, v, first_comp);
    //printf("%d %s\n", i, name[a-first_comp]);
    assert(a-first_comp >= 0);
    strcat(str, name[a-first_comp]);
    //printf("%d Z %s\n", i, name[a-first_comp]);
  }
  return str;
}

void func(int first_comp, int c, int max_comp)
{
  int count;
  int storage;
  unsigned int i, j, k;
  int astorage = max_comp - first_comp + 1;

  if (first_comp > max_comp) {
    return;
  }

  printf("#define __CG_SWIZZLE%d_STARTING_WITH_%s(%s) ",
    astorage,
    COMP[first_comp],
    arg2[max_comp]+first_comp*3);
  //for (count=c; count>1; count--) {
  for (count=2; count<=c; count++) {
    unsigned int max = 1;
    for (i=0; i<count; i++) {
      max *= 4;
    }
    for (j=0; j<max; j++) {
      {
        int emit = 0;
        int maxc = 0;
        int m;
        int is_const = 0;
        for (k=0; k<count; k++) {
          int v = ((j >> (2*k)) & 0x3);
          if (v < first_comp) {
            goto skip;
          }
        }
        for (k=0; k<count; k++) {
          int v = ((j >> (2*k)) & 0x3);
          if (v == first_comp) {
            emit = 1;
          }
          if (v > maxc) {
            maxc = v;
          }
        }
        if (maxc != max_comp) {
          goto skip;
        }
        if (!emit) {
            goto skip;
        }
        for (k=0; k<count; k++) {
          int v = ((j >> (2*k)) & 0x3);
          for (m=k+1; m<count; m++) {
            int v2 = ((j >> (2*m)) & 0x3);
            if (v == v2) {
              is_const = 1;
            }
          }
        }
        storage = maxc - first_comp + 1;
        printf("\\\n  %s__CGswizzle%s<T,%d,%d,0x%s> ",
          is_const ? "__CGconst " : "",
          is_const ? "_const" : "",
          storage, count, mask(first_comp, count, j));
        swiz_string(count, j);
      }
      skip:;
    }
  }
#if 0
  printf("\n");
#else
  if (max_comp > 0 && first_comp < max_comp) {
    printf("\\\n  __CG_SWIZZLE%d_STARTING_WITH_%s(%s)\n",
      astorage-1,
      COMP[first_comp],
      arg2[max_comp-1] + first_comp*3);
  } else {
    printf("\n");
  }
#endif
}

void func2(int first_comp, int c)
{
  int max_comp;
  for (max_comp=0; max_comp<=3; max_comp++) {
    func(first_comp, c, max_comp);
  }
}

int main(int argc, char **argv)
{
  int i, j;

  for (j=0; j<4; j++) {
    func2(j, 4);
  }
}

