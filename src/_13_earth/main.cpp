#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include <tool/stb_image.h>
#define SPACE 1080.00
#define PI acos(-1)

using namespace std;
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

typedef struct
{
    int X;
    int Y;
    int Z;
    double U;
    double V;
} VERTICES;
// 整数坐标系

const int num = 10;
const int VertexCount = (90 / num) * (360 / num) * 4;
VERTICES VERTEX[VertexCount];
const GLdouble sphere = 30.f;

GLdouble rtri = 1.f;
GLdouble a0 = 0.f,
         a1 = 0.f,
         a2 = 0.f,
         a3 = 0.f,
         b1 = 0.f,
         b2 = 0.f;
bool isLight = true;

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

static GLuint tex[10];
void bind_image2D(GLsizei texId, GLuint* texName, const char* fileName);
void create_texture();

void DisplaySphere(double R, GLuint texture);
void CreateSphere(double R, double K=0, double H=0, double Z=0);

int main(int argc, char **argv){
    glutInit(&argc, argv); //初始化
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);           //设置窗口的位置
    glutInitWindowSize(640, 640);               //设置窗口的大小
    glutCreateWindow("butterfly"); //创建窗口并赋予title

    setEnvironment();
    initgl();
    create_texture();

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

    GLfloat light_ambient[]     = { 0.0, 0.0, 0.0, 1.0 };   // 环境光
    GLfloat light_diffuse[]     = { 0.5, 0.5, 0.5, 1.0 };   // 漫反射
    GLfloat light_specular[]    = { 1.0, 1.0, 1.0, 1.0 };   // 镜面反射


    // /* 光照 */
    // glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);    // 环绕光
    // glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);    // 漫反射
    // glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);  // 镜面反射

    /* 点光源 */
    // glLightfv(GL_LIGHT0, GL_POSITION, _light_position);
    // glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.f);
    // glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 96.f);


    GLfloat nolight_ambient[]   = { 0.5, 0.5, 0.5, 1.0 };   // 无光源环境光
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, nolight_ambient);
    // glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};      //a
    GLfloat mat_shininess[] = {50.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);      // 定义材料属性
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    // glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    /* gllightf
    // GL_CONSTANT_ATTENUATION  1.0 恒定衰减因子
    // GL_LINEAR_ATTENUATION    0.0 线性衰减因子
    // GL_QUADRATIC_ATTENUATION 0.0 二次衰减因子
     */


    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_CULL_FACE);                                     // 物体内部光线渲染
    // glCullFace(GL_BACK);
    // glCullFace(GL_FRONT);

    glEnable(GL_LIGHTING);                                      // 开灯
    // glEnable(GL_LIGHT0);

    glShadeModel (GL_SMOOTH);
}

void initgl(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
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
    if(w < h)  glOrtho(0.0, SPACE,
                       0.0, SPACE / WDIVH,
                      SPACE, -SPACE); 
    else glOrtho(0.0, SPACE * WDIVH, 
                 0.0, SPACE, SPACE, -SPACE);

    glTranslated(SPACE/2, SPACE/2, 0.0f);
    // glRotated(-15.f, 1.0, 0.0f, 0.f);
    return ;
}

void keyboard(unsigned char key, int x, int y){
    if(key == 27) exit(0);
    if(key == 'q') a0 += 1.f;
    if(key == 'Q') a0 -= 1.f;
    if(key == 'w') a1 += 1.f;
    if(key == 'W') a1 -= 1.f;
    if(key == 'e') a2 += 1.f;
    if(key == 'E') a2 -= 1.f;
    if(key == 'r') b1 += 1.f;
    if(key == 'R') b1 -= 1.f;
    if(key == 't') b2 += 1.f;
    if(key == 'T') b2 -= 1.f;

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
    // gluLookAt(x-eye_x, y-old_y, SPACE);
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


    glPushMatrix();

    glColor3ub(183u, 141u,  18u);
    glTranslated(0.f, -SPACE/2, 0.f);
    glScaled(SPACE*1.5, 0.f, SPACE*1.5);
    glutSolidCube(1.0);
    glPopMatrix();

    light();

    glPopMatrix();
    glutSwapBuffers();
}

