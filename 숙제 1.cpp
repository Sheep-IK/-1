#define _CRT_SECURE_NO_WARNINGS
#define WIDTH  600
#define HEIGHT  600




#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <random>
#include <time.h>

typedef struct inform {

	GLuint vao;
	GLuint vbo[2];

}inform;

typedef struct shape {

	float spot[4][3];
	float color[4][3];

}shape;


inform entity[100];
inform line;

shape fruit[100];
float knife[2][3];
float knifeColor[2][3] = { {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f} };


float LineX1;
float LineX2;
float LineY1;
float LineY2;

int number;

bool max = false;
bool face = true;
bool wire = false;
bool draw = false;

//--- 필요한 변수 선언
GLint width, height;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체



char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading
	if (!fptr) // Return NULL on failure
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file
	length = ftell(fptr); // Find out how many bytes into the file we are
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
	fclose(fptr); // Close the file
	buf[length] = 0; // Null terminator
	return buf; // Return the buffer
}

void TimerFunction(int value);
void drawknife();
void UpdateVBO();
void UpdateLineVBO();
void make_vertexShaders()//--- 버텍스 세이더 객체 만들기
{
	GLchar* vertexSource;
	vertexSource = filetobuf("vertex.glsl");
	//--- 버텍스 세이더 객체 만들기
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- 버텍스 세이더 컴파일하기
	glCompileShader(vertexShader);
	//--- 컴파일이 제대로 되지 않은 경우: 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);


}
void make_fragmentShaders()//--- 프래그먼트 세이더 객체 만들기
{
	GLchar* fragmentSource; //--- 소스코드 저장 변수
	fragmentSource = filetobuf("fragment.glsl");
	//--- 프래그먼트 세이더 객체 만들기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- 프래그먼트 세이더 컴파일
	glCompileShader(fragmentShader);
	//--- 컴파일이 제대로 되지 않은 경우: 컴파일 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);

}
void make_shaderProgram() {
	make_vertexShaders();
	make_fragmentShaders();

	//-- shader Program
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);
	//--- 세이더 삭제하기
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//--- Shader Program 사용하기
	glUseProgram(shaderProgramID);
}

