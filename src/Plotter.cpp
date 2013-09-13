#include "Plotter.h"
#include <gl/glut.h>
#include <process.h>

void DrawBlock()
{
    glLineWidth(1);
    glColor3f(0,0,0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0,0);
    glVertex2f(0,410);
    glVertex2f(760,410);
    glVertex2f(760,0);
    glEnd();
}

void PlotFunc()
{
    Plotter*ptr=Plotter::GetInstance();
    DataPacket pack;

    do
    {
        if(ptr->m_pending.empty())
        {
            Sleep(1);
            break;
        }
        glClear(GL_COLOR_BUFFER_BIT);
        pack=ptr->m_pending.front();

        glColor3f(0,0,1);
        glBegin(GL_LINE_STRIP);
        for(int i=0; i<pack.size; ++i)
            glVertex2f(i*760./pack.size,(pack.data[i]+1)/2*410);
        glEnd();
        //DrawBlock();

        delete[]pack.data;
        ptr->m_pending.pop();
    }
    while(0);
    DrawBlock();
    glutSwapBuffers();
    glutPostRedisplay();
}

void Reshape(int x,int y)
{
    glViewport(0,0,x,y);
}

#define width 400
#define height 300

void PlotThread(void*para)
{
    int argc=1;
    glutInit(&argc, NULL);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize (800, 450);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("Plotter");



    ((Plotter*)para)->SetBGColor(1,1,1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluOrtho2D(-20,780,-20,430);
    glPointSize( 6.0 );

    glutReshapeFunc(Reshape);
    glutDisplayFunc(PlotFunc);
    glutMainLoop();
}

Plotter* Plotter::m_inst=NULL;

Plotter::Plotter()
{
    //ctor
    m_inst=this;
    _beginthread(PlotThread,0,this);
}

void Plotter::Plot(Real*buf,int size)
{
    DataPacket pack;
    pack.data=new Real[size];
    pack.size=size;
    memcpy(pack.data,buf,sizeof(Real)*size);
    m_pending.push(pack);
}

void Plotter::SetBGColor(float R,float G,float B)
{
    glClearColor (R, G, B, 0.0);
}

Plotter::~Plotter()
{
    //dtor
}
