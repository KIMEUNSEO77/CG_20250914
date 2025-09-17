#include <iostream>
#include <glew.h>
#include <freeglut.h>
#include <freeglut_ext.h>
#include <random>
#include <vector>
using namespace std;

//#include <glm/glm.hpp>
//#include <glm/ext.hpp>
//#include <glm/gtc/matrix_transform.hpp>
int width = 800;
int height = 600;

const int MAX_RECTS = 30;

struct Rec
{
    float color[3];
    float scale;
    float posX, posY;
    float width, height;
};

vector<Rec> rects;

int dragIndex = -1;  // 드래그 중인 사각형 인덱스
float dragMouseX = 0.0f;
float dragMouseY = 0.0f;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

// 랜덤 색상 생성
void RandomColor(float color[3])
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    color[0] = dis(gen);
    color[1] = dis(gen);
    color[2] = dis(gen);
}

// 랜덤 위치 생성
void RandomPosition(float& posX, float& posY)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    posX = dis(gen);
    posY = dis(gen);
}

// 사각형 추가
void AddRects()
{
    if (rects.size() >= 10) return;

    Rec temp{};
    RandomPosition(temp.posX, temp.posY);
    RandomColor(temp.color);
    temp.scale = 1.0f;
    temp.width = 0.2f;
    temp.height = 0.2f;

    rects.push_back(temp);  
}

// 사각형 그리기
void DrawRect(const Rec& rect)
{
    glPushMatrix();
    glTranslatef(rect.posX, rect.posY, 0.0f);  // 사각형의 중심을 posX, posY로 이동
    glScalef(rect.scale, rect.scale, 1.0f);
    glColor3fv(rect.color);
    glRectf(
        -rect.width / 2, -rect.height / 2,
        rect.width / 2, rect.height / 2
    );
    glPopMatrix();
}

// 마우스 클릭한 좌표 정규화
void PixelTrans(int px, int py, float& nx, float& ny)
{
    float w = (float)glutGet(GLUT_WINDOW_WIDTH);
    float h = (float)glutGet(GLUT_WINDOW_HEIGHT);
    nx = (px / w) * 2.0f - 1.0f;
    ny = 1.0f - (py / h) * 2.0f;
}

// 중심 좌표 화면 밖으로 못 나가게
void MouseInside(Rec& r)
{
    float w = (r.width * r.scale) * 0.5f;
    float h = (r.height * r.scale) * 0.5f;

    if (r.posX < -1.0f + w) r.posX = -1.0f + w;
    if (r.posX > 1.0f - w) r.posX = 1.0f - w;
    if (r.posY < -1.0f + h) r.posY = -1.0f + h;
    if (r.posY > 1.0f - h) r.posY = 1.0f - h;
}

// 마우스가 어느 사각형을 클릭하는지
int PickRect(float x, float y)
{
    // 마지막에 그린 것이 위에 있으니 뒤에서 앞으로 검사
    for (int i = rects.size() - 1; i >= 0; --i)
    {
        float halfW = rects[i].width * rects[i].scale * 0.5f;
        float halfH = rects[i].height * rects[i].scale * 0.5f;
        float left = rects[i].posX - halfW;
        float right = rects[i].posX + halfW;
        float bottom = rects[i].posY - halfH;
        float top = rects[i].posY + halfH;

        if (x >= left && x <= right && y >= bottom && y <= top)
            return i;
    }
    return -1;
}

// 충돌 체크할 준비!!!!
void GetCollid(const Rec& r, float& left, float& right, float& bottom, float& top)
{
    float halfW = (r.width * r.scale) * 0.5f;
    float halfH = (r.height * r.scale) * 0.5f;
    left = r.posX - halfW;
    right = r.posX + halfW;
    bottom = r.posY - halfH;
    top = r.posY + halfH;
}

// 충돌 체크
bool CheckCollid(float l1, float r1, float b1, float t1,
    float l2, float r2, float b2, float t2)
{
    if (r1 < l2 || r2 < l1) return false;
    if (t1 < b2 || t2 < b1) return false;

    return true;
}

void SetRec(Rec& r, float left, float right, float bottom, float top)
{
    r.posX = (left + right) * 0.5f;
    r.posY = (bottom + top) * 0.5f;
    r.width = (right - left);
    r.height = (top - bottom);
    r.scale = 1.0f;
}

void RemoveRect(int idx)
{
    if (idx < 0 || idx >= rects.size()) return;
    rects.erase(rects.begin() + idx); // 앞으로 당겨지면서 삭제
}

