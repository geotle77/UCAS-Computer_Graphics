#include "mesh.h"

void mesh::Initlize()
{
    for (MyMesh::VertexIter v_it = om_mesh_.vertices_begin(); v_it != om_mesh_.vertices_end(); ++v_it)
    {   
        Vertex v;
        v.x = om_mesh_.point(*v_it)[0];
        v.y = om_mesh_.point(*v_it)[1];
        v.z = om_mesh_.point(*v_it)[2];
        vertices.push_back(v);
        link l;
        for (MyMesh::VertexVertexIter vv_it = om_mesh_.vv_iter(*v_it); vv_it.is_valid(); ++vv_it)
        {
            l.push_back(vv_it->idx());
        }
        linkpoints.push_back(l);
    }
    linkcostheaps.resize(vertices.size());
    for(auto it = om_mesh_.faces_begin(); it != om_mesh_.faces_end(); it++)
    {   
         Normal n;
         Face f;
         int i =0;
        for(auto fv_it = om_mesh_.fv_begin(*it); fv_it != om_mesh_.fv_end(*it); fv_it++)
        {
            Normal vn;
            f.vertex[i] = fv_it->idx();
            f.normal[i] = fv_it->idx();
            i++;
            vn.nx = om_mesh_.normal(*fv_it)[0];
            vn.ny = om_mesh_.normal(*fv_it)[1];
            vn.nz = om_mesh_.normal(*fv_it)[2];
            vertexnormals.push_back(vn);
        } 
        n.nx = om_mesh_.normal(*it)[0];
        n.ny = om_mesh_.normal(*it)[1];
        n.nz = om_mesh_.normal(*it)[2];
        normals.push_back(n);
        faces.push_back(f);
    }
    linkfaces.resize(vertices.size());
    for(auto it = om_mesh_.faces_begin(); it != om_mesh_.faces_end(); it++)
    {
        for(auto fv_it = om_mesh_.fv_begin(*it); fv_it != om_mesh_.fv_end(*it); fv_it++)
        {
            linkfaces[fv_it->idx()].push_back(it->idx());
        }
    }
    Q.resize(vertices.size());
}

Matrix4f mesh::ComputeQ(int p)//计算第p个顶点的Q矩阵
{
    Q[p] = Matrix4f::Zero();
    Matrix4f kp;
    for(int i=0;i<linkfaces[p].size();i++)
    {
        Vector4f plane;
        plane.x() = normals[linkfaces[p][i]].nx;
        plane.y() = normals[linkfaces[p][i]].ny;
        plane.z() = normals[linkfaces[p][i]].nz;
        plane.w() = -vertices[p].x*plane.x()-vertices[p].y*plane.y()-vertices[p].z*plane.z();
        kp=plane*plane.transpose();
        Q[p]+=kp;
    }
    return Q[p];
}
static double Det3x3(double Qsolve[4][10],int idx)
{
    Matrix3d mat;
    for(int i=0;i<3;i++)
    {
        for(int j=0;j<3;j++)
        {
            mat(i,j)=Qsolve[idx][i*3+j];
        }
    }
    double det = 0;
    det = mat(0,0)*mat(1,1)*mat(2,2)+mat(0,1)*mat(1,2)*mat(2,0)+mat(0,2)*mat(1,0)*mat(2,1)
        -mat(0,2)*mat(1,1)*mat(2,0)-mat(0,1)*mat(1,0)*mat(2,2)-mat(0,0)*mat(1,2)*mat(2,1);
    return det;
}
struct item mesh::Getcost(int pti,int ptj)//计算第pti个顶点和第ptj个顶点合并后的代价
{
    Matrix4f Qnew;
    Qnew = Q[pti]+Q[ptj];

    double Qsolve[4][10];
    memset(Qsolve,0,sizeof(Qsolve));
    for(int i=0;i<4;i++)
    {
        for(int j=0;j<3;j++)
        {
            for(int k=0;k<3;k++)
            {
                Qsolve[i][j*3+k]=Qnew(j,k);
            }
        }
    }
    for(int i=1;i<4;i++)
    {
        for(int j=0;j<3;j++)
        {
           Qsolve[i][j*3+i-1]=Qnew(j,3);
        }
    }

    Vector4d optimalv4;
    double temp3X3 = Det3x3(Qsolve,0);
    if(temp3X3!=0)
    {
        optimalv4.x() = Det3x3(Qsolve,1)/temp3X3;
        optimalv4.y() = Det3x3(Qsolve,2)/temp3X3;
        optimalv4.z() = Det3x3(Qsolve,3)/temp3X3;
    }
    else{
        optimalv4.x() = 0;
        optimalv4.y() = 0;
        optimalv4.z() = 0;
    }
    optimalv4.w() = 1;

    double cost ;
    cost = (optimalv4.transpose()*Qnew*optimalv4).coeff(0, 0);
    item best;
    if(!(cost>=2||cost<=-2))
    { 
        cost = 0.0;
    }
    best.cost = fabs(cost);
    best.optimalv3.x = optimalv4.x();
    best.optimalv3.y = optimalv4.y();
    best.optimalv3.z = optimalv4.z();
    best.validpair.x()=pti;
    best.validpair.y()=ptj;
    return best;
}

void mesh::HeapPush(item temp)
{
    linkheap &heap1 = linkcostheaps[temp.validpair.x()];
    linkheap &heap2 = linkcostheaps[temp.validpair.y()];
    heap2.push(temp);
    heap1.push(temp);
}
void mesh::MakeHeap()
{
    cout << "MakeHeap....." << endl;
    linkheap null;
    for(int i=0;i<vertices.size();i++)
    {
        linkcostheaps.push_back(null);
    }
    for(int i=0;i<vertices.size();i++)
    {
        for(int j=0;j<linkpoints[i].size();j++)
        {
            item temp;
            temp = Getcost(i,linkpoints[i][j]);
            HeapPush(temp);
        }
    }
}
Normal mesh::Normailize(int face_idx)
{

}

