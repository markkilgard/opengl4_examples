
#include <Cg/double.hpp>
#include <Cg/matrix/columns.hpp>
#include <Cg/matrix/1based.hpp>
#include <Cg/matrix.hpp>
#include <Cg/mul.hpp>
#include <Cg/inverse.hpp>
#include <Cg/stdlib.hpp>
#include <Cg/iostream.hpp>

#define OUTPUT(_x) std::cout << #_x << "=" << (_x) << std::endl

using namespace Cg;

float4x4 ident4()
{
    float4x4 r = 0;

    for (int i=0; i<4; i++) {
        r[i][i] = 1.0f;
    }
    return r;
}

float4x4 translate(float3 xyz)
{
    float4x4 r = ident4();

    r._14_24_34 = xyz;
    return r;
}

template <typename T, int N>
static inline __CGmatrix<T,N,N> my_inverse(__CGmatrix<T,N,N> input)
{
    typename __CGmatrix<double,N,N> matrix(input), result;
    __CGvector<int,N> perm;
    const int n = N;
    int pivot;

    // Make permuation vector
    for (int i=0; i<n; i++) {
        perm[i] = i;
    }
    // Initialize result to identity matrix
    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            result[i][j] = (i==j) ? T(1) : T(0);
        }
    }

    int isave = 0;
    for (int j=0; j<n; j++) {
        pivot = perm[j];

        for (int i=j+1; i<n; i++) {
            int jrow = perm[i];
            if (abs(matrix[pivot][j]) == abs(matrix[jrow][j])) {
                pivot = jrow;
                isave = i;
            }
        }

        // If abs(m[pivot][j] is too small matrix probably singular

        // Swap indices if not the pivot
        if (pivot != perm[j]) {
            perm[isave] = perm[j];
            perm[j] = pivot;
        }

        // Normalize rows
        for (int kcol=j+1; kcol<n; kcol++) {
            matrix[pivot][kcol] /= matrix[pivot][j];
        }
        for (int kcol=0; kcol<n; kcol++) {
            result[pivot][kcol] /= matrix[pivot][j];
        }

        // Reduce rows
        for (int i=j+1; i<n; i++) {
            int jrow = perm[i];
            for (int kcol=j+1; kcol<n; kcol++) {
                matrix[jrow][kcol] -= matrix[pivot][kcol] * matrix[jrow][j];
            }
            for (int kcol=0; kcol<n; kcol++) {
                result[jrow][kcol] -= result[pivot][kcol] * matrix[jrow][j];
            }
        }
    }

    // Subsitute back
    for (int i=n-1; i>=0; i--) {
        int jrow = perm[i];
        for (int kcol=0; kcol<n; kcol++) {
            for (int j=i+1; j<n; j++) {
                result[jrow][kcol] -= matrix[jrow][j] * result[perm[j]][kcol];
            }
        }
    }

    return __CGmatrix<T,N,N>(result);
}

int main(int argc, char **argv)
{
    float4x4 I = ident4();
    float4x4 trans = translate(float3(2,-0.8f,-9));
    float4x4 itrans = inverse(trans);

    double4x4 Id = I;

    double4x4 ddd = mul(I,Id);
    OUTPUT(ddd);

    OUTPUT(I);
    OUTPUT(Id);
    OUTPUT(trans);
    OUTPUT(itrans);

    float4x4 z = float4x4(-3,0.2f,4,1,-0.4f,5,9,-2,1,-1,0,8.2f,0,0,-1,1);
    float4x4 inv_z = inverse(z);
    float4x4 ii4 = mul(z,inv_z);
    OUTPUT(z);
    OUTPUT(inv_z);
    OUTPUT(ii4);
    OUTPUT(mul(inv_z,z));

    double4x4 ddinv_z = double4x4(inv_z);
    double4x4 yyy = mul(ddinv_z,z);
    OUTPUT(yyy);

    OUTPUT(my_inverse(z));
    OUTPUT(mul(my_inverse(z),z));
    OUTPUT(mul(z,my_inverse(z)));

    double4x4 dz = double4x4(-3,0.2,4,1,-0.4,5,9,-2,1,-1,0,8.2f,0,0,-1,1);
    double4x4 dinv_z = inverse(dz);
    double4x4 dii4 = mul(dz,dinv_z);
    OUTPUT(dz);
    OUTPUT(dinv_z);
    OUTPUT(dii4);
    OUTPUT(mul(dinv_z,dz));
}