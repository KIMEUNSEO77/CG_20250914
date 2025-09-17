#include <iostream>
#include <glew.h>
#include <freeglut.h>
#include <random>
//#include <glm/glm.hpp>
//#include <glm/ext.hpp>
//#include <glm/gtc/matrix_transform.hpp>

const int width = 800;
const int height = 600;

struct Rec
{
    float color[3];
    float scale;
    float posX, posY;
    float width, height;
};

float bgColor[3] = { 1.0f, 1.0f, 1.0f };  // 배경색

Rec rects[4];
int recSpace[4] = { 0, 1, 2, 3 };  // 4사분면

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

// 클릭된 사분면 구하기
int InitSpace(float x, float y)
{
    int index = 0;

    if (y >= 0) index = 0;
    else index = 2;
    if (x >= 0) index += 1;

    return index;
}

// 랜덤 색상 생성
void RandomColor(float color[3])
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    color[0] = dis(gen);
    color[1] = dis(gen);
    color[2] = dis(gen);
}

// 사각형 초기화
void InitRects()
{
    rects[0] = { {1.0f, 0.0f, 0.0f}, 1.0f, -0.5f, 0.5f, 1.0f, 1.0f };
    rects[1] = { {0.0f, 1.0f, 0.0f}, 1.0f, 0.5f, 0.5f, 1.0f, 1.0f };
    rects[2] = { {0.0f, 0.0f, 1.0f}, 1.0f, -0.5f, -0.5f, 1.0f, 1.0f };
    rects[3] = { {1.0f, 1.0f, 0.0f}, 1.0f, 0.5f, -0.5f, 1.0f, 1.0f };
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

// 마우스 클릭 위치 확인
int CheckRectClick(float x, float y)
{
    for (int i = 0; i < 4; ++i)
    {
        float left = rects[i].posX - rects[i].width * rects[i].scale / 2;
        float right = rects[i].posX + rects[i].width * rects[i].scale / 2;
        float bottom = rects[i].posY - rects[i].height * rects[i].scale / 2;
        float top = rects[i].posY + rects[i].height * rects[i].scale / 2;

        if (x >= left && x <= right && y >= bottom && y <= top)
        {
            return i; // 클릭된 사각형 인덱스
        }
    }
    return -1;
}

// 마우스 클릭
void MouseClick(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN)
    {
        // 정규화 좌표 변환
        float nx = (x / (float)glutGet(GLUT_WINDOW_WIDTH)) * 2 - 1;
        float ny = 1 - (y / (float)glutGet(GLUT_WINDOW_HEIGHT)) * 2;

        int clickedRect = CheckRectClick(nx, ny);

        if (button == GLUT_LEFT_BUTTON)
        {
            if (clickedRect != -1) 
            {
                RandomColor(rects[clickedRect].color);
            }
            else
            {
                RandomColor(bgColor);
            }
        }
        else if (button == GLUT_RIGHT_BUTTON)
        {
            if (clickedRect != -1)
            {
                rects[clickedRect].scale = std::max(0.1f, rects[clickedRect].scale - 0.1f);
            }
            else
            {
                /*
                // 모든 사각형 크기 확대
                for (int i = 0; i < 4; i++)
                {
                    if (rects[i].scale < 1.0f)
                        rects[i].scale += 0.1f;
                }
                */
                int space = InitSpace(nx, ny); // 사분면 구하기
                int index = recSpace[space];   // 인덱스 구하기
                if (rects[index].scale < 1.0f)
                    rects[index].scale += 0.1f;
            }
        }
        glutPostRedisplay();
    }
}

void main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("Tesk_02");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
        std::cout << "GLEW Initialized\n";

    InitRects();   // 사각형 초기화

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);

    glutMouseFunc(MouseClick); // 마우스 콜백

    glutMainLoop();
}

GLvoid drawScene()
{
    glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (auto& it : rects)
    {
        DrawRect(it);   // 사각형 그리기
    }

    glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}