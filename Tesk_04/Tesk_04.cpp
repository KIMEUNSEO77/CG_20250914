#include <iostream>
#include <glew.h>
#include <freeglut.h>
#include <freeglut_ext.h>
#include <vector>
#include <random>
using namespace std;

int width = 800;
int height = 600;

struct Rec
{
    float color[3];
    float scale;
    float posX, posY;
    float width, height;

    // 움직이는 방향
    int moveIndex = -1;
};
vector<Rec> rects;

struct Pos
{
    float posX;
    float posY;
};
vector<Pos> origins; // 원래 위치 저장
// 대각선으로 움직일 방향(오른쪽 아래, 왼쪽위, 왼쪽 아래, 오른쪽 위)
Pos dPos[4] = { {0.01f, -0.01f}, {-0.01f, 0.01f}, {-0.01f, -0.01f}, {0.01f, 0.01f}};
Pos zPos[4] = { {0.05f, -0.05f}, {-0.05f, 0.05f}, {-0.05f, -0.05f}, {0.05f, 0.05f} };

// 마우스 클릭 위치
float mouseX = 0.0f;
float mouseY = 0.0f;

// 애니메이션 시작 유무
bool started[5];

// 따라갈 사각형
int center = -1;

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

// 랜덤 크기 설정
void RandomSize(float& width, float& height)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.1f, 0.6f);

    width = dis(gen);
    height = dis(gen);
}

// 사각형 추가
void AddRects()
{
    if (rects.size() >= 5) return;

    Rec temp{};
    temp.posX = mouseX;
    temp.posY = mouseY;
    RandomColor(temp.color);
    temp.scale = 1.0f;
    temp.width = 0.2f;
    temp.height = 0.2f;

    // 방향 지정
    temp.moveIndex = rand() % 4;

    rects.push_back(temp);

    // 원래 좌표도 저장
    if (origins.size() <= 5)
        origins.push_back({ temp.posX, temp.posY });
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

void Mouse(int button, int state, int x, int y)
{
    float nx, ny;
    PixelTrans(x, y, nx, ny);

    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            mouseX = nx;
            mouseY = ny;
            AddRects();
            glutPostRedisplay(); // 화면 갱신
        }
    }
}

// 리셋 함수
void Reset()
{
    rects.clear();
    origins.clear();

    for (int i = 0; i < 5; i++)
        started[i] = false;

    glutPostRedisplay(); // 화면 갱신
}

// 원래 위치로 이동
void MoveToOrigin()
{
    for (int i = 0; i < rects.size(); i++)
    {
        rects[i].posX = origins[i].posX;
        rects[i].posY = origins[i].posY;
    }
    glutPostRedisplay(); // 화면 갱신
}

// 벽에 닿았는지 확인하는 함수
bool IsWall(const Rec& rec, float x, float y)
{
    float halfW = rec.width * rec.scale * 0.5f;
    float halfH = rec.height * rec.scale * 0.5f;

    // 정규화 좌표
    if (x - halfW <= -1.0f) return true; // 왼쪽
    if (x + halfW >= 1.0f) return true; // 오른쪽
    if (y - halfH <= -1.0f) return true; // 아래
    if (y + halfH >= 1.0f) return true; // 위
    return false;
}

// 모든 사각형이 대각선으로 움직이다가 벽에 닿았으면 방향 전환
void Animation_1()
{
    for (int i = 0; i < rects.size(); i++)
    {
        float nx = rects[i].posX + dPos[rects[i].moveIndex].posX;
        float ny = rects[i].posY + dPos[rects[i].moveIndex].posY;

        if (IsWall(rects[i], nx, ny))
        {
            rects[i].moveIndex = (rects[i].moveIndex + 1) % 4;
        }
        else
        {
            rects[i].posX += dPos[rects[i].moveIndex].posX;
            rects[i].posY += dPos[rects[i].moveIndex].posY;
        }
    }
}

