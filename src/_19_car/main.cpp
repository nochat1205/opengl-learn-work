#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <tool/stb_image.h>
#define SPACE 1080
#define T 2052
#define PI acos(-1)
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

using namespace std;

class Car;
class trafficLight;

int sysTime;
unsigned char *checkImage;
GLdouble R = SPACE / 12;
GLdouble pos[][2] = {
    -R, -R,
     R, -R,
     R,  R,
    -R,  R,
};

static GLdouble eye_r = 1.0;
static GLdouble eye_xz = 0.f;
static GLdouble eye_y = 0.f;
static GLdouble up[3] = {0.f, 1.f, 0.f};


void init(void);
void reshape(int w, int h);
void keyboard (unsigned char key, int x, int y);
void display(void);

void draw_scene();
void display_car();
void init_machine();
void time_flow();
bool out(Car &car);

void light();

static GLuint tex[10];
void bind_image2D(GLsizei texId, GLuint* texName, const char* fileName);
void create_texture();

class Car{
private:
    #define VERTICAL 0
    #define RIGHT    1
    #define LEFT     2

    static GLdouble quick;
    static GLdouble da;
    GLdouble rr;
    void step();
    GLdouble front(Car*);
    void $setA(GLdouble edge);

public:
    static GLdouble size;
    /* state */
    GLfloat  translate[3] = {0.0, 0.0f, 0.0f};    // 移动参数
    GLfloat  base_angle = 0;                      // 基础方向
    GLfloat  r_angle = 0.0;                       // 方向转角
    GLfloat  rotate[3] = {0.0f, 1.0f, 0.0f};      // 旋转轴
    GLint    target = 0;                          // 0 vertical 1 right, 2 left 
    GLdouble v = 10.f;                            // 速度
    GLdouble a =  0.f;                            // state
    GLdouble center[3] = {R, 0.0, R};             // x, z

    Car(GLuint );
    Car(GLuint , GLuint );
    // 路径 车灯 斑马线
    void flow(trafficLight &, GLdouble zebra, Car* =nullptr);
    void display();
    void show() {printf("pos<%f, %f, %f>\n", translate[0], translate[1], translate[2]);};
};

class trafficLight{
private:
    #define YELLOW  1
    #define RED     2
    #define GREEN   0
    static GLubyte colors[3][3];

public:
    int pos=0;
    int directness = 0;
    int left = 0;

    trafficLight() = default;
    trafficLight(int pos): pos(pos) {};
    void flow();
    void display();
};

vector<Car> cars_pt[12];
trafficLight trafficLights[4];

int main(int argc, char** argv) {
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
   glutInitWindowSize(640, 640);
   glutInitWindowPosition(100, 100);
   glutCreateWindow(argv[0]);

   init();

   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutIdleFunc(display);
   glutMainLoop();
   return 0; 
}

void init(void) {
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);

    srand(time(NULL));
    create_texture();
    light();
    init_machine();
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);                                     // 启用纹理贴图
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);           // 防止纹理走样

    glPushMatrix();
    // eye_r = 0.5;
    glScaled(1/eye_r, 1/eye_r, 1/eye_r);
    gluLookAt(sin(eye_xz)*cos(eye_y), sin(eye_y), -cos(eye_xz)*cos(eye_y),
            0, 0, 0, 
            up[0], up[1], up[2]);

    draw_scene();
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();


    glutWireCube(SPACE/9); // SPACE/9/2 *1.5
    glPopMatrix();
    time_flow();
    for(int i=0; i < 4; ++i) trafficLights[i].display();

    for(int i=0; i < 12; ++i)
        for(int j=0; j < cars_pt[i].size(); ++j)
            cars_pt[i][j].display();

    glPopMatrix();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    GLdouble WDIVH = (GLdouble) w / (GLdouble)h;

    if (h==0) h = 1;
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 方形投影空间
    if(w < h)  glOrtho(0.0, SPACE,
                       0.0, SPACE / WDIVH,
                      SPACE/2, -SPACE/2); 
    else glOrtho(0.0, SPACE * WDIVH,
                 0.0, SPACE, SPACE/2, -SPACE/2);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glTranslated(SPACE/2, SPACE/2, 0.0f);
    glRotated(-30.f, 1.0, 0.0f, 0.f);

    return ;
}