// 합체
void TryMerge(int& idx)
{
    if (idx < 0 || idx >= rects.size()) return;

    // 동시 합체 방지
    bool merged = true;
    while (merged) 
    {
        merged = false;

        float l1, r1, b1, t1;
        GetCollid(rects[idx], l1, r1, b1, t1);

        for (int i = rects.size() - 1; i >= 0; --i) 
        {
            // 이 사각형 외의 사각형만 검사
            if (i == idx) continue;

            float l2, r2, b2, t2;
            GetCollid(rects[i], l2, r2, b2, t2);

            if (CheckCollid(l1, r1, b1, t1, l2, r2, b2, t2)) 
            {
                // 합쳐질 사각형
                float L = std::min(l1, l2);
                float R = std::max(r1, r2);
                float B = std::min(b1, b2);
                float T = std::max(t1, t2);

                SetRec(rects[idx], L, R, B, T);
                RandomColor(rects[idx].color);

                // 검사된 사각형은 삭제
                RemoveRect(i);

                // idx가 삭제된 인덱스보다 뒤라면 한 칸 앞으로
                if (i < idx) idx--;

                merged = true;
                break; // 다시 처음부터 검사
            }
        }
    }

    // 화면 밖 방지
    MouseInside(rects[idx]);
}

// 기존 사각형의 자식? 만들기
void MakeChild(const Rec& base, Rec& A, Rec& B)
{
    A = B = Rec{};
    A.scale = B.scale = base.scale;

    // 가로가 더 길면 가로로, 세로면 세로로 절반
    if (base.width >= base.height) 
    {
        A.width = B.width = base.width * 0.5f;
        A.height = B.height = base.height;

        float dx = (A.width * A.scale) * 0.5f;
        A.posX = base.posX - dx;  A.posY = base.posY;
        B.posX = base.posX + dx;  B.posY = base.posY;
    }
    else 
    {
        A.width = B.width = base.width;
        A.height = B.height = base.height * 0.5f;

        float dy = (A.height * A.scale) * 0.5f;
        A.posX = base.posX;  A.posY = base.posY + dy;
        B.posX = base.posX;  B.posY = base.posY - dy;
    }

    RandomColor(A.color);
    RandomColor(B.color);

    MouseInside(A);
    MouseInside(B);
}

// 분해
void SplitRect(int idx)
{
    if (idx < 0 || idx >= rects.size()) return;

    int recCount = rects.size();
    if (recCount >= MAX_RECTS) return;

    // 부모 백업 후 삭제
    Rec base = rects[idx];
    rects.erase(rects.begin() + idx);
    recCount--;

    // 자식 두 개 생성
    Rec A, B;
    MakeChild(base, A, B);

    if (recCount <= MAX_RECTS - 2) 
    {
        rects.push_back(A);
        rects.push_back(B);
    }
    else if (recCount == MAX_RECTS - 1) 
    {
        rects.push_back(A); // 한 칸만 남았으면 하나만
    }
}

// 마우스 콜백
void Mouse(int button, int state, int x, int y)
{
    float nx, ny;  // 마우스가 클릭한 좌표를 정규화
    PixelTrans(x, y, nx, ny);

    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            int hit = PickRect(nx, ny);
            if (hit != -1)
            {
                dragIndex = hit;
                // 마우스가 중심에서 얼마나 떨어져있는지
                dragMouseX = rects[hit].posX - nx; 
                dragMouseY = rects[hit].posY - ny;
                //dragMouseX = rects[hit].posX;
                //dragMouseY = rects[hit].posY;
            }
        }
        else if (state == GLUT_UP)
        {
            dragIndex = -1; // 드래그 종료
        }
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        int hit = PickRect(nx, ny);
        if (hit != -1) 
        {
            if (dragIndex == hit) dragIndex = -1; // 드래그 중이면 해제
            SplitRect(hit);                     
            glutPostRedisplay();
        }
    }
    glutPostRedisplay();
}

// 마우스 드래그 콜백
void MouseDrag(int x, int y)
{
    if (dragIndex == -1) return;

    float nx, ny;   // x, y를 정규화한 좌표
    PixelTrans(x, y, nx, ny);

    // 마우스 커서가 중심으로 바로 가는걸 방지
    rects[dragIndex].posX = nx + dragMouseX;
    rects[dragIndex].posY = ny + dragMouseY;
    //rects[dragIndex].posX = nx;
    //rects[dragIndex].posY = ny;

    MouseInside(rects[dragIndex]);

    TryMerge(dragIndex);

    glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'a':
        AddRects();
        glutPostRedisplay();  // 화면 갱신
        break;
    }
}

void main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("Tesk_03");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
        std::cout << "GLEW Initialized\n";

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);

    glutKeyboardFunc(Keyboard);

    glutMouseFunc(Mouse);
    glutMotionFunc(MouseDrag);

    glutMainLoop();
}

GLvoid drawScene()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < rects.size(); ++i)
    {
        DrawRect(rects[i]);
    }

    glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}