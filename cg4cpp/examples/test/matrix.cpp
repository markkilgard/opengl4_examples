
#include <stdio.h>
#include <iostream>
#include <Cg/vector/xyzw.hpp>
#include <Cg/vector.hpp>
#include <Cg/iostream.hpp>

#define OUTPUT(_x) std::cout << #_x << "=" << (_x) << std::endl

using namespace Cg;

int main(int argc, char **argv)
{
    float4 a = float4(0),
           b = float4(1);

    OUTPUT(a);
    OUTPUT(b);
    a.yz = b.zy;
    OUTPUT(a);
    OUTPUT(b);
    getchar();
}