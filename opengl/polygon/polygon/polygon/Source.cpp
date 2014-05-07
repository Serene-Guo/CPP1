 
 
 
//#include <stdlib.h>



#include <windows.h>
#include <gl/glut.h>
#include <stdio.h>
#include <vector>



using namespace std;
#define MAX_CHAR 128 
int selmode=0;
int width=500,height=500;
int PolygonColorIndex=0;
enum MenuEntry {Create,Delete,Move,Save,Load,Clear};//�˵�ѡ��  //  ����int ���͵�
enum State {Normal,Creating,Deleting,Moving,Changeing,Clearing};//��������״̬
GLfloat colors[8][3]={ {0.0,0.0,0.0},{1.0,0.0,0.0},{0.0,1.0,0.0},{0.0,0.0,1.0},{0.0,1.0,1.0},
{1.0,0.0,1.0},{1.0,1.0,0.0},{1.0,1.0,1.0} };//��ɫֵ
struct point{int x,y; point(int tx=0,int ty=0){x=tx;y=ty;}};
struct poly
{
    int offsetx,offsety;
	int color;
    vector<point> p;
	poly(){offsetx=offsety=0;color=0;}
	  //��Ҫ��ʼ��������


} po[100];
int ponum=0,selected=-1;
State state=Normal;
vector<point>::iterator it;     
vector<point> drawingpoint;   //���еĵ� 

//��ʾ�ַ���
void drawString(const char* str,int x,int y) 
{
    glRasterPos2d(x,y);
    static int isFirstCall = 1; 
    static GLuint lists; 
    if( isFirstCall )
    {
        isFirstCall = 0; 
        lists = glGenLists(MAX_CHAR); 
        wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists); 
    } 
    for(; *str!='\0'; ++str) 
        glCallList(lists + *str); 
} 
//�����������Ҫ������
//�������,���� selected��ֵ
void processHits (GLint hits, GLuint buffer[])
{
   unsigned int i, j;
   GLuint names, *ptr;
    printf ("hits = %d\n", hits);
	/// printf ("buffer[0] = %d\n", buffer[0]);   һֱ��1

   ptr = (GLuint *) buffer;

   for (i = 0; i < hits; i++)
   {
      names = *ptr;
	   printf ("���и���= %d\n", names);
      ptr+=3;  ////���� bufferǰ�����֣��õ����ж���ı��name
      for (j = 0; j < names; j++)
      {
         selected=*ptr-1;
         printf("selected=%d\n",selected);
         ptr++;
      }
      printf ("\n");
   }
}
//draw����   ��ʾ������б��е����ж���Σ�������
void draw(GLenum mode);




void pick(int x, int y)
{// 
   GLuint selectBuf[512];
   GLint hits;
   GLint viewport[4];

   glGetIntegerv(GL_VIEWPORT, viewport);

   glSelectBuffer(512, selectBuf);
   glRenderMode(GL_SELECT);
   glInitNames();
   glPushName(0);

   glMatrixMode (GL_PROJECTION);
   glPushMatrix ();
   glLoadIdentity ();
 
   gluPickMatrix ( x, viewport[3]-y,  5.0, 5.0, viewport);
   gluOrtho2D (0.0, width, height, 0.0);
   draw(GL_SELECT); /// 

   glMatrixMode (GL_PROJECTION);
   glPopMatrix ();
   glFlush ();

   hits = glRenderMode(GL_RENDER);
   processHits(hits, selectBuf); ///����selected��������

   glutPostRedisplay();// ����������
}
//////////////////////////////////////////////////////////////////////
//draw ���� ��ʾ�����б��е� �����
void draw(GLenum mode=GL_RENDER)
{
   
    for(int i=0;i<ponum;i++)
    {
        if(mode==GL_SELECT) 
			glLoadName(i+1);  ///ֻ������ջ����  ����
        
		if(state==Changeing&&selected==i)
			 {  printf ("coloer=%d\n",PolygonColorIndex);
			po[i].color=PolygonColorIndex;  ///����ѡ�еĶ���ε���ɫ
		}
	 
	 
	 glColor3f(colors[po[i].color][0],colors[po[i].color][1],colors[po[i].color][2]); ///����ÿ������ε���ɫ
        glBegin(GL_POLYGON);
        //glBegin(GL_POLYGON);
        for(it=po[i].p.begin();it!=po[i].p.end();++it)
        {
            glVertex2d((*it).x+po[i].offsetx,(*it).y+po[i].offsety);
        }
        glEnd();
 
    }
}

void display()
{
    glClear( GL_COLOR_BUFFER_BIT );
	
    //�û���ʾ��
    glColor3f( 0.0, 0.0, 1.0 ); 

	drawString("Press RIGHT button to show menu!",20,60);    

	 

    glColor3f( 1.0, 0.0, 0.0 );
    glPointSize(5.0);
    if(state==Creating)
    {///��ʾ���еĶ��㣡����
        glBegin(GL_POINTS);
        for(it=drawingpoint.begin();it!=drawingpoint.end();++it)
            glVertex2d((*it).x,(*it).y);
        glEnd();
    }

    draw();  //��ʾ ���е� �����

   
	 
 


   glFlush();   //��������� ˢ�� ������У�ǿ����ʾ��
   glutSwapBuffers();  //���Ҳ�ñ����У���������
}