void keyboard (unsigned char key, int x, int y) {
    if(key==27) exit(0);
    else if(key == 't') sysTime = (sysTime+T/20) % T;



    const GLdouble near = 1.f/12;
    const double a_angle = PI/32;

    if(key == 'x') eye_xz -= a_angle;
    else if(key == 'z') eye_xz += a_angle;

    if(key == 'n') eye_r -= near;
    else if(key == 'N') eye_r += near;
    eye_r = max(eye_r, a_angle);


    if(key == 'y') eye_y -= a_angle;
    else if(key == 'Y') eye_y += a_angle;
    eye_y = max(eye_y,  0.f);
    eye_y = min(eye_y, PI/2);

    // glRotated(, 1.0f)

    // printf("lookAt{eye:(%f, %f, %f)}")
}

void create_texture() {

    // 创建
    bind_image2D(1, tex, "./static/texture/cross.jpg");

    // 设置
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // 设置环绕和过滤方式
    // float borderColor[] = {0.3f, 0.1f, 0.7f, 1.0f};
    // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // 设置绘制模式, 使用纹理贴图中的颜色绘制带纹理的多边形
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);


    // 创建
    bind_image2D(2, tex+1, "./static/texture/garden.jpg");

    // 设置
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // 设置环绕和过滤方式
    // float borderColor[] = {0.3f, 0.1f, 0.7f, 1.0f};
    // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 设置绘制模式, 使用纹理贴图中的颜色绘制带纹理的多边形
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

}

void bind_image2D(GLsizei texId, GLuint* texName, const char* fileName) {
   unsigned char *checkImage;
   int width, height, nrChannels;

    glGenTextures(texId, texName);
    glBindTexture(GL_TEXTURE_2D, *texName);
    // 加载图片
   checkImage = stbi_load(fileName, &width, &height, &nrChannels, 0);
   if (checkImage) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB-3+nrChannels, 
                   width, height, 0, 
                   GL_RGB-3+nrChannels, GL_UNSIGNED_BYTE, checkImage);
   }
    stbi_image_free(checkImage);
}

void draw_scene(){
    double size = SPACE /2 ;

    glBindTexture(GL_TEXTURE_2D, tex[0]);                       // 绑定纹理
    
    glBegin(GL_QUADS);
    glTexCoord2f(-1.0, -1.0);   glVertex3d(-size, 0.0, -size); 
    glTexCoord2f(-1.0,  2.0);   glVertex3d(-size, 0.0,  size);
    glTexCoord2f( 2.0,  2.0);   glVertex3d( size, 0.0,  size);
    glTexCoord2f( 2.0, -1.0);   glVertex3d( size, 0.0, -size);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, tex[1]);                       // 绑定纹理
    glBegin(GL_QUADS);
    for(int i=0; i < 4; ++i) {
        int x = 1 - 2*(pos[i][0] < 0), y = 1 - 2* (pos[i][1] < 0);
        glTexCoord2f(-1.0, -1.0);   glVertex3d(pos[i][0], 1.0, pos[i][1]); 
        glTexCoord2f(-1.0,  2.0);   glVertex3d(pos[i][0], 1.0, y * SPACE/2);
        glTexCoord2f( 2.0,  2.0);   glVertex3d(x * SPACE/2, 1.0, y * SPACE/2);
        glTexCoord2f( 2.0, -1.0);   glVertex3d(x * SPACE/2, 1.0, pos[i][1]);
    }
        
    glEnd();

}

GLdouble Car::size  = SPACE/9/2/3 /2;
GLdouble Car::quick = Car::size  / 3;
GLdouble Car::da    = Car::quick / 10;

Car::Car(GLuint direction){
    base_angle = direction*90.0f;
    for(int i=0; i < 2; ++i) {
        int xyz[2] = {0, 2};
        translate[xyz[i]] = (SPACE / 2)
                          * ((direction+i) % 2) 
                          * (1 - 2*(direction/2));
    }
    translate[2] -= SPACE / 120;
    translate[0] -= center[2];
}

Car::Car(GLuint direction, GLuint target){
    GLdouble offset = SPACE/9/ 2 / 3; 
    GLdouble road[3];
    road[LEFT    ] = offset /2;
    road[VERTICAL] = offset *3 / 2;
    road[RIGHT   ] = offset *5 / 2;

    this->target = target;
    base_angle = direction*90.f;

    if(target == RIGHT)     center[2] =  R;
    else if(target == LEFT) center[2] = -R;    

    translate[0] = SPACE / 2;
    translate[2] = road[target];
    rr = translate[2] - center[2];

}

