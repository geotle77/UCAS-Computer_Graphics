#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <set> 
#include <string>
#include <Eigen/Dense>
using namespace std;
struct vertex
{
    float x;
    float y;
    float z;
    set<int> adjacent_faces;
};
struct edge
{
    int v1;
    int v2;
    bool operator<(const edge &other) const
    {
        return std::tie(v1,v2)<std::tie(other.v1,other.v2);
    }
};
struct face
{
    int v1;
    int v2;
    int v3;
};
struct Normal
{
    int face_index;
    float x;
    float y;
    float z;
};

class Mesh
{
public:
    vector<vertex> vertices;
    vector<face> faces;
    vector<Normal> normals;
    set<edge> edges;
    vector<edge> edgesVec;
    int face_num;
    int vertex_num;
    void loadmodel(string filename);
    void calNormal();
    void simplify(int target);
    void export2Obj(vector<vertex> vertices, vector<face> faces);
    
private:
    Eigen::Matrix4d ComputePlaneEquation(face& f);
    Eigen::Matrix4d ComputeQem(vertex ver);
    float ComputeEdgeCost(const edge &e,vector<Eigen::Matrix4d>& QEM);
    edge FindMincostEdge(vector<Eigen::Matrix4d>& QEM);
    void CollapseEdge(edge e,vector<Eigen::Matrix4d>& QEM);
};
Mesh Dragon;

void Mesh::loadmodel(string filename)
{
    ifstream file(filename);
    string line;
    while (getline(file, line))
    {
        stringstream ss(line);
        string type;
        ss >> type;
        if (type == "v")
        {
            vertex v;
            ss >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        }
        else if (type == "f")
        {
            face f;
            ss >> f.v1 >> f.v2 >> f.v3;
            f.v1--; 
            f.v2--;
            f.v3--;
            faces.push_back(f);
            int newFaceIndex = faces.size() - 1;
            vertices[f.v1].adjacent_faces.insert(newFaceIndex);
            vertices[f.v2].adjacent_faces.insert(newFaceIndex);
            vertices[f.v3].adjacent_faces.insert(newFaceIndex);
        }
    }
    for(auto &face:faces){
        edges.insert({std::min(face.v1, face.v2), std::max(face.v1, face.v2)});
        edges.insert({std::min(face.v2, face.v3), std::max(face.v2, face.v3)});
        edges.insert({std::min(face.v3, face.v1), std::max(face.v3, face.v1)});
    }
    for(auto &edge:edges){
        edgesVec.push_back(edge);
    }
    file.close();
    face_num = faces.size();
    vertex_num = vertices.size();
    int edge_num = edges.size();
    printf("the face number is %d\n", face_num);
    printf("the vertex number is %d\n", vertex_num);
    printf("the edge number is %d\n", edge_num);

}

void Mesh::calNormal()
{
   for(auto &face:faces){
        Normal normal;
        normal.face_index = &face - &faces[0];
        Eigen::Vector3f v1 = Eigen::Vector3f(vertices[face.v1].x, vertices[face.v1].y, vertices[face.v1].z);
        Eigen::Vector3f v2 = Eigen::Vector3f(vertices[face.v2].x, vertices[face.v2].y, vertices[face.v2].z);
        Eigen::Vector3f v3 = Eigen::Vector3f(vertices[face.v3].x, vertices[face.v3].y, vertices[face.v3].z);
        Eigen::Vector3f normal_vector = (v2 - v1).cross(v3 - v1);
        normal_vector.normalize(); // 归一化法向量
        normal.x = normal_vector.x();
        normal.y = normal_vector.y();
        normal.z = normal_vector.z();
        normals.push_back(normal);
   }
}

Eigen::Matrix4d Mesh :: ComputePlaneEquation(face& f)
{
    Eigen::Matrix4d FlatMatrix;
    int face_index = &f - &faces[0];
    Eigen::Vector3f normal = Eigen::Vector3f(normals[face_index].x, normals[face_index].y, normals[face_index].z);
    Eigen::Vector3f v1 = Eigen::Vector3f(vertices[f.v1].x, vertices[f.v1].y, vertices[f.v1].z);
    Eigen::Vector3f point = v1;
    double d = -normal.dot(point);
    FlatMatrix << normal.x() * normal.x(), normal.x() * normal.y(), normal.x() * normal.z(), normal.x() * d,
        normal.x() * normal.y(), normal.y() * normal.y(), normal.y() * normal.z(), normal.y() * d,
        normal.x() * normal.z(), normal.y() * normal.z(), normal.z() * normal.z(), normal.z() * d,
        normal.x() * d, normal.y() * d, normal.z() * d, d * d;
    return FlatMatrix;
}