// 모든 사각형을 지그재그로 이동
void Animation_2()
{
    for (int i = 0; i < rects.size(); i++)
    {
        float nx = rects[i].posX + zPos[(rects[i].moveIndex + 2) % 4].posX;
        float ny = rects[i].posY + zPos[(rects[i].moveIndex + 2) % 4].posY;

        if (IsWall(rects[i], nx, ny))
        {
            rects[i].moveIndex = (rects[i].moveIndex + 1) % 4;
        }
        else
        {
            rects[i].posX = nx;
            rects[i].posY = ny;

            rects[i].moveIndex = (rects[i].moveIndex + 2) % 4;
        }
    }
}

// 모든 사각형의 크기를 랜덤하게 변경
void Animation_3()
{
    for (int i = 0; i < rects.size(); i++)
        RandomSize(rects[i].width, rects[i].height);
}

// 모든 사각형 색상을 변경
void Animation_4()
{
    for (int i = 0; i < rects.size(); i++)
        RandomColor(rects[i].color);
}

// 따라할 랜덤한 사각형 선택
int randomRect()
{
    int random = rand() % rects.size();
    return random;
}

void PickCenter() 
{
    if (center < 0) center = randomRect(); // 한 번만 선택
    // 혹은 키 입력으로 centerIdx를 바꾸도록!
}

// 선택된 사각형을 따라가기
void Animation_5()
{
    PickCenter();

    float nx = rects[center].posX + dPos[rects[center].moveIndex].posX;
    float ny = rects[center].posY + dPos[rects[center].moveIndex].posY;

    if (IsWall(rects[center], nx, ny))
    {
        rects[center].moveIndex = (rects[center].moveIndex + 1) % 4;
    }
    else
    {
        rects[center].posX += dPos[rects[center].moveIndex].posX;
        rects[center].posY += dPos[rects[center].moveIndex].posY;
    }

    for (int i = 0; i < rects.size(); i++)
    {
        if (i == center) continue;

        float nx = rects[i].posX + dPos[rects[center].moveIndex].posX;
        float ny = rects[i].posY + dPos[rects[center].moveIndex].posY;

        if (IsWall(rects[i], nx, ny))
        {
            rects[i].moveIndex = (rects[i].moveIndex + 1) % 4;
        }
        else
        {
            rects[i].posX += dPos[rects[center].moveIndex].posX;
            rects[i].posY += dPos[rects[center].moveIndex].posY;
        }
    }
}

// 타이머
void Timer(int value)
{
    if (started[0])
    {
        Animation_1();
        glutPostRedisplay(); // 화면 갱신
    }

    if (started[1])
    {
        Animation_2();
        glutPostRedisplay();
    }

    if (started[2])
    {
        Animation_3();
        glutPostRedisplay(); // 화면 갱신
    }

    if (started[3])
    {
        Animation_4();
        glutPostRedisplay(); // 화면 갱신
    }

    if (started[4])
    {
        Animation_5();
        glutPostRedisplay(); // 화면 갱신
    }

    // 타이머 재설정
    glutTimerFunc(50, Timer, 0);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case '1':
        started[0] = !started[0];
        if (started[4]) started[4] = false;
        if (started[1]) started[1] = false;
        break;
    case '2':
        started[1] = !started[1];
        if (started[0]) started[0] = false;
        break;
    case '3':
        started[2] = !started[2];
        break;
    case '4':
        started[3] = !started[3];
        break;
    case '5':
        started[4] = !started[4];
        if (started[0]) started[0] = false;
        break;
    case 's':
        for (int i = 0; i < 5; i++)
            started[i] = false;
        break;
    case 'm':
        MoveToOrigin();
        break;
    case 'r':
        Reset();
        break;
    case 'q':
        exit(0);
        break;
    }
}

void main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("Tesk_04");

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

    glutTimerFunc(50, Timer, 0);   // 타이머 콜백 함수

    glutMainLoop();
}

GLvoid drawScene()
{
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
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