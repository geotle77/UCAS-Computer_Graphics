#include "mesh.h"
#include <Eigen/Dense>
#include <queue>

static Eigen::Matrix4d ComputeQem(vertex ver)
{

}



/**
 * Simplifies the mesh by reducing the number of faces to the target value.
 * 
 * @param target The target number of faces.
 */
void Mesh::simplify(int target)
{
    vector<Eigen::Matrix4d> QEM(vertices.size());
    for(auto& vertex:vertices )
    {
        QEM.push_back(ComputeQem(vertex));
    }
}