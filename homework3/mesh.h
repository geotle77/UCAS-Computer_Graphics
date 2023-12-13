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
    vector<int> adjacent_faces;
};
struct edge
{
    int v1;
    int v2;
    float cost;
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
    Eigen::Vector3f normal;
};

class Mesh
{
public:
    vector<vertex> vertices;
    vector<face> faces;
    set<edge> edges;
    vector<edge> edgesVec;
    vector<Eigen::Matrix4f> QEM;
    int face_num;
    int vertex_num;
    void loadmodel(string filename);
    void calNormal();
    void simplify(int target);
    void export2Obj(vector<vertex> vertices, vector<face> faces);
    
private:
    void recal_normal(face& face);
    Eigen::Matrix4f ComputePlaneEquation(face& f);
    Eigen::Matrix4f ComputeQem(vertex& ver);
    void ComputeEdgeCost(vector<edge> &edgesVec);
    edge FindMincostEdge();
    void updateEdgeCost(edge &e);
    void CollapseEdge(edge e,vector<Eigen::Matrix4f>& QEM);
};
Mesh Dragon;

void insert_unique(std::vector<int>& vec, int element) {
    if (std::find(vec.begin(), vec.end(), element) == vec.end()) {
        vec.push_back(element);
    }
}
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
            insert_unique(vertices[f.v1].adjacent_faces, newFaceIndex);
            insert_unique(vertices[f.v2].adjacent_faces, newFaceIndex);
            insert_unique(vertices[f.v3].adjacent_faces, newFaceIndex);
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
         Eigen::Vector3f v1 = Eigen::Vector3f(vertices[face.v1].x, vertices[face.v1].y, vertices[face.v1].z);
         Eigen::Vector3f v2 = Eigen::Vector3f(vertices[face.v2].x, vertices[face.v2].y, vertices[face.v2].z);
         Eigen::Vector3f v3 = Eigen::Vector3f(vertices[face.v3].x, vertices[face.v3].y, vertices[face.v3].z);
         Eigen::Vector3f normal = (v2 - v1).cross(v3 - v1);
         normal.normalize();
         face.normal = normal;
   }
}
/**
 * Simplifies the mesh by reducing the number of faces to the target value.
 * 
 * @param target The target number of faces.
 */
void Mesh::simplify(int target)
{
    Eigen::Matrix4f QEM0 = Eigen::Matrix4f::Zero();
    for(auto& vertex:vertices )
    {
        if(vertex.adjacent_faces.size() == 0){
            QEM.push_back(QEM0);
            continue;
        }
        Eigen::Matrix4f ver_Q = ComputeQem(vertex);
        QEM.push_back(ver_Q);
    }

    ComputeEdgeCost(edgesVec);
    while (faces.size()>target)
    {
        edge min_cost_edge = FindMincostEdge();
        CollapseEdge(min_cost_edge,QEM);
    }

}

Eigen::Matrix4f Mesh :: ComputePlaneEquation(face& f)
{
    Eigen::Matrix4f FlatMatrix;
    int face_index = &f - &faces[0];
    Eigen::Vector3f normal = faces[face_index].normal;
    Eigen::Vector3f v1 = Eigen::Vector3f(vertices[f.v1].x, vertices[f.v1].y, vertices[f.v1].z);
    Eigen::Vector3f point = v1;
    double d = -normal.dot(point);
    FlatMatrix << normal.x() * normal.x(), normal.x() * normal.y(), normal.x() * normal.z(), normal.x() * d,
        normal.x() * normal.y(), normal.y() * normal.y(), normal.y() * normal.z(), normal.y() * d,
        normal.x() * normal.z(), normal.y() * normal.z(), normal.z() * normal.z(), normal.z() * d,
        normal.x() * d, normal.y() * d, normal.z() * d, d * d;
    return FlatMatrix;
}

Eigen::Matrix4f Mesh:: ComputeQem(vertex& ver)
{
    Eigen::Matrix4f ver_Q = Eigen::Matrix4f::Zero();
    for (auto &face_index : ver.adjacent_faces)
    {
        ver_Q += ComputePlaneEquation(faces[face_index]);
    }
    int vertex_index = &ver - &vertices[0];
    return ver_Q;
}
void Mesh::ComputeEdgeCost(vector<edge> &edgesVec)
{
    for(auto &edge:edgesVec){
        Eigen::Vector4f v1 = Eigen::Vector4f(vertices[edge.v1].x, vertices[edge.v1].y, vertices[edge.v1].z, 1);
        Eigen::Vector4f v2 = Eigen::Vector4f(vertices[edge.v2].x, vertices[edge.v2].y, vertices[edge.v2].z, 1);
        Eigen::Vector4f v = (v1 + v2) / 2; // The position of the vertex after collapsing the edge
        Eigen::Matrix4f edge_Q = QEM[edge.v1] + QEM[edge.v2];
        Eigen::Vector4f v4 = Eigen::Vector4f(v.x(), v.y(), v.z(), 1);
        float cost = v4.transpose() * edge_Q * v4;
        if(cost<0) edge.cost=0;
        else edge.cost = cost;
    }
}
void Mesh::updateEdgeCost(edge &e){
    Eigen::Vector4f v1 = Eigen::Vector4f(vertices[e.v1].x, vertices[e.v1].y, vertices[e.v1].z, 1);
    Eigen::Vector4f v2 = Eigen::Vector4f(vertices[e.v2].x, vertices[e.v2].y, vertices[e.v2].z, 1);
    Eigen::Vector4f v = (v1 + v2) / 2; // The position of the vertex after collapsing the edge
    Eigen::Matrix4f new_Q = QEM[e.v1] + QEM[e.v2];
    Eigen::Vector4f v4 = Eigen::Vector4f(v.x(), v.y(), v.z(), 1);
    float cost = v4.transpose() * new_Q * v4;
    if(cost<0) e.cost=0;
    else e.cost = cost;
}

