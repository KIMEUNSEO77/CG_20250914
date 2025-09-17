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
std::mt19937 gen(rd());  // Mersenne Twister ����
std::uniform_real_distribution<float> dist(0.0f, 1.0f); // 0.0 ~ 1.0

void main(int argc, char** argv)
{
    glutInit(&argc, argv);                        // GLUT�� openGLȯ�� �ʱ�ȭ
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ������ ���� ���� | RGBA(�⺻) ��� (DEPH(3�������� ���))
    glutInitWindowPosition(100, 100);             // �������� ���� ��� �𼭸� ��ǥ
    glutInitWindowSize(800, 600);                 // ������ ũ��(�ȼ� ����)
    glutCreateWindow("Tesk_1");                   // ������ ����

    // GLEW �ʱ�ȭ
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
        std::cout << "GLEW Initialized\n";

    glutDisplayFunc(drawScene);  // ��� �ݹ� �Լ�
    glutReshapeFunc(Reshape);    // �ٽ� �׸��� �ݹ� �Լ�(������ ũ�Ⱑ ������ �� ���� ������ ����)
    // gulitSpecialFunc(SpecialKeyboard);  // �ƽ�Ű�� �ƴ� Ư���� Ű ex) GULT_KEY_LEFT
    // gultMouseFunc(Mouse);     // ���콺 �ݹ� �Լ�
    // gultMotionFunc(Motion);    // ���콺 ��ư�� ���� ä ������ �� ȣ��� �ݹ� �Լ�
    // gultPassiveMotionFunc(Motion); // ���콺 ��ư�� ������ ���� ä ������ �� ȣ��� �ݹ� �Լ�

    glutKeyboardFunc(Keyboard);  // Ű���� �ݹ� �Լ�

    // ���ݱ��� ������ �������� ���⿡ �׸� �׸����� ȭ�鿡 ���
    glutMainLoop();  // ���� �Լ��� �ּ� �� �� ȣ���ؾ���.
}

GLvoid drawScene()
{
    glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);  // ������ ���� ����(���)
    glClear(GL_COLOR_BUFFER_BIT);                                  // ȭ�� Ŭ����. ���� ������ �÷��� ����
    glutSwapBuffers();        // �׸��⸦ �����ϴ� ���ÿ� ȭ�鿡 ��Ÿ���� �ʴ� ���ۿ� ������.(���� ���۸�)
                              // ���� ������� ���ۿ� �������� �׸��� ��ũ���� ��� ��Ÿ��.
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

    glutPostRedisplay(); // ȭ�� ����

    // Ÿ�̸� �缳��
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
        glutPostRedisplay();   // ȭ�� ����
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
            glutTimerFunc(1000, Timer, 0);   // Ÿ�̸� �ݹ� �Լ�
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