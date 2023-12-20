#include <GL/glut.h>
#include <iostream>
#include <time.h>
#include <queue>
#include <math.h>
#include <vector>
#include <fstream>
#include <sstream>        
#include <string>
#include<unordered_map>
#include <Eigen/Dense>

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/PropertyManager.hh>

using namespace std;    
using namespace Eigen;

typedef OpenMesh::TriMesh_ArrayKernelT <> MyMesh;

struct Vertex{
    double x;
    double y;
    double z;
};
struct Texture {
    double TU;
    double TV;
};
struct Normal {
    double nx;
    double ny;
    double nz;
};
struct Face{
    int vertex[3];
    int texture[3];
    int normal[3];
};

struct item{
  double cost;
  Vertex optimalv3;
  Vector2i validpair;
  bool operator==(const item& other) const {
        return {(this->validpair.x() == other.validpair.x() && this->validpair.y() == other.validpair.y()||this->validpair.x() == other.validpair.y() && this->validpair.y() == other.validpair.x())&&this->cost == other.cost};
    }
};
struct CompareCost {
    bool operator()(item const& a, item const& b) {
        return a.cost < b.cost; // 注意这里是大于符号，因为我们要实现最小堆
    }
};
typedef vector<int> link;
typedef priority_queue<item, std::vector<item>, CompareCost> linkheap;


class mesh
{
    private:
        MyMesh om_mesh_;
        int sizeofvalidvertex;
        int sizeofvalidface;  
        vector<link> linkfaces;
        vector<link> linkpoints;
        vector<link> nearestpoints;
        vector<link> linkcostheaps;
        vector<bool>validvertices;
        vector<bool>validfaces;
        vector<bool>validcostheaps;
        vector<item> costheap;
        vector<Eigen::Matrix4d> Q;        
        void Initlize();
        Eigen::Matrix4d ComputeQ(int p);
        struct item Getcost(int pti,int ptj);
        void MakeHeap();
        void HeapPush(item temp);
        void DeleteVertex();
        Normal Normailize(int i);
        struct item HeapPop();
    public:
        mesh(const string & model_path)
        {
                if (!OpenMesh::IO::read_mesh(om_mesh_, model_path))
            {
                std::cerr << "Error loading model: " << model_path << std::endl;
                // 这里你可以选择抛出一个异常，或者做其他的错误处理
            }
             // 请求顶点法线属性
            om_mesh_.request_vertex_normals();
            om_mesh_.request_face_normals();

            // 更新顶点法线
            om_mesh_.update_normals();
            Initlize();
        };
        vector<Vertex> vertices;
        vector<Face> faces;
        vector<Texture> textures;
        vector<Normal> normals;
        vector<Normal> vertexnormals;
        
        void Simplify(double target);
        void Savemodel(const string & model_path);
};