edge Mesh::FindMincostEdge()
{
    edge min_cost_edge = edgesVec[0];
    float min_cost = edgesVec[0].cost;
    for(auto &edge:edgesVec){
        if(edge.cost < min_cost){
            min_cost = edge.cost;
            min_cost_edge = edge;
        }
    }
    return min_cost_edge;
}
void Mesh::recal_normal(face& face)
{

    Eigen::Vector3f v1 = Eigen::Vector3f(vertices[face.v1].x, vertices[face.v1].y, vertices[face.v1].z);
    Eigen::Vector3f v2 = Eigen::Vector3f(vertices[face.v2].x, vertices[face.v2].y, vertices[face.v2].z);
    Eigen::Vector3f v3 = Eigen::Vector3f(vertices[face.v3].x, vertices[face.v3].y, vertices[face.v3].z);
    Eigen::Vector3f normal = (v2 - v1).cross(v3 - v1);
    normal.normalize();
    face.normal = normal;
}

void Mesh::CollapseEdge(edge e,vector<Eigen::Matrix4f>& QEM)
{
    Eigen::Vector4f v1 = Eigen::Vector4f(vertices[e.v1].x, vertices[e.v1].y, vertices[e.v1].z, 1);
    Eigen::Vector4f v2 = Eigen::Vector4f(vertices[e.v2].x, vertices[e.v2].y, vertices[e.v2].z, 1);
    Eigen::Vector4f v = (v1 + v2) / 2; // The position of the vertex after collapsing the edge
    vertex new_vertex;
    new_vertex.x = v.x();
    new_vertex.y = v.y();
    new_vertex.z = v.z();
    vertices.push_back(new_vertex);
    int new_vertex_index = vertices.size() - 1;
   for(auto &edge:edgesVec){
        bool updated = false;
        if((edge.v1 == e.v1 && edge.v2 != e.v2) || (edge.v2 == e.v1 && edge.v1 != e.v2)){
            edge.v1 = new_vertex_index;
            updated = true;
        }
        if((edge.v1 == e.v2 && edge.v2 != e.v1) || (edge.v2 == e.v2 && edge.v1 != e.v1)){
            edge.v2 = new_vertex_index;
            updated = true;
        }
        if(updated){
            updateEdgeCost(edge);
        }
    }
    
    vector<int>& face_idx1 = vertices[e.v1].adjacent_faces;
    vector<int>& face_idx2 = vertices[e.v2].adjacent_faces;
    vector<int> delete_face_idx;
    for(auto &face_idx:face_idx1 ){ 
       face & f= faces[face_idx];
        if(f.v1 != e.v2 && f.v2 != e.v2 && f.v3 != e.v2){
            if(f.v1 == e.v1) {f.v1 = new_vertex_index;insert_unique(vertices[new_vertex_index].adjacent_faces,face_idx);}
            if(f.v2 == e.v1) {f.v2 = new_vertex_index;insert_unique(vertices[new_vertex_index].adjacent_faces,face_idx);}
            if(f.v3 == e.v1) {f.v3 = new_vertex_index;insert_unique(vertices[new_vertex_index].adjacent_faces,face_idx);}
        }
        else{
            int face_index = &f - &faces[0];
            delete_face_idx.push_back(face_index);
            // 更新 f 的顶点的 adjacent_faces
          

            // 更新所有大于 face_index 的 adjacent_faces
            for (auto& vertex : vertices) {
                for (auto& f_idx : vertex.adjacent_faces) {
                    if (f_idx > face_index) {
                        f_idx--;
                    }
                }
            }

            faces.erase(faces.begin() + face_index);
        }
    }
    // for (int vertex_index : {f.v1, f.v2, f.v3}) {
    //             auto& adjacent_faces = vertices[vertex_index].adjacent_faces;
    //             adjacent_faces.erase(std::remove(adjacent_faces.begin(), adjacent_faces.end(), face_index), adjacent_faces.end());
    //     }
    for(auto &face_idx:face_idx2){
        face & f= faces[face_idx];
        if(f.v1 != e.v1 && f.v2 != e.v1 && f.v3 != e.v1){
            bool deleted= false;
            for(auto idx:delete_face_idx){
                if(face_idx == idx) deleted = true;
            }
            if(deleted) continue; 
            if(f.v1 == e.v2) {f.v1 = new_vertex_index;insert_unique(vertices[new_vertex_index].adjacent_faces,face_idx);}
            if(f.v2 == e.v2) {f.v2 = new_vertex_index;insert_unique(vertices[new_vertex_index].adjacent_faces,face_idx);}
            if(f.v3 == e.v2) {f.v3 = new_vertex_index;insert_unique(vertices[new_vertex_index].adjacent_faces,face_idx);}
        }
    }

    edgesVec.erase(std::remove_if(edgesVec.begin(), edgesVec.end(), 
    [&e](const auto& it) {
        return (it.v1 == e.v1 && it.v2 == e.v2) || (it.v1 == e.v2 && it.v2 == e.v1);
    }), edgesVec.end());

    Eigen::Matrix4f newQEM = QEM[e.v1] + QEM[e.v2];
    QEM.push_back(newQEM);
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
        file << "f " << (++faces[i].v1) << " " << (++faces[i].v2) << " " << (++faces[i].v3) << endl;
    }
    file.close();
}