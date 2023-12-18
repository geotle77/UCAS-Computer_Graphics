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
        validvertices.push_back(true);
        link l;
        for (MyMesh::VertexVertexIter vv_it = om_mesh_.vv_iter(*v_it); vv_it.is_valid(); ++vv_it)
        {
            l.push_back(vv_it->idx());
        }
        linkpoints.push_back(l);
    }
    sizeofvalidvertex = vertices.size();

    linkfaces.resize(vertices.size());
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
        validfaces.push_back(true);
    }
    sizeofvalidface = faces.size();
    for(auto it = om_mesh_.faces_begin(); it != om_mesh_.faces_end(); it++)
    {
        for(auto fv_it = om_mesh_.fv_begin(*it); fv_it != om_mesh_.fv_end(*it); fv_it++)
        {
            linkfaces[fv_it->idx()].push_back(it->idx());
        }
    }
    Q.resize(vertices.size());
    for(int i=0;i<vertices.size();i++)
    {
        Q[i] = ComputeQ(i);
    }
    MakeHeap();
}

Matrix4d mesh::ComputeQ(int p)//计算第p个顶点的Q矩阵
{
    Q[p] = Matrix4d::Zero();
    Matrix4d kp;
    for(int i=0;i<linkfaces[p].size();i++)
    {
        Vector4d plane;
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
    Matrix4d Qnew;
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

    Vector4d optimalv4=Vector4d::Zero();
    double temp3X3 = Det3x3(Qsolve,0);
    if(temp3X3!=0)
    {
        optimalv4.x() = -Det3x3(Qsolve,1)/temp3X3;
        optimalv4.y() = -Det3x3(Qsolve,2)/temp3X3;
        optimalv4.z() = -Det3x3(Qsolve,3)/temp3X3;
    }
    else{
        optimalv4.x() = 0.0;
        optimalv4.y() = 0.0;
        optimalv4.z() = 0.0;
    }
    optimalv4.w() = 1.0;

    double cost ;
    Eigen::Matrix<double, 1, 1> costMatrix = optimalv4.transpose() * Qnew * optimalv4;
    cost = costMatrix(0,0);
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
//TODO:waiting for debug
void mesh::HeapPush(item temp)
{
    bool repeat = false;
    auto insertPos = std::upper_bound(costheap.begin(), costheap.end(), temp, CompareCost());
    
    for(auto &it:costheap){
        if(it==temp){
            repeat = true;
            break;
        }
    }
    if(!repeat){
        int idx = insertPos - costheap.begin();
        costheap.insert(insertPos, temp);
        validcostheaps.insert(validcostheaps.begin()+idx,true);
        linkcostheaps[temp.validpair.x()].push_back(idx);
        linkcostheaps[temp.validpair.y()].push_back(idx);
        for(auto &it:linkcostheaps){
            for(auto &it2:it){
                if(it2>idx)
                    it2++;
            }
        }
    }
}
void mesh::MakeHeap()
{
    cout << "MakeHeap....." << endl;
    link null;
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
            validcostheaps.push_back(true);
        }
    }
}
Normal mesh::Normailize(int face_idx)
{
    Face &f = faces[face_idx];
    Normal n;
    Vector3f v1,v2;
    v1.x() = vertices[f.vertex[1]].x-vertices[f.vertex[0]].x;
    v1.y() = vertices[f.vertex[1]].y-vertices[f.vertex[0]].y;
    v1.z() = vertices[f.vertex[1]].z-vertices[f.vertex[0]].z;
    v2.x() = vertices[f.vertex[2]].x-vertices[f.vertex[0]].x;
    v2.y() = vertices[f.vertex[2]].y-vertices[f.vertex[0]].y;
    v2.z() = vertices[f.vertex[2]].z-vertices[f.vertex[0]].z;
    n.nx = v1.y()*v2.z()-v1.z()*v2.y();
    n.ny = v1.z()*v2.x()-v1.x()*v2.z();
    n.nz = v1.x()*v2.y()-v1.y()*v2.x();
    return n;
}
struct item mesh::HeapPop()
{
    
    int idx = 0;
    while(!validcostheaps[idx]&&!(validvertices[costheap[idx].validpair.x()]&&validvertices[costheap[idx].validpair.y()]&&(costheap[idx].validpair.y()!=costheap[idx].validpair.x())))
    {
        if((costheap[idx].validpair.y()==costheap[idx].validpair.x())||!validvertices[costheap[idx].validpair.x()]&&!validvertices[costheap[idx].validpair.y()])
            validcostheaps[idx]=false;
        idx++;
    }
    item best = costheap[idx];
    validcostheaps[idx]=false;
    return best;
}

