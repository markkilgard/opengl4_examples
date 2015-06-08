
// monkey.cpp - data for triangles for monkey head

#include "monkey.hpp"

#ifdef _MSC_VER
#pragma warning( disable : 4305 )
#endif

#include "MonkeyHead.h"

#include <list>
#include <vector>
#include <map>

#include <Cg/vector.hpp>
#include <Cg/all.hpp>

using std::map;
using std::vector;
using std::list;
using std::pair;
using namespace Cg;

bool less_than(const float3 &a, const float3 &b)
{
    // For each component...
    for (int i=0; i<3; i++) {
        if (a[i] < b[i]) {
            return true;
        } else if (a[i] > b[i]) {
            return false;
        }
    }
    // Is equal...
    return false;
}

struct Edge {
    float3 v0, v1;

    Edge(const float3 &a, const float3 &b) {
        if (less_than(a, b)) {
            v0 = a;
            v1 = b;
        } else {
            v0 = b;
            v1 = a;
        }
    }

    bool operator < (const Edge &other) const {
        if (less_than(v0, other.v0)) {
            return true;
        }
        if (less_than(other.v0, v0)) {
            return false;
        }
        // equal v0's...
        if (less_than(v1, other.v1)) {
            return true;
        }
        if (less_than(other.v1, v1)) {
            return false;
        }
        return false;
    }
};

typedef int Face;

typedef list<Face> FaceList;
typedef map<Edge,FaceList> EdgeMap;

EdgeMap edge_map;

void makeEdgeMap(int num_tris, const int3 tri_ndxs[],
                 int num_verts, const float3 v[])
{
    for (int i=0; i<num_tris; i++) {
        for (int j=0; j<3; j++) {
            Edge e(v[tri_ndxs[i][j]], v[tri_ndxs[i][(j+1)%3]]);
            EdgeMap::iterator e_iter = edge_map.find(e);
            if (e_iter != edge_map.end()) {
                e_iter->second.push_back(i);
            } else {
                FaceList face_list(1,i);
                edge_map[e] = face_list;
            }
        }
    }

    for (int i=0; i<num_tris; i++) {
        printf("tri: %d\n", i);
        for (int j=0; j<3; j++) {
            printf("  edg: %d\n", j);
            pair<int,int> ndx(tri_ndxs[i][j], tri_ndxs[i][(j+1)%3]);
            if (ndx.first > ndx.second) {
                std::swap(ndx.first, ndx.second);
            }
            Edge e(v[ndx.first], v[ndx.second]);
            FaceList face_list = edge_map[e];
            for (FaceList::iterator iter = face_list.begin(); iter != face_list.end(); iter++) {
                int face = *iter;
                if (face != i) {
                    printf("    other %d\n", face);
                    bool found_match = false;
                    for (int z=0; z<3; z++) {
                        pair<int,int> ndx2(tri_ndxs[face][z], tri_ndxs[face][(z+1)%3]);
                        if (ndx2.first > ndx2.second) {
                            std::swap(ndx2.first, ndx2.second);
                        }

                        if (ndx == ndx2) {
                            printf("match\n");
                            found_match = true;
                            break;
                        }
                    }
                    if (!found_match) {
                        int3 ndxs = tri_ndxs[face];
                        for (int z=0; z<3; z++) {
                            if (all(e.v0 == v[ndxs[z]])) {
                                printf("        ndx=%d\n", z);
                            } else if (all(e.v1 == v[ndxs[z]])) {
                                printf("        ndx=%d\n", z);
                            }
                        }
                    }
                }
            }
        }
    }
}