void Car::display(){
    GLdouble scale[] = {1.0, 0.5, 0.5};
    GLdouble wheel = 0.2;
    GLdouble top = 0.3;

    glPushMatrix();
    glRotated(base_angle, rotate[0], rotate[1], rotate[2]);
    glTranslated(translate[0], translate[1], translate[2]);

    glRotated(r_angle, rotate[0], rotate[1], rotate[2]);
    
    glScaled(scale[0], scale[1], scale[2]);
    glTranslated(0, size*(0.5+wheel), 0.f);
    glColor3ub(190u, 201u, 54u);
    glutSolidCube(size);

    glTranslated(0.f, size*0.5, 0.f); // 位移到底端
    glScaled(1/scale[0], 1/scale[1], 1/scale[2]);

    /* 轮子 */
    glColor3ub(252u, 195u, 7u);
    for(int i=0; i < 4; ++i){
        int x = 1 - (i/2) *2;
        int z = 1 - (i%2) *2;
        int r_b = 5;


        glPushMatrix();
        glTranslated(size/2*x, 0.f, size/2*z);

        // 运动状态表达
        if(1.f < r_angle && r_angle < 90.f)
            glRotated(10.f, 0.f, 1.f, 0.f);
        else if(-1.f > r_angle && r_angle > -90.f)
            glRotated(-10.f, 0.f, 1.f, 0.f);

        glRotated((sysTime%(T/5))/T*5*360.f, 0.f, 0.0f, -1.f);

        glutWireTorus(size*wheel*0.7, size*wheel, 60, 60);
        glutWireCube(size*wheel*0.7f);
        glPopMatrix();
    }

    /* car top */
    glColor3ub(252u, 140u, 35u);
    glTranslated(0.f, size*(scale[1]+top/2), 0.0);
    glutSolidCube(size*top);

    // glScaled();
    glPopMatrix();
}

void Car::flow(trafficLight &fl, GLdouble zebra, Car *car) {
    /* d 控制模块
     - 车
     - 灯
     - 车
     */
    int* light[3] = {&(fl.directness), nullptr, &(fl.left)};
    GLdouble dist = front(car) - Car::size*2;

    if(target == RIGHT) ;
    else if(*(light[target]) != GREEN && translate[0] > zebra){
        // printf("pos:%f, zebra::%f, size:%f, dist:%f\n", translate[0], zebra, Car::size, dist);
        // if((translate[0]-zebra-Car::size/2) < dist && ((translate[0]-zebra-Car::size/2) < quick*quick/da/2))
            // v = 0;
        // printf("dist::%f\n", dist*0.5);
        dist = min(dist, translate[0]-zebra-Car::size);
    }
    dist = max(0, dist);
    $setA(dist);
    step();
}

void Car::step(){
    double pos = center[2] / abs(center[2]);
    if(abs(translate[2]) > R) translate[2] += v*pos;
    else if(translate[0] < -R) translate[0] -= v;
    else if(abs(translate[0]) < R)  {
        if(target == VERTICAL){
          translate[0] -= v;  
        }
        else{
            double b_angle = r_angle; // 原始度数

            double angle = v / rr;    // left为负数
            double degree = angle / PI * 180.f;

            r_angle -= degree;
            if(target==RIGHT) {
                translate[0] -= abs(rr * (sin(r_angle / 180.f * PI) - sin(b_angle / 180.f * PI)));
                translate[2] += abs(rr * 
                                (cos(r_angle / 180.f * PI) - cos(b_angle / 180.f * PI)));
            }else if(target==LEFT){
                translate[0] -= abs(rr * (sin(r_angle / 180.f * PI) - sin(b_angle / 180.f * PI)));
                translate[2] -= abs(rr * 
                                (cos(r_angle / 180.f * PI) - cos(b_angle / 180.f * PI)));
            }
        }
    }else translate[0] -= v;
}

void Car::$setA(GLdouble dist) {

    if(dist < quick*quick/2/da) a = -da; // 刹车 距离判断
    else a = da;
    if(a > 0 && v > quick)      a = 0;
    else if(a < 0 && v < 0) a = 0;

    v += a;
    v = min(quick, v);
    v = max(0.0,     v);
}

GLdouble Car::front(Car* car){
    GLdouble degree = r_angle / 180.f * PI;
    GLdouble dis[3];
    GLdouble dir[3] = {-cos(degree), 0.f, sin(degree)};
    // if(target==LEFT) dir[2] = -abs(sin(degree));
    // else if(target == RIGHT) dir[2] = abs(sin(degree));

    GLdouble accumulate = 0.f;
    const GLdouble M = 9999999;

    if(car == nullptr){
        return M;
    }

    for (int i = 0; i < 3; i++)
        dis[i] = car->translate[i] - this->translate[i];

    for(int i=0; i < 3; ++i)
        accumulate += dir[i] * dis[i];
    return accumulate;
}

