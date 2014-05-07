 
 
 
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
enum MenuEntry {Create,Delete,Move,Save,Load,Clear};//菜单选项  //  都是int 类型的
enum State {Normal,Creating,Deleting,Moving,Changeing,Clearing};//程序运行状态
GLfloat colors[8][3]={ {0.0,0.0,0.0},{1.0,0.0,0.0},{0.0,1.0,0.0},{0.0,0.0,1.0},{0.0,1.0,1.0},
{1.0,0.0,1.0},{1.0,1.0,0.0},{1.0,1.0,1.0} };//颜色值
struct point{int x,y; point(int tx=0,int ty=0){x=tx;y=ty;}};
struct poly
{
    int offsetx,offsety;
	int color;
    vector<point> p;
	poly(){offsetx=offsety=0;color=0;}
	  //需要初始化！！！


} po[100];
int ponum=0,selected=-1;
State state=Normal;
vector<point>::iterator it;     
vector<point> drawingpoint;   //所有的点 

//显示字符串
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
//这个函数很重要！！！
//点击处理,设置 selected的值
void processHits (GLint hits, GLuint buffer[])
{
   unsigned int i, j;
   GLuint names, *ptr;
    printf ("hits = %d\n", hits);
	/// printf ("buffer[0] = %d\n", buffer[0]);   一直是1

   ptr = (GLuint *) buffer;

   for (i = 0; i < hits; i++)
   {
      names = *ptr;
	   printf ("命中个数= %d\n", names);
      ptr+=3;  ////跳过 buffer前三部分，得到命中对象的标号name
      for (j = 0; j < names; j++)
      {
         selected=*ptr-1;
         printf("selected=%d\n",selected);
         ptr++;
      }
      printf ("\n");
   }
}
//draw函数   显示多边形列表中的素有多边形！！！！
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
   processHits(hits, selectBuf); ///设置selected变量，，

   glutPostRedisplay();// ？？？？？
}
//////////////////////////////////////////////////////////////////////
//draw 函数 显示所有列表中的 多边形
void draw(GLenum mode=GL_RENDER)
{
   
    for(int i=0;i<ponum;i++)
    {
        if(mode==GL_SELECT) 
			glLoadName(i+1);  ///只保留了栈顶的  名字
        
		if(state==Changeing&&selected==i)
			 {  printf ("coloer=%d\n",PolygonColorIndex);
			po[i].color=PolygonColorIndex;  ///更改选中的多边形的颜色
		}
	 
	 
	 glColor3f(colors[po[i].color][0],colors[po[i].color][1],colors[po[i].color][2]); ///设置每个多边形的颜色
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
	
    //用户提示语
    glColor3f( 0.0, 0.0, 1.0 ); 

	drawString("Press RIGHT button to show menu!",20,60);    

	 

    glColor3f( 1.0, 0.0, 0.0 );
    glPointSize(5.0);
    if(state==Creating)
    {///显示所有的顶点！！！
        glBegin(GL_POINTS);
        for(it=drawingpoint.begin();it!=drawingpoint.end();++it)
            glVertex2d((*it).x,(*it).y);
        glEnd();
    }

    draw();  //显示 所有的 多边形

   
	 
 


   glFlush();   //这个函数是 刷新 命令队列，强制显示，
   glutSwapBuffers();  //这个也得必须有！！！！！
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
void processMenuEvents(int option)//处理 菜单响应事件
{
    switch(option)
    {
        case Create:
            state=Creating;
            selected=-1;
            drawingpoint.clear();  //向量的处理
            po[ponum]=poly();   //新加一个 多边形
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
			//删除 po[]中的所有的多边形。。
			ponum=0;

    }
    display();
}
void processColorMenu(int option)
{
	state=Changeing;   ///状态改为 改变颜色
    selected=-1;
	PolygonColorIndex=option;


}
void createMenu()// 
{
    int c_menu; 
	c_menu= glutCreateMenu(processColorMenu);  /// 处理菜单的函数
	glutAddMenuEntry("黑色",0);  ////后边这个代表编号
	glutAddMenuEntry("红色",1);
	glutAddMenuEntry("绿色",2);
	glutAddMenuEntry("蓝色",3);
	glutAddMenuEntry("青色",4);
	glutAddMenuEntry("枚红色",5);
	glutAddMenuEntry("黄色",6);
	glutAddMenuEntry("白色",7);


	glutCreateMenu(processMenuEvents);//  菜单选择事件的响应
    glutAddMenuEntry("绘制多边形",Create); 
    glutAddMenuEntry("删除",Delete); 
	glutAddSubMenu("改变颜色",c_menu); 
    glutAddMenuEntry("移动多边形",Move); 
  
    glutAddMenuEntry("保存场景",Save); 
    glutAddMenuEntry("载入场景",Load); 
	glutAddMenuEntry("清屏",Clear); 
    glutAttachMenu(GLUT_RIGHT_BUTTON); 
}
void keyboard ( unsigned char key, int x, int y )//鎸夐敭澶勭悊绋嬪簭
{
    //printf("key:%d\n",key);
    switch ( key )
    {
        case 27: exit ( 0 ); break; //ESC
        case 13: //ENTER
                 switch(state)
                 {
                     case Creating:
                         state=Normal;   /// 多边形的点，结束
                         if(drawingpoint.size()) ponum++;   ///若画的点的个数不为零，则，多边形的个数加1
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
                         state=Normal;   //移动过程，按空格结束！！！
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

void mouse(int button, int mstate, int x, int y)//榧犳爣鎸夐敭澶勭悊绋嬪簭
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
				///break;   没有这一句，，switch 语句从与选项值相同的case标签处开始执行，直到遇到 break语句！！！！！！！
			
				pick(x,y);
					if(selected!=-1)
                         {  ///说明选中了一个多边形！！！！
                             for(int i=selected;i<ponum-1;i++)
                                 po[i]=po[i+1];
                             ponum--;     ///从列表中删除！！
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
    glutCreateWindow("我的多边形世界");
	myinit();
	/////---------------华丽的分割线--------------------------------------
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