void InitBuffer()
{
	for (int i = 0; i < 100; ++i) {
		glGenVertexArrays(1, &entity[i].vao); //--- VAO 를 지정하고 할당하기
		glBindVertexArray(entity[i].vao); //--- VAO를 바인드하기
		glGenBuffers(2, entity[i].vbo); //--- 2개의 VBO를 지정하고 할당하기

		//위치
		glBindBuffer(GL_ARRAY_BUFFER, entity[i].vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(fruit[i].spot), fruit[i].spot, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //n각형

		glEnableVertexAttribArray(0);


		//컬러
		glBindBuffer(GL_ARRAY_BUFFER, entity[i].vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(fruit[i].color), fruit[i].color, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); //n각형
		glEnableVertexAttribArray(1);
	}

}//--- 아래 5개 함수는 사용자 정의 함수 임
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void Keyboard(unsigned char key, int x, int y);

GLvoid drawScene();
GLvoid Reshape(int w, int h);




//--- 메인 함수
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	srand(time(NULL));

	for (int i = 0; i < 3; ++i) {

		for (int k = 0; k < 4; ++k) {
			for (int j = 0; j < 3; ++j) {
				fruit[i].color[k][j] = rand() % 10 / 10.0;
			}
		}
	}

	{
		//entity[0].spot[0][0] = spotX2;	//x
		//entity[0].spot[0][1] = spotY2;	//y
		//entity[0].spot[0][2] = 0;		//z
		//entity[0].spot[1][0] = spotX3;
		//entity[0].spot[1][1] = spotY3;
		//entity[0].spot[1][2] = 0;
		//entity[0].spot[2][0] = spotX4;
		//entity[0].spot[2][1] = spotY4;
		//entity[0].spot[2][2] = 0;
		//entity[0].spot[3][0] = spotX1;
		//entity[0].spot[3][1] = spotY1;
		//entity[0].spot[3][2] = 0;

	}	//entity 값 넣어주기


	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(10, 10);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Example1");
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();

	make_shaderProgram();

	{	//좌표축 묶기
		glGenVertexArrays(1, &line.vao); //--- VAO 를 지정하고 할당하기
		glBindVertexArray(line.vao); //--- VAO를 바인드하기
		glGenBuffers(2, line.vbo); //--- 2개의 VBO를 지정하고 할당하기

		//위치
		glBindBuffer(GL_ARRAY_BUFFER, line.vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(knife), knife, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //n각형
		glEnableVertexAttribArray(0);

		//컬러
		glBindBuffer(GL_ARRAY_BUFFER, line.vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(knifeColor), knifeColor, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); //n각형
		glEnableVertexAttribArray(1);



		//glGenVertexArrays(1, &Line[1].vao); //--- VAO 를 지정하고 할당하기
		//glBindVertexArray(Line[1].vao); //--- VAO를 바인드하기
		//glGenBuffers(2, Line[1].vbo); //--- 2개의 VBO를 지정하고 할당하기

		////위치
		//glBindBuffer(GL_ARRAY_BUFFER, Line[1].vbo[0]);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(Yline), Yline, GL_STATIC_DRAW);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //n각형
		//glEnableVertexAttribArray(0);

		////컬러
		//glBindBuffer(GL_ARRAY_BUFFER, Line[1].vbo[1]);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(LineColor[1]), LineColor[1], GL_STATIC_DRAW);
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); //n각형
		//glEnableVertexAttribArray(1);

		glLineWidth(3);

	}

	InitBuffer();

	//--- 세이더 프로그램 만들기
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse); // 마우스 입력 콜백함수
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard); // 키보드 입력 콜백함수
	glutTimerFunc(30, TimerFunction, 1);
	glutMainLoop();

}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	if (draw) {
		glBindVertexArray(line.vao);
		glDrawArrays(GL_LINES, 0, 2); // 두 개의 점으로 선을 그립니다.
	}

	//glBindVertexArray(Line[1].vao);
	//glDrawArrays(GL_LINES, 0, 2); // 두 개의 점으로 선을 그립니다.


		//UpdateVBO();
	UpdateLineVBO();
	number = 0;
	while (max == false) {
		
			glBindVertexArray(entity[number].vao);
			if (entity[number].vao == NULL)
				max = true;

			if (face) {
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4); //--- 렌더링하기: a번 인덱스에서 n개의 버텍스를 사용하여 Shape 그리기
			}
			if (wire) {
				glDrawArrays(GL_LINE_LOOP, 0, 4);
			}
			++number;

	 }


	glutSwapBuffers(); // 화면에 출력하기

}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'a':
		if (face)
			face = false;
		else
			face = true;
		break;

	case 'b':
		if (wire)
			wire = false;
		else
			wire = true;
		break;

	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y) {
	float mx = (x - (WIDTH / 2)) / (WIDTH / 2.0);
	float my = ((HEIGHT / 2) - y) / (HEIGHT / 2.0);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		draw = true;
		LineX1 = mx;
		LineY1 = my;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		draw = false;

		LineX1 = NULL;
		LineY1 = NULL;

		LineX2 = NULL;
		LineY2 = NULL;
	}

	glutPostRedisplay();
}


void Motion(int x, int y) {
	float mx = (x - (WIDTH / 2.0)) / (WIDTH / 2.0);
	float my = ((HEIGHT / 2.0) - y) / (HEIGHT / 2.0);
	if (draw == true) {
		LineX2 = mx;
		LineY2 = my;
		}


	glutPostRedisplay();
}

void TimerFunction(int value)
{
	//if (draw == true)
	//drawknife();

	UpdateLineVBO();
	glutTimerFunc(30, TimerFunction, 1); 

}

//void UpdateVBO() {
//	for (int i = 0; i < 100; ++i) {
//		glBindBuffer(GL_ARRAY_BUFFER, entity[i].vbo[0]);
//		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(fruit[i].spot), fruit[i].spot);
//
//		glBindBuffer(GL_ARRAY_BUFFER, entity[i].vbo[1]);
//		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(fruit[i].color), fruit[i].color);
//	}
//
//}

void UpdateLineVBO() {
	drawknife();
	for (int i = 0; i < 2; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, line.vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(knife), knife, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, line.vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(knifeColor), knifeColor, GL_STATIC_DRAW);
	}
}

void drawknife() {

	knife[0][0] = LineX1;
	knife[0][1] = LineY1;
	knife[0][2] = 0;
	knife[1][0] = LineX2;
	knife[1][1] = LineY2;
	knife[1][2] = 0;

	//for (int j = 0; j < 3; ++j) {
	//	knifeColor[0][j] = rand() % 10 / 10.0;
	//}
}