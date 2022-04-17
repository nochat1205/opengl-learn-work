/*
 double 尺度 坐标系
 */
#include<stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include<math.h>
#include<vector>
#include<stdlib.h>
#define LENGTH 1080.00
#define PI acos(-1)

using namespace std;

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))


// 整数坐标系

GLdouble rtri = 1.f/20;
GLdouble a0 = 0.f,
         a1 = 0.f,
         a2 = 0.f,
         a3 = 0.f;
GLdouble offset = 0.f;

    static GLdouble eye_r = 1.0;
    static GLdouble eye_xz = 0.f;
    static GLdouble eye_y = 0.f;
    static GLdouble up[3] = {0.f, 1.f, 0.f};


void initgl();
void setEnvironment();
void reshape(int, int);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
// void passiveMotion(int, int);
void display();
void light();

int main(int argc, char **argv){
    glutInit(&argc, argv); //初始化
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);           //设置窗口的位置
    glutInitWindowSize(640, 640);               //设置窗口的大小
    glutCreateWindow("butterfly"); //创建窗口并赋予title

    setEnvironment();
    initgl();
    glutReshapeFunc(reshape);
    // glutMouseFunc();
    // glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutMainLoop();
    return 0;
}

void setEnvironment(){
    /*
        https://www.glprogramming.com/red/chapter05.html
     */
    GLfloat nolight_ambient[]   = { 0.5, 0.5, 0.5, 1.0 };   // 无光源环境光
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, nolight_ambient);

    GLfloat light_ambient[]     = { 0.0, 0.0, 0.0, 1.0 };   // 环境光
    GLfloat light_diffuse[]     = { 0.5, 0.5, 0.5, 1.0 };   // 漫反射
    GLfloat light_specular[]    = { 1.0, 1.0, 1.0, 1.0 };   // 镜面反射

    // /* 光照 */
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);    // 环绕光
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);    // 漫反射
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);   // 镜面反射

    /* 点光源 */
    // glLightfv(GL_LIGHT0, GL_POSITION, _light_position);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.f);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 100.0);


    GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};      //a
    GLfloat mat_shininess[] = {50.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);      // 定义材料属性
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    // glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    /* gllightf
     GL_CONSTANT_ATTENUATION  1.0 恒定衰减因子
     GL_LINEAR_ATTENUATION    0.0 线性衰减因子
     GL_QUADRATIC_ATTENUATION 0.0 二次衰减因子
     */

    glEnable(GL_COLOR_MATERIAL);
    // glEnable(GL_CULL_FACE);                                     // 物体内部光线渲染
    // glCullFace(GL_BACK);
    // glCullFace(GL_FRONT);

    glEnable(GL_LIGHTING);                                      // 开灯
    glEnable(GL_LIGHT0);

    glShadeModel (GL_SMOOTH);
}

void initgl(){
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_ALPHA_TEST);
    glClearColor((GLfloat)251/255, (GLfloat)164/255, (GLfloat)20/255, 1.0);

}

void reshape(int w, int h){ 
    GLdouble WDIVH = (GLdouble) w / (GLdouble)h;

    if (h==0) h = 1;
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // 方形投影空间
    if(w < h)  glOrtho(0.0, LENGTH,
                       0.0, LENGTH / WDIVH,
                      LENGTH, -LENGTH); 
    else glOrtho(0.0, LENGTH * WDIVH, 
                 0.0, LENGTH, LENGTH, -LENGTH);
    glTranslated(LENGTH/2, LENGTH/2, 0.0f);
    glRotated(-30.f, 1.0, 0.0f, 0.f);
    return ;
}

void keyboard(unsigned char key, int x, int y){
    if(key == 27) exit(0);
    if(key == 'l') glDisable(GL_LIGHT0);
    if(key == 'L') glEnable(GL_LIGHT0);
    if(key == 'q') a1 += 3.f;
    if(key == 'Q') a1 -= 3.f;
    if(key == 'w') a2 += 3.f;
    if(key == 'W') a2 -= 3.f;
    if(key == 'e') a3 += 3.f;
    if(key == 'E') a3 -= 3.f;
    if(key == 'f') offset += LENGTH/60;
    if(key == 'F') offset -= LENGTH/60;

    const GLdouble near = 1.f/12;
    const double a_angle = PI/32;

    if(key == 'x') eye_xz -= a_angle;
    else if(key == 'z') eye_xz += a_angle;

    if(key == 'n') eye_r -= near;
    else if(key == 'N') eye_r += near;
    eye_r = max(eye_r, a_angle);

    if(key == 'y') eye_y -= a_angle;
    else if(key == 'Y') eye_y += a_angle;
    eye_y = min(eye_y,  0.f);
    eye_y = max(eye_y, -PI/2);

}

