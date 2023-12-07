#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;
struct vertex
{
    float x;
    float y;
    float z;
};

struct face
{
    int v1;
    int v2;
    int v3;
};
class Mesh
{
public:
    vector<vertex> vertices;
    vector<face> faces;
    int face_num;
    int vertex_num;
    int edge_num;
    void loadmodel(string filename);
    void export2Obj(vector<vertex> vertices, vector<face> faces);
    void simplify(int target);
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
            faces.push_back(f);
        }
    }
    
    file.close();
    face_num = faces.size();
    vertex_num = vertices.size();

    printf("the face number is %d\n", face_num);
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