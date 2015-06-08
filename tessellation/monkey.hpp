
// monkey.hpp - header for triangles for monkey head

#include <Cg/vector.hpp>

using namespace Cg;

extern const int MonkeyHead_num_of_vertices;
extern const int MonkeyHead_num_of_triangles;
extern const float3 MonkeyHead_vertices[];
extern const float3 MonkeyHead_normals[];
extern const int3 MonkeyHead_triangles[];

void makeEdgeMap(int num_tris, const int3 tri_ndxs[],
                 int num_verts, const float3 v[]);
