
#define __CG_SWIZZLE_STARTING_WITH_W_NOT_XYZ_2(_w) \
    __CGswizzle<T,2,0x0000> _w##_w;

#define __CG_SWIZZLE_STARTING_WITH_W_NOT_XYZ_3(_w) \
    __CGswizzle<T,3,0x000000> _w##_w##_w; \
    __CG_SWIZZLE_STARTING_WITH_W_NOT_XYZ_2(_w)

#define __CG_SWIZZLE_STARTING_WITH_W_NOT_XYZ_4(_w) \
    __CGswizzle<T,4,0x00000000> _w##_w##_w##_w; \
    __CG_SWIZZLE_STARTING_WITH_W_NOT_XYZ_3(_w)

#define __CG_SWIZZLE_STARTING_WITH_X_4(_x,_y,_z,_w) \
    __CGswizzle<T,4,0x00000000> _x##_x##_x##_x; \
    __CGswizzle<T,4,0x01000000> _x##_x##_x##_y; \
    __CG_SWIZZLE_STARTING_WITH_X_3(_x,_y,_z,_w)

template <typename T, int N, int mask>
class __CGswizzle {
  T v[N];
};

template <typename T>
class doesnt_work {
  union {
    struct {
      union {
        __CGswizzle<T,1,1,0x00> x;
        __CGswizzle<T,1,1,0x00> r;
        __CGswizzle<T,1,1,0x00> s;
      };
      union {
        struct {
          union {
            __CGswizzle<T,1,1,0x00> y;
            __CGswizzle<T,1,1,0x00> g;
            __CGswizzle<T,1,1,0x00> t;
          };
          union {
            struct {
              union {
                __CGswizzle<T,1,1,0x00> z;
                __CGswizzle<T,1,1,0x00> b;
                __CGswizzle<T,1,1,0x00> p;
              };
              union {
                __CGswizzle<T,1,1,0x00> w;
                __CGswizzle<T,1,1,0x00> a;
                __CGswizzle<T,1,1,0x00> q;
                // all multi-component swizzles containing w (and not x nor y nor z)
                __CG_SWIZZLE_STARTING_WITH_W_NOT_XYZ_4(w);
                __CG_SWIZZLE_STARTING_WITH_W_NOT_XYZ_4(a);
                __CG_SWIZZLE_STARTING_WITH_W_NOT_XYZ_4(q);
              };
            };
            // all multi-component swizzles containing z (and not x nor y)
            __CG_SWIZZLE_STARTING_WITH_Z_NOT_XY_4(z,w);
            __CG_SWIZZLE_STARTING_WITH_Z_NOT_XY_4(b,a);
            __CG_SWIZZLE_STARTING_WITH_Z_NOT_XY_4(p,q);
          };
        };
        // all multi-component swizzles containing y (and not x)
        __CG_SWIZZLE_STARTING_WITH_Y_NOT_X_4(y,z,w);
        __CG_SWIZZLE_STARTING_WITH_Y_NOT_X_4(g,b,a);
        __CG_SWIZZLE_STARTING_WITH_Y_NOT_X_4(t,p,q);
      };
    };
    // all multi-component swizzles containing x
    __CG_SWIZZLE_STARTING_WITH_X_4(x,y,z,w);
    __CG_SWIZZLE_STARTING_WITH_X_4(r,g,b,a);
    __CG_SWIZZLE_STARTING_WITH_X_4(s,t,p,q);
  };
};