GLubyte trafficLight::colors[3][3] = {
     65u, 174u,  60u,
    251u, 200u,  47u,
    244u, 62u,    6u
};

void trafficLight::flow() {
    int *light[2] = {&directness, &left};
    for(int i=0; i < 2; ++i){
        int solute_time = (sysTime + T/4*(pos%2) + T/2*i) % T;
        if(solute_time > T/4) *(light[i]) = RED;
        else if(solute_time > T/5) *(light[i]) = YELLOW;
        else if(solute_time > T/20) *(light[i]) = GREEN;
        else *(light[i]) = YELLOW;
    }
    // printf("%d %d \n", directness, left);

}

void trafficLight::display() {
    GLdouble height = SPACE / 12;
    GLdouble dz = SPACE / 9/2/2;
    GLdouble size = abs(dz)/2;
    glPushMatrix();
    glRotated(pos*90.f, 0.f, 1.f, 0.f);

    // printf("%d %d\n", left, directness);
    glPushMatrix();{ // light
        glTranslated(R, height, dz/2);
        glColor3ubv(colors[left]);
        glutSolidSphere(size, 60, 60);

        glTranslated(0, 0, dz);
        glColor3ubv(colors[directness]);
        glutSolidSphere(size, 60, 60); 
    }glPopMatrix();
    
    glColor3ub(80u, 62u, 42u);
    glPushMatrix();{ // 杆
        glTranslated(R, height/2, R);
        glScaled(0.05f, 1.f, 0.05f);
        glutSolidCube(height);
    }glPopMatrix();

    glPopMatrix();
}

void light(){
    GLfloat light_ambient[]     = { 0.1, 0.1, 0.1, 1.0 };   // 环境光
    GLfloat light_diffuse[]     = { 0.5, 0.5, 0.5, 1.0 };   // 漫反射
    GLfloat light_specular[]    = { 1.0, 1.0, 1.0, 1.0 };   // 镜面反射
    GLfloat light_position[4]   = { 0.0, 1.0, 0.0, 0.0};
    // /* 光照 */
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);    // 环绕光
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);    // 漫反射
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);  // 镜面反射

    /* 点光源 */
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.f);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 100.0);

    GLfloat nolight_ambient[]   = { 0.5, 0.5, 0.5, 1.0 };   // 无光源环境光
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, nolight_ambient);

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
    // glEnable(GL_CULL_FACE);                                     // 物体内部光线渲染
    // glCullFace(GL_BACK);
    // glCullFace(GL_FRONT);

    glEnable(GL_LIGHTING);                                      // 开灯
    glEnable(GL_LIGHT0);

}

void init_machine(){
    for(int i=0; i < 4; ++i)
        trafficLights[i] = trafficLight(i);
}

void time_flow(){
    int rand_num = rand() % 12;

    // glRotated(1.f/30, 0.f, 1.f, 0.f);

    sysTime = (sysTime +1) % T;
    for(int i=0; i < 4; ++i)
        trafficLights[i].flow();

    /* cars */
    if(sysTime % 20 == 0){
        if(cars_pt[rand_num].size() == 0)
            cars_pt[rand_num].push_back(Car(rand_num/3, rand_num%3));
        else {
            GLdouble dist = SPACE/2 - cars_pt[rand_num].back().translate[0];
            if(dist > Car::size*2)
                cars_pt[rand_num].push_back(Car(rand_num/3, rand_num%3));
        }
    }

    for(int i=0; i < 12; ++i) {
        Car* car = nullptr;
        for(int j=0; j < (int)cars_pt[i].size(); ++j) {
            cars_pt[i][j].flow(trafficLights[i%4], R, car);
            car = &cars_pt[i][j];
            // printf("%3d %3d ", i, j);
            // cars_pt[i][j].show();
        }
    }

    for(int i=0; i < 12; ++i) {
        vector<Car>::iterator iter = cars_pt[i].begin();
        while(iter != cars_pt[i].end() && out(*iter)){
            iter = cars_pt[i].erase(iter);
        }
    }
}

bool out(Car &car){
    if(abs(car.translate[0]) > SPACE/2) return true;
    if(abs(car.translate[2]) > SPACE/2) return true;
    return false;
}