void mesh::DeleteVertex()
{
    struct item vbest = this->HeapPoP();

    vertices.push_back(vbest.optimalv3);
    validvertices.push_back(true);
    sizeofvalidvertex++;

    int todelete1 = vbest.validpair.x();
    int todelete2 = vbest.validpair.y();
    validvertices[todelete1] = false;
    validvertices[todelete2] = false;
    sizeofvalidvertex-=2;

    for(int i=0;i<linkfaces[todelete1].size();i++)
    {
        for(int j =0;j<linkfaces[todelete2].size();j++)
        {
            if(linkfaces[todelete1][i]==linkfaces[todelete2][j])
            {    
                validfaces[linkfaces[todelete1][i]]=false;
                int c = faces[linkfaces[todelete1][i]].vertex[0]^faces[linkfaces[todelete1][i]].vertex[1]^faces[linkfaces[todelete1][i]].vertex[2];
                for(int k=0;k<linkfaces[c].size();k++)
                {
                    if(linkfaces[c][k]==linkfaces[todelete1][i])
                    {
                        linkfaces[c].erase(linkfaces[c].begin()+k);
                    }
                    sizeofvalidface--;
                }
            }
        }
    }
    link nulllnkface;
    linkfaces.push_back(nulllnkface);
    for(int i=0;i<linkfaces[todelete1].size();i++)
    {
        if(validfaces[linkfaces[todelete1][i]])
        {
            linkfaces.rbegin()->push_back(linkfaces[todelete1][i]);
            if(faces[linkfaces[todelete1][i]].vertex[0]==todelete1)
            {
                faces[linkfaces[todelete1][i]].vertex[0]=vertices.size()-1;
            }
            else if(faces[linkfaces[todelete1][i]].vertex[0]==todelete2)
            {
                faces[linkfaces[todelete1][i]].vertex[0]=vertices.size()-1;
            }
            else if(faces[linkfaces[todelete1][i]].vertex[1]==todelete1)
            {
                faces[linkfaces[todelete1][i]].vertex[1]=vertices.size()-1;
            }
            normals[linkfaces[todelete1][i]] = Normailize(linkfaces[todelete1][i]);
        }
    }
    for(int i=0;i<linkfaces[todelete2].size();i++)
    {
        if(validfaces[linkfaces[todelete2][i]])
        {
            linkfaces.rbegin()->push_back(linkfaces[todelete2][i]);
            if(faces[linkfaces[todelete2][i]].vertex[0]==todelete1)
            {
                faces[linkfaces[todelete2][i]].vertex[0]=vertices.size()-1;
            }
            else if(faces[linkfaces[todelete2][i]].vertex[0]==todelete2)
            {
                faces[linkfaces[todelete2][i]].vertex[0]=vertices.size()-1;
            }
            else if(faces[linkfaces[todelete2][i]].vertex[1]==todelete1)
            {
                faces[linkfaces[todelete2][i]].vertex[1]=vertices.size()-1;
            }
            normals[linkfaces[todelete2][i]] = Normailize(linkfaces[todelete2][i]);
        }
    }
    Q.push_back(Matrix4f::Zero());
    Matrix4f newQ = ComputeQ(vertices.size()-1);
    linkheap nullpair;
    linkcostheaps.push_back(nullpair);
    link ano;
    for(int i=0;i<linkcostheaps[todelete1].size();i++)
    {
        int anopoint =0;
        priority_queue<item, std::vector<item>, CompareCost> &heap = linkcostheaps[todelete1];
        priority_queue<item, std::vector<item>, CompareCost> temp_q;
        vector<item> temp_v;
        while(!heap.empty())
        {
            temp_v.push_back(heap.top());
            temp_q.push(heap.top());
            heap.pop();
        }
        heap = temp_q;
        if(temp_v[i].validpair.x()==todelete1)
        anopoint = temp_v[i].validpair.y();
        else 
        anopoint = temp_v[i].validpair.x();
        if(validvertices[anopoint])
        {
            ano.push_back(anopoint);
        }
    }
    for(int i=0;i<linkcostheaps[todelete2].size();i++)
    {
        int anopoint =0;
        priority_queue<item, std::vector<item>, CompareCost> &heap = linkcostheaps[todelete2];
        priority_queue<item, std::vector<item>, CompareCost> temp_q;
        vector<item> temp_v;
        while(!heap.empty())
        {
            temp_v.push_back(heap.top());
            temp_q.push(heap.top());
            heap.pop();
        }
        heap = temp_q;
        if(temp_v[i].validpair.x()==todelete2)
        anopoint = temp_v[i].validpair.y();
        else 
        anopoint = temp_v[i].validpair.x();
        if(validvertices[anopoint])
        {
            ano.push_back(anopoint);
        }
    }
    sort(ano.begin(),ano.end());
    int p=1;
    for(int i=0;i<ano.size();i++)
    {
        if(ano[i]!=ano[i-1])
        {
            ano[p]=ano[i];
            p++;
        }
    }
    ano.erase(ano.begin()+p,ano.end());
    nearestpoints.push_back(ano);
    int tempsize = linkcostheaps[todelete1].size();
    for(int i=0;i<tempsize;i++)
    {
        linkcostheaps[todelete1].pop();
    }
    tempsize = linkcostheaps[todelete2].size();
    for(int i=0;i<tempsize;i++)
    {
        linkcostheaps[todelete2].pop();
    }
    for(int i=0;i<ano.size();i++)
    {
        item temp;
        temp = Getcost(vertices.size()-1,ano[i]);
        HeapPush(temp);
    }
}

