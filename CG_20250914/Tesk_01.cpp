#include <iostream>
#include <glew.h>
#include <freeglut.h>
#include <freeglut_ext.h>
#include <random>

//#include <glm/glm.hpp>
//#include <glm/ext.hpp>
//#include <glm/gtc/matrix_transform.hpp>

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

GLvoid Keyboard(unsigned char key, int x, int y);

GLfloat bgColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
bool started = false;

std::random_device rd;
std::mt19937 gen(rd());  // Mersenne Twister 엔진
std::uniform_real_distribution<float> dist(0.0f, 1.0f); // 0.0 ~ 1.0

void main(int argc, char** argv)
{
    glutInit(&argc, argv);                        // GLUT와 openGL환경 초기화
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 윈도우 더블 버퍼 | RGBA(기본) 모드 (DEPH(3차원에서 사용))
    glutInitWindowPosition(100, 100);             // 윈도우의 좌측 상단 모서리 좌표
    glutInitWindowSize(800, 600);                 // 윈도우 크기(픽셀 단위)
    glutCreateWindow("Tesk_1");                   // 윈도우 생성

    // GLEW 초기화
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
        std::cout << "GLEW Initialized\n";

    glutDisplayFunc(drawScene);  // 출력 콜백 함수
    glutReshapeFunc(Reshape);    // 다시 그리기 콜백 함수(윈도우 크기가 변했을 때 취할 동작을 지정)
    // gulitSpecialFunc(SpecialKeyboard);  // 아스키가 아닌 특별한 키 ex) GULT_KEY_LEFT
    // gultMouseFunc(Mouse);     // 마우스 콜백 함수
    // gultMotionFunc(Motion);    // 마우스 버튼을 누른 채 움직일 때 호출될 콜백 함수
    // gultPassiveMotionFunc(Motion); // 마우스 버튼을 누르지 않은 채 움직일 때 호출될 콜백 함수

    glutKeyboardFunc(Keyboard);  // 키보드 콜백 함수

    // 지금까지 생성한 윈도우들과 여기에 그린 그림들을 화면에 출력
    glutMainLoop();  // 메인 함수는 최소 한 번 호출해야함.
}

GLvoid drawScene()
{
    glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);  // 윈도우 색상 지정(배경)
    glClear(GL_COLOR_BUFFER_BIT);                                  // 화면 클리어. 버퍼 영역을 컬러로 설정
    glutSwapBuffers();        // 그리기를 실행하는 동시에 화면에 나타나지 않는 버퍼에 렌더링.(더블 버퍼링)
                              // 스왑 명령으로 버퍼에 렌더링한 그림을 스크린상에 즉시 나타냄.
}

GLvoid Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

void Timer(int value)
{
    if (!started) return;

    bgColor[0] = dist(gen);
    bgColor[1] = dist(gen);
    bgColor[2] = dist(gen);

    glutPostRedisplay(); // 화면 갱신

    // 타이머 재설정
    glutTimerFunc(1000, Timer, 0);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'c':
        bgColor[0] = 0.0f;
        bgColor[1] = 1.0f;
        bgColor[2] = 1.0f;
        glutPostRedisplay();   // 화면 갱신
        break;

    case 'm':
        bgColor[0] = 1.0f;
        bgColor[1] = 0.0f;
        bgColor[2] = 1.0f;
        glutPostRedisplay();
        break;

    case 'y':
        bgColor[0] = 1.0f;
        bgColor[1] = 1.0f;
        bgColor[2] = 0.0f;
        glutPostRedisplay();
        break;

    case 'a':
    {
        bgColor[0] = dist(gen);
        bgColor[1] = dist(gen);
        bgColor[2] = dist(gen);
        glutPostRedisplay();
    }
    break;

    case 'w':
        bgColor[0] = 1.0f;
        bgColor[1] = 1.0f;
        bgColor[2] = 1.0f;
        glutPostRedisplay();
        break;

    case 'k':
        bgColor[0] = 0.0f;
        bgColor[1] = 0.0f;
        bgColor[2] = 0.0f;
        glutPostRedisplay();
        break;

    case 't':
        started = true;

        if (started)
        {
            glutTimerFunc(1000, Timer, 0);   // 타이머 콜백 함수
        }
        break;

    case 's':
        started = false;
        break;

    case 'q':
        exit(0);
        break;
    }
}