void light() {
    const double base = SPACE / 2;

    glEnable(GL_TEXTURE_2D);
    glPushMatrix(); 
    // glDisable(GL_LIGHT);
    glColor3ub( 17u, 119u, 176u);
    { // sun
        const double sun = base / 4;
        const GLfloat light_position[]  = {0.f, 0.f, 0.f, 1.f};
        GLfloat _light_direction[3] = {1.f, 0.0, 0.0};

        static double angle = 0.f;

        angle += a0;
        if(angle > 360.f) angle -= 360.f;
        else if(angle < 0.f)  angle += 360.f;

        DisplaySphere(sun, tex[0]);

        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, _light_direction);
        // glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 127.f);

        glDisable(GL_TEXTURE_2D);    
        glEnable(GL_LIGHT0);

        glRotated(angle, 0.0f, 1.f, 0.0f); // 旋转某点
        glTranslated(base*2/3, 0.f, 0.f);
    }

    // earth
    glColor3ub( 18u, 161u, 130u);
    {
        const double earth = base / 8;
        static double s_angle = 0.f;
        static double angle = 0.f;

        s_angle += b1;
        angle += a1;
        if(angle > 360.f) angle -= 360.f;
        else if(angle < 0.f)  angle += 360.f;

        glRotated(angle, 0.0f, 1.f, 0.0f); // 旋转某点
        // DisplaySphere(earth, tex[1]);
        glPushMatrix();
        glRotated(s_angle, -1.f, 1.f, 0.f);
        glutWireSphere(earth, 120, 120);
        glTranslated(1.0f, 0.f, 0.f);
        glColor3ub(255u, 0, 0);
        glutWireSphere(earth, 120, 120);
        glPopMatrix();

        glTranslated(base/4, 0.f, 0.f);
    }

    // moon
    glColor3ub( 131u, 72u, 212u);{
        const double moon = base / 16;
        static double s_angle = 0.f;
        
        static double angle = 0.f;
        s_angle += b2;
        angle += a2;
        if(angle > 360.f) angle -= 360.f;
        else if(angle < 0.f)  angle += 360.f;

        glRotated(angle, 0.0f, 1.f, 0.0f); // 旋转某点
        
        glPushMatrix();
        glRotated(s_angle, -1.f, 1.f, 0.f);
        glutWireSphere(moon, 120, 120);
        glTranslated(1.0f, 0.f, 0.f);
        glColor3ub(255u, 0, 0);
        glutWireSphere(moon, 120, 120);
        glPopMatrix();


    }

    // const double angle = PI / 180 * 60;
    // GLfloat lightPos[] = {0.f, 0.f, SPACE / 6, 1.0f};    // 位置
    // GLfloat lightDire[3] = {0.f, 0.f, 1.f};
    //     // GLdouble size = scale / 6;
    // glColor3ub(255u, 0u, 0u);

    // glLightfv(GL_LIGHT0, GL_POSITION, lightPos);                // 位置 最后一个参数为0.0为无穷远处
    // glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDire);
    // glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.f);
    // glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 34.0f);    
    glPopMatrix();
    
    glDisable(GL_TEXTURE_2D);
}

void create_texture() {
    const char* files[] = {"./static/texture/sun.jpg", 
                         "./static/texture/earth.jpg",
                         "./static/texture/moon.png"};
    for(int i=0; i < 3; ++i){
        bind_image2D(i+1, tex+i, files[i]);
    }

    CreateSphere(sphere);
}

void bind_image2D(GLsizei texId, GLuint* texName, const char* fileName) {
   unsigned char *data;
   int width, height, nrChannels;

    glGenTextures(texId, texName);
    glBindTexture(GL_TEXTURE_2D, *texName);
    // 加载图片
   data = stbi_load(fileName, &width, &height, &nrChannels, 0);
   if (data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB-3+nrChannels, 
                   width, height, 0, 
                   GL_RGB-3+nrChannels, GL_UNSIGNED_BYTE, data);
   }

    glGenTextures(texId, texName);
    glBindTexture(GL_TEXTURE_2D, *texName);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
}

void DisplaySphere(double R, GLuint texture) {
    int b;
    glPushMatrix();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glScalef(R/sphere, R/sphere, R/sphere);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_TRIANGLE_STRIP);
    for (b = 0; b < VertexCount; b++) {
        glTexCoord2f(VERTEX[b].U, VERTEX[b].V);
        glVertex3f(VERTEX[b].X, VERTEX[b].Y, -VERTEX[b].Z);
    }

    for (b = 0; b < VertexCount; b++) {
        glTexCoord2f(VERTEX[b].U, -VERTEX[b].V);
        glVertex3f(VERTEX[b].X, VERTEX[b].Y, VERTEX[b].Z);
    }

    glEnd();
    glPopMatrix();
}

void CreateSphere(double R, double K, double H, double Z) {
    int n;
    double a;
    double b;
    n = 0;
    for (b = 0; b <= 90 - num; b += num) {
        for (a = 0; a <= 360 - num; a += num) {
            VERTEX[n].X = R * sin((a) / 180 * PI) * sin((b) / 180 * PI) - H;
            VERTEX[n].Y = R * cos((a) / 180 * PI) * sin((b) / 180 * PI) + K;
            VERTEX[n].Z = R * cos((b) / 180 * PI) - Z;
            VERTEX[n].V = (2 * b) / 360;
            VERTEX[n].U = (a) / 360;
            n++;

            VERTEX[n].X = R * sin((a) / 180 * PI) * sin((b + num) / 180 * PI) - H;
            VERTEX[n].Y = R * cos((a) / 180 * PI) * sin((b + num) / 180 * PI) + K;
            VERTEX[n].Z = R * cos((b + num) / 180 * PI) - Z;
            VERTEX[n].V = (2 * (b + num)) / 360;
            VERTEX[n].U = (a) / 360;
            n++;

            VERTEX[n].X = R * sin((a + num) / 180 * PI) * sin((b) / 180 * PI) - H;
            VERTEX[n].Y = R * cos((a + num) / 180 * PI) * sin((b) / 180 * PI) + K;
            VERTEX[n].Z = R * cos((b) / 180 * PI) - Z;
            VERTEX[n].V = (2 * b) / 360;
            VERTEX[n].U = (a + num) / 360;
            n++;
            VERTEX[n].X = R * sin((a + num) / 180 * PI) * sin((b + num) / 180 * PI) - H;
            VERTEX[n].Y = R * cos((a + num) / 180 * PI) * sin((b + num) / 180 * PI) + K;
            VERTEX[n].Z = R * cos((b + num) / 180 * PI) - Z;
            VERTEX[n].V = (2 * (b + num)) / 360;
            VERTEX[n].U = (a + num) / 360;
            n++;
        }
    }
}