Eigen::Matrix4d Mesh:: ComputeQem(vertex ver)
{
    Eigen::Matrix4d QEM = Eigen::Matrix4d::Zero();
     if (ver.adjacent_faces.empty())
    {
        return QEM;
    }
    for (auto &face_index : ver.adjacent_faces)
    {
        QEM += ComputePlaneEquation(faces[face_index]);
    }
    return QEM;
}
float Mesh::ComputeEdgeCost(const edge &e,vector<Eigen::Matrix4d>& QEMs)
{
    Eigen::Vector4d v1 = Eigen::Vector4d(vertices[e.v1].x, vertices[e.v1].y, vertices[e.v1].z, 1);
    Eigen::Vector4d v2 = Eigen::Vector4d(vertices[e.v2].x, vertices[e.v2].y, vertices[e.v2].z, 1);

    Eigen::Matrix4d QEM = QEMs[e.v1] + QEMs[e.v2];

    Eigen::Vector4d v = (v1 + v2) / 2; // The position of the vertex after collapsing the edge

    return v.transpose() * QEM * v;
}

edge Mesh::FindMincostEdge(vector<Eigen::Matrix4d>& QEM)
{
    double min_cost = std::numeric_limits<double>::max();
    edge min_cost_edge;
    for (auto &e : edgesVec)
    {
        double cost = ComputeEdgeCost(e,QEM);
        if (cost < min_cost)
        {
            min_cost = cost;
            min_cost_edge = e;
        }
    }
    return min_cost_edge;
}

void Mesh::CollapseEdge(edge e, vector<Eigen::Matrix4d>& QEM)
{
    Eigen::Vector4d v1 = Eigen::Vector4d(vertices[e.v1].x, vertices[e.v1].y, vertices[e.v1].z, 1);
    Eigen::Vector4d v2 = Eigen::Vector4d(vertices[e.v2].x, vertices[e.v2].y, vertices[e.v2].z, 1);
    Eigen::Vector4d v = (v1 + v2) / 2; // The position of the vertex after collapsing the edge
    vertex new_vertex;
    new_vertex.x = v.x();
    new_vertex.y = v.y();
    new_vertex.z = v.z();
    vertices.push_back(new_vertex);
    vertices.erase(vertices.begin() + std::max(e.v1, e.v2));
    vertices.erase(vertices.begin() + std::min(e.v1, e.v2));
    int new_vertex_index = vertices.size()-1;

    for(auto &edge:edgesVec){
        if(edge.v1 == e.v1 || edge.v1 == e.v2) {
            edge.v1 = new_vertex_index;
        }
        if(edge.v2 == e.v1 || edge.v2 == e.v2) {
            edge.v2 = new_vertex_index;
        }
        if(edge.v1 > e.v1) {
            edge.v1--;
        }
        if(edge.v1 > e.v2) {
            edge.v1--;
        }
        if(edge.v2 > e.v1) {
            edge.v2--;
        }
        if(edge.v2 > e.v2) {
            edge.v2--;
        }
    }

    for (auto& face : faces) {
        if (face.v1 == e.v1 || face.v1 == e.v2) {
            face.v1 = new_vertex_index;
        }
        if (face.v2 == e.v1 || face.v2 == e.v2) {
            face.v2 = new_vertex_index;
        }
        if (face.v3 == e.v1 || face.v3 == e.v2) {
            face.v3 = new_vertex_index;
        }
        if (face.v1 > e.v1) {
        face.v1--;
        }
        if (face.v1 > e.v2) {
            face.v1--;
        }
        if (face.v2 > e.v1) {
            face.v2--;
        }
        if (face.v2 > e.v2) {
            face.v2--;
        }
        if (face.v3 > e.v1) {
            face.v3--;
        }
        if (face.v3 > e.v2) {
            face.v3--;
        }
    }

    // Remove edges associated with the deleted vertices
    // Remove edges associated with the deleted vertices
    edgesVec.erase(std::remove_if(edgesVec.begin(), edgesVec.end(), [&](const edge& edge) {
    return edge.v1 == edge.v2;
    }), edgesVec.end());

    // Remove faces associated with the deleted vertices
    faces.erase(std::remove_if(faces.begin(), faces.end(), [&](const face& face) {
        return face.v1 == face.v2 || face.v1 == face.v3 || face.v2 == face.v3;
    }), faces.end());

    // Update the QEM vector
    QEM[new_vertex_index] = QEM[e.v1] + QEM[e.v2];
    QEM.erase(QEM.begin() + std::max(e.v1, e.v2));
    QEM.erase(QEM.begin() + std::min(e.v1, e.v2));
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
    printf("the QEM calculated\n");
    while (faces.size()>target)
    {
        edge min_cost_edge = FindMincostEdge(QEM);
        CollapseEdge(min_cost_edge,QEM);
    }
}


void Mesh::export2Obj(vector<vertex> vertices, vector<face> faces)
{
    ofstream file("simplified_dragon.obj");
    file << "# " << vertices.size() << " " << faces.size() << endl;
    for (int i = 0; i < vertices.size(); i++)
    {
        file << "v " << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << endl;
    }
    for (int i = 0; i < faces.size(); i++)
    {
        file << "f " << faces[i].v1 << " " << faces[i].v2 << " " << faces[i].v3 << endl;
    }
    file.close();
}