void mesh::DeleteVertex()
{
    struct item vbest = this->HeapPop();

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
            if(linkfaces[todelete1][i]==linkfaces[todelete2][j])//两个顶点共享一条边,删除共享的面
            {    
                validfaces[linkfaces[todelete1][i]]=false;
                int c = faces[linkfaces[todelete1][i]].vertex[0]^faces[linkfaces[todelete1][i]].vertex[1]^faces[linkfaces[todelete1][i]].vertex[2]^todelete1^todelete2;//c为共享面的另一个顶点
                for(int k=0;k<linkfaces[c].size();k++)
                {   
                    if(c>linkfaces.size())
                        return;
                    if(linkfaces[c][k]==linkfaces[todelete1][i])
                    {
                        linkfaces[c].erase(linkfaces[c].begin()+k);//
                    }
                    
                }
                sizeofvalidface--;
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
            else if(faces[linkfaces[todelete1][i]].vertex[1]==todelete1)
            {
                faces[linkfaces[todelete1][i]].vertex[1]=vertices.size()-1;
            }
            else if(faces[linkfaces[todelete1][i]].vertex[2]==todelete1)
            {
                faces[linkfaces[todelete1][i]].vertex[2]=vertices.size()-1;
            }
            normals[linkfaces[todelete1][i]] = Normailize(linkfaces[todelete1][i]);
        }
    }
    for(int i=0;i<linkfaces[todelete2].size();i++)
    {
        if(validfaces[linkfaces[todelete2][i]])
        {
            linkfaces.rbegin()->push_back(linkfaces[todelete2][i]);
            if(faces[linkfaces[todelete2][i]].vertex[0]==todelete2)
            {
                faces[linkfaces[todelete2][i]].vertex[0]=vertices.size()-1;
            }
            else if(faces[linkfaces[todelete2][i]].vertex[1]==todelete2)
            {
                faces[linkfaces[todelete2][i]].vertex[1]=vertices.size()-1;
            }
            else if(faces[linkfaces[todelete2][i]].vertex[2]==todelete2)
            {
                faces[linkfaces[todelete2][i]].vertex[2]=vertices.size()-1;
            }
            normals[linkfaces[todelete2][i]] = Normailize(linkfaces[todelete2][i]);
        }
    }
    Q.push_back(Matrix4d::Zero());
    Matrix4d newQ = ComputeQ(vertices.size()-1);
    
    link nullpair;
    linkcostheaps.push_back(nullpair);
    link ano;
    for(int i=0;i<linkcostheaps[todelete1].size();i++)
    {
        int anopoint =0;
        if(costheap[linkcostheaps[todelete1][i]].validpair.x()==todelete1)
        {
            anopoint = costheap[linkcostheaps[todelete1][i]].validpair.y();
        }
        else 
            anopoint = costheap[linkcostheaps[todelete1][i]].validpair.x();
        if(validvertices[anopoint])
        {
           ano.push_back(anopoint);
        }
    }
    for(int i=0;i<linkcostheaps[todelete2].size();i++)
    {
        int anopoint =0;
        if(costheap[linkcostheaps[todelete2][i]].validpair.x()==todelete2)
        {
            anopoint = costheap[linkcostheaps[todelete2][i]].validpair.y();
        }
        else 
            anopoint = costheap[linkcostheaps[todelete2][i]].validpair.x();
        if(validvertices[anopoint])
        {
           ano.push_back(anopoint);
        }
    }
    sort(ano.begin(),ano.end());
    int p=1;
    for(int i=1;i<ano.size();i++)
    {
        if(ano[i]!=ano[i-1])
        {
            ano[p]=ano[i];
            p++;
        }
    }
    if(p<ano.size())
        ano.erase(ano.begin()+p,ano.end());
    nearestpoints.push_back(ano);
    for(auto it:linkcostheaps[todelete1])
    {
        validcostheaps[it]=false;
    }
    for(auto it:linkcostheaps[todelete2])
    {
        validcostheaps[it]=false;
    }
    linkcostheaps[todelete1].clear();
    linkcostheaps[todelete2].clear();
    link null;
    linkpoints.push_back(null);
    for(int i=0;i<ano.size();i++)
    {
        
        linkpoints.rbegin()->push_back(ano[i]);
        item temp;
        if(validvertices[ano[i]])
        {
            temp = Getcost(vertices.size()-1,ano[i]);
            HeapPush(temp);
        }
    }
}

void mesh::Simplify(int target)
{
   int opt_faces = sizeofvalidface*target/(faces.size());
   cout << "Delete faces ... " << opt_faces << endl;
   clock_t start, finish;
    start = time(NULL);
    while(sizeofvalidface>opt_faces)
    {
         DeleteVertex();
    }
    finish = time(NULL);
    cout << "Delete faces done!" << endl;
    cout << "Time cost: " << (double)(finish - start) << "s" << endl;
}

void mesh::Savemodel(const string & model_path)
{
    cout << "Save model ... " << endl;

    
    ofstream file(model_path);
    if (!file.is_open()) {
        cout << "Failed to open the file." << endl;
        return;
    }
    // Write vertices
    file<<"# "<<vertices.size()<<" vertices"<<endl;
    file<<"# "<<sizeofvalidface<<" faces"<<endl;
    for (const auto& vertex : vertices) {
        file << "v " << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
    }
    // Write faces
    for (const auto& face : faces) {
        if (!validfaces[&face - &faces[0]]) continue;
        if (!validvertices[face.vertex[0]] || !validvertices[face.vertex[1]] || !validvertices[face.vertex[2]]) continue;
        file << "f " << (face.vertex[0] + 1) << " " << (face.vertex[1] + 1) << " " << (face.vertex[2] + 1) << "\n";
    }
    file.close();
    cout << "Model saved." << endl;
}