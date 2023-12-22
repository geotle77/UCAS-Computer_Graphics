#include<GL/glut.h>
#include "mesh.h"
#include "showmodel.h"
#define NEED_SIMPLIY 1
void output_usage()
{
    cout<<"Usage:./main input_name output_name ratio"<<endl;
    cout<<"input_name: the name of input model"<<endl;
    cout<<"output_name: the name of output model"<<endl;
    cout<<"ratio: the ratio of the number of output model to the number of input model"<<endl;
    cout<<"Example: ./main dragon_repair dragon_simplified 0.5"<<endl;
}

int main(int argc, char** argv)
{
    if(argc ==4){
        std:string input_name = argv[1];
        std::string output_name = argv[2];
        double ratio = atof(argv[3]);
        cout<<"the input file is "<<input_name<<endl;
        cout<<"the output file is "<<output_name<<endl;
        if(ratio >1||ratio<0){
            while (true) {
                cout << "The ratio is invalid, please input a number between 0 and 1: ";
                cin >> ratio;
                if (ratio > 0 && ratio < 1) {
                    break;
                }
            }
        }
        cout<<"the ratio is "<<ratio<<endl;
        cout<<"start simplify,it may take some minutes,please waiting patiently!"<<endl;
        string input_path = "./input_model/"+input_name+".obj";
        string output_path = "./output_model/"+output_name+".obj";
        #if NEED_SIMPLIY
        mesh Dragon(input_path);
        Dragon.Simplify(ratio);
        Dragon.Savemodel(output_path);
        mesh Simplify_Dragon(output_path);
        #else
        mesh Simplify_Dragon(output_path);
        #endif 
        faces = Simplify_Dragon.faces;
        vertices = Simplify_Dragon.vertices;
    }
    else{
        output_usage();
    }
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);//设置窗口大小
    glutInitWindowPosition(100, 100);//设置窗口位置
    glutCreateWindow("Simplipy_Window");//创建窗口
    InitScene();
    glutDisplayFunc(DrawScene);//显示回调函数
    glutSpecialFunc(SpecialKeys);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseClick);
    glutTimerFunc(100, timer, 0); // 设置第一次调用定时器的时间
    glutMainLoop();//进入主循环
    return 0;
}