void idle()
{
    display();
}
void save()
{
    FILE *f=fopen("po.txt","w");
    fprintf(f,"%d\n",ponum);
    for(int i=0;i<ponum;i++)
    {
        fprintf(f,"\n");
 fprintf(f,"%d\n",po[i].color);
        fprintf(f,"%d %d\n",po[i].offsetx,po[i].offsety);
        for(it=po[i].p.begin();it!=po[i].p.end();++it)
        {
            fprintf(f,"%d %d\n",(*it).x,(*it).y);
        }
        fprintf(f,"-1 -1\n");
    }
    fflush(f);fclose(f);
}
void load()
{
    FILE *f=fopen("po.txt","r");
    fscanf(f,"%d\n",&ponum);
    for(int i=0,x,y;i<ponum;i++)
   {fscanf(f,"%d",&po[i].color);
        fscanf(f,"%d%d",&po[i].offsetx,&po[i].offsety);
        po[i].p.clear();
        while(1)
        {
            fscanf(f,"%d%d",&x,&y);
            if(x<0) break;
            po[i].p.push_back(point(x,y));
        }
    }
    display();
    fclose(f);
}
void processMenuEvents(int option)//���� �˵���Ӧ�¼�
{
    switch(option)
    {
        case Create:
            state=Creating;
            selected=-1;
            drawingpoint.clear();  //�����Ĵ���
            po[ponum]=poly();   //�¼�һ�� �����
            break;
        case Delete:
            state=Deleting;
            selected=-1;
            break;
        case Move:
            state=Moving;
            selected=-1;
            break;
        case Save:
			 state=Normal;
            save();
            break;
        case Load:
			state=Normal;
            load();
            break;
		case Clear:
			state=Clearing;
			//ɾ�� po[]�е����еĶ���Ρ���
			ponum=0;

    }
    display();
}
void processColorMenu(int option)
{
	state=Changeing;   ///״̬��Ϊ �ı���ɫ
    selected=-1;
	PolygonColorIndex=option;


}
void createMenu()// 
{
    int c_menu; 
	c_menu= glutCreateMenu(processColorMenu);  /// ����˵��ĺ���
	glutAddMenuEntry("��ɫ",0);  ////������������
	glutAddMenuEntry("��ɫ",1);
	glutAddMenuEntry("��ɫ",2);
	glutAddMenuEntry("��ɫ",3);
	glutAddMenuEntry("��ɫ",4);
	glutAddMenuEntry("ö��ɫ",5);
	glutAddMenuEntry("��ɫ",6);
	glutAddMenuEntry("��ɫ",7);


	glutCreateMenu(processMenuEvents);//  �˵�ѡ���¼�����Ӧ
    glutAddMenuEntry("���ƶ����",Create); 
    glutAddMenuEntry("ɾ��",Delete); 
	glutAddSubMenu("�ı���ɫ",c_menu); 
    glutAddMenuEntry("�ƶ������",Move); 
  
    glutAddMenuEntry("���泡��",Save); 
    glutAddMenuEntry("���볡��",Load); 
	glutAddMenuEntry("����",Clear); 
    glutAttachMenu(GLUT_RIGHT_BUTTON); 
}
void keyboard ( unsigned char key, int x, int y )//按键处理程序
{
    //printf("key:%d\n",key);
    switch ( key )
    {
        case 27: exit ( 0 ); break; //ESC
        case 13: //ENTER
                 switch(state)
                 {
                     case Creating:
                         state=Normal;   /// ����εĵ㣬����
                         if(drawingpoint.size()) ponum++;   ///�����ĵ�ĸ�����Ϊ�㣬�򣬶���εĸ�����1
                         break;
                     case Deleting:
                         
                         state=Normal;
                         selected=-1;
                         break;
					 case Changeing:
						 state=Normal;
                         selected=-1;
		                 break;
                    
                     case Moving:
                         state=Normal;   //�ƶ����̣����ո����������
                         selected=-1;
                         break;
                     case Normal:break;
                 }
        default:
                 break;
    }
    display();
}

int savex,savey;

void mouse(int button, int mstate, int x, int y)//鼠标按键处理程序
{
    savex=x,savey=y;
    if(mstate==GLUT_DOWN && button==GLUT_LEFT_BUTTON)
    {
        switch(state)
        {
            case Creating:
                po[ponum].p.push_back(point(x,y));
                drawingpoint.push_back(point(x,y));
				////glbegin
                break;
            case Deleting:
				///break;   û����һ�䣬��switch ������ѡ��ֵ��ͬ��case��ǩ����ʼִ�У�ֱ������ break��䣡������������
			
				pick(x,y);
					if(selected!=-1)
                         {  ///˵��ѡ����һ������Σ�������
                             for(int i=selected;i<ponum-1;i++)
                                 po[i]=po[i+1];
                             ponum--;     ///���б���ɾ������
                         }
				break;
         //   case Movebefore:
		    case Moving: 
			case Changeing:
                pick(x,y);
                break;
           
            case Normal:break;
        }
    }
    display();
}
void onMouseMove(int x,int y) 
{
    if(state==Moving && selected!=-1)
    {
        po[selected].offsetx+=x-savex;
        po[selected].offsety+=y-savey;
        savex=x;savey=y;
    }
    display();
}
void reshape( int w, int h )
{
    width=w;height=h;
    glViewport( 0, 0, (GLsizei)w, (GLsizei)h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D (0.0, width, height, 0.0);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    display();  
}

void myinit()
{
 
glClearColor(1.0,1.0,1.0,1.0);
 
}
int main( int argc, char ** argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
    glutInitWindowPosition( 200, 200 );
    glutInitWindowSize( width, height );
    glutCreateWindow("�ҵĶ��������");
	myinit();
	/////---------------�����ķָ���--------------------------------------
    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    //glutIdleFunc( display );
    glutMouseFunc( mouse);
    glutKeyboardFunc( keyboard );
    glutMotionFunc( onMouseMove );
    createMenu(); 
    glutMainLoop();
    return 0;
}