void mouse(int button, int state, int x, int y){
    ;
}

void motion(int x, int y){
    ;
    // gluLookAt(x-eye_x, y-old_y, LENGTH);
    // eye_x = x; eye_y = y;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    
    // eye_r = 0.5;
    glScaled(1/eye_r, 1/eye_r, 1/eye_r);
    gluLookAt(sin(eye_xz)*cos(eye_y), sin(eye_y), cos(eye_xz)*cos(eye_y),
            0, 0, 0, 
            up[0], up[1], up[2]);

    // glTranslated(0.0, 0.0, LENGTH / 6);

    glPushMatrix();
    glColor3ub(183u, 141u,  18u);
    glScaled(LENGTH*1.5, 0.f, LENGTH*1.5);
    glutSolidCube(1.0);
    glPopMatrix();

    light();
    // GLfloat _light_direction[3] = {0.0, 0.0, -1.0};
    // glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, _light_direction);
    glPopMatrix();
    glutSwapBuffers();
}

void light() {
    const double basic_base = LENGTH / 60;

    glPushMatrix(); 
    glColor3ub( 17u, 119u, 176u); { // bottom
        double angle  = a0;
        const double base = basic_base;
        const double size[3] = {10.f, 3.f, 10.f};

        glTranslated(offset, 0.f, 0.f);
        glRotated(angle, 0.0f, 0.0f, -1.0f); // 旋转某点

        glTranslated(-base*size[0]/2, 0.f, 0.f);
        glPushMatrix();
        glScaled(size[0], size[1], size[2]);
        glutSolidSphere(base, 120, 120);
        glPopMatrix();
    }

    glColor3ub( 18u, 161u, 130u);
    // first bar
    {
        double angle = a1;
        const double base = basic_base;
        const double size[3] = {8.f, 1.f, 1.f};
        glRotated(90.f+angle, 0.0f, 0.0f, -1.0f); // 旋转某点

        glTranslated(-base*size[0] / 2, 0.f, 0.f);
        glPushMatrix();
        glScaled(size[0], size[1], size[2]);
        glutSolidCube(base);
        glPopMatrix();

        glTranslated(-base*size[0] / 2, 0.f, 0.f);
    }

    glColor3ub( 152u, 175u, 134u);
    // second bar
    {
        double angle = a2;
        const double base = basic_base;
        const double size[3] = {8.f, 1.f, 1.f};

        glRotated(30.f+angle, 0.0f, 0.0f, -1.0f); // 旋转某点

        glTranslated(-base*size[0]/2, 0.f, 0.f);
        glPushMatrix();
        glScaled(size[0], size[1], size[2]);
        glutSolidCube(base);
        glPopMatrix();
        glTranslated(-base*size[0]/2, 0.f, 0.f);

    }

    glColor3ub( 255u, 255u, 255u);
    // light
    {
        double angle = a3;
        double cone_angle = PI * 30 / 180;
        const double base = basic_base*3;
        const GLfloat light_position[]  = {0.f, 0.f, 0.f, 1.f};
        const GLfloat light_direction[] = {0.f, 0.f, -1.f};

        glRotated(150.f+angle, 0.0f, 0.0f, -1.0f); // 旋转某点
        glRotated(90.f, 0.0f, 1.0f, 0.0f); // 旋转某点

        glTranslated(0.0, 0.0, -base);
        glutWireCone(base, base/tan(cone_angle), 120, 120);
        glColor3ub(255u, 0u, 0u);
        glScaled(1.f, 1.f, 3.f);
        glutWireCube(base/2);

        glLightfv(GL_LIGHT0, GL_POSITION, light_position); // 相较于modelview
        glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_direction);
    }

    glPopMatrix();

}
