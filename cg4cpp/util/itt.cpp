
class weird {
    union {
        __CGswizzle<T,1,1,0x00> x;
        __CGswizzle<T,1,1,0x00> r;
        __CGswizzle<T,1,1,0x00> s;
        __CG_SWIZZLE4_STARTING_WITH_X(x,y,z,w);
        __CG_SWIZZLE4_STARTING_WITH_X(r,g,b,a);
        __CG_SWIZZLE4_STARTING_WITH_X(s,t,p,q);
    };
    union {
        __CGswizzle<T,1,1,0x00> y;
        __CGswizzle<T,1,1,0x00> g;
        __CGswizzle<T,1,1,0x00> t;
        __CG_SWIZZLE3_STARTING_WITH_Y_NOT_X(y,z,w);
        __CG_SWIZZLE3_STARTING_WITH_Y_NOT_X(g,b,a);
        __CG_SWIZZLE3_STARTING_WITH_Y_NOT_X(t,p,q);
    };
    union {
        __CGswizzle<T,1,1,0x00> z;
        __CGswizzle<T,1,1,0x00> b;
        __CGswizzle<T,1,1,0x00> p;
        __CG_SWIZZLE2_STARTING_WITH_Z_NOT_XY(z,w);
        __CG_SWIZZLE2_STARTING_WITH_Z_NOT_XY(b,a);
        __CG_SWIZZLE2_STARTING_WITH_Z_NOT_XY(p,q);
    };
    union {
        __CGswizzle<T,1,1,0x00> w;
        __CGswizzle<T,1,1,0x00> a;
        __CGswizzle<T,1,1,0x00> q;
        __CG_SWIZZLE1_STARTING_WITH_W_NOT_XYZ(z,w);
        __CG_SWIZZLE1_STARTING_WITH_W_NOT_XYZ(b,a);
        __CG_SWIZZLE1_STARTING_WITH_W_NOT_XYZ(p,q);
    };
};
