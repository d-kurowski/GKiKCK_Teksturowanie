#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <windows.h>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <cmath>
#include <gl/gl.h>
#include <gl/glut.h>

const float pi = M_PI;
											   
bool isSideVisible[5] = {true, true, true, true, true};

typedef float point3[3];
typedef float point2[2];
static GLfloat viewer[] = { 0.0, 0.0, 0.0 };

static GLfloat pixels2radians;

static GLfloat phi = 0.0;
static GLfloat theta = 0.0;
static GLfloat R = 15;

static bool left_pressed = false;
static bool right_pressed = false;

static int x_pos_old = 0;
static int y_pos_old = 0;

static int delta_x = 0;
static int delta_y = 0;

static GLfloat UP_y = 1.0;

GLbyte *LoadTGAImage(const char *FileName, GLint *ImWidth, GLint *ImHeight, GLint *ImComponents, GLenum *ImFormat) {

#pragma pack(1)            
	typedef struct
	{
		GLbyte    idlength;
		GLbyte    colormaptype;
		GLbyte    datatypecode;
		unsigned short    colormapstart;
		unsigned short    colormaplength;
		unsigned char     colormapdepth;
		unsigned short    x_orgin;
		unsigned short    y_orgin;
		unsigned short    width;
		unsigned short    height;
		GLbyte    bitsperpixel;
		GLbyte    descriptor;
	}TGAHEADER;
#pragma pack(8)

	FILE *pFile;
	TGAHEADER tgaHeader;
	unsigned long lImageSize;
	short sDepth;
	GLbyte *pbitsperpixel = NULL;

	*ImWidth = 0;
	*ImHeight = 0;
	*ImFormat = GL_BGR_EXT;
	*ImComponents = GL_RGB8;

	pFile = fopen(FileName, "rb");
	if (pFile == NULL)
		return NULL;

	fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);

	*ImWidth = tgaHeader.width;
	*ImHeight = tgaHeader.height;
	sDepth = tgaHeader.bitsperpixel / 8;

	if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
		return NULL;

	lImageSize = tgaHeader.width * tgaHeader.height * sDepth;

	pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));

	if (pbitsperpixel == NULL)
		return NULL;

	if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1)
	{
		free(pbitsperpixel);
		return NULL;
	}

	switch (sDepth)
	{
	case 3:
		*ImFormat = GL_BGR_EXT;
		*ImComponents = GL_RGB8;
		break;
	case 4:
		*ImFormat = GL_BGRA_EXT;
		*ImComponents = GL_RGBA8;
		break;
	case 1:
		*ImFormat = GL_LUMINANCE;
		*ImComponents = GL_LUMINANCE8;
		break;
	};

	fclose(pFile);

	return pbitsperpixel;
}

struct PyramidPoints {
	double x;
	double y;
	double z;
};

void drawPyramid(PyramidPoints point1, PyramidPoints point2, PyramidPoints point3, PyramidPoints point4, PyramidPoints point5) {

	glColor3f(1.0, 1.0, 1.0);

	glBegin(GL_TRIANGLES);
	if (isSideVisible[0]) {
		glNormal3f(point1.x, point1.y, point1.z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(point1.x, point1.y, point1.z);

		glNormal3f(point2.x, point2.y, point2.z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(point2.x, point2.y, point2.z);

		glNormal3f(point3.x, point3.y, point3.z);
		glTexCoord2f(0.5f, 1.0f);
		glVertex3f(point3.x, point3.y, point3.z);
	}
	if (isSideVisible[1]) {
		glNormal3f(point1.x, point1.y, point1.z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(point1.x, point1.y, point1.z);

		glNormal3f(point3.x, point3.y, point3.z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(point3.x, point3.y, point3.z);

		glNormal3f(point4.x, point4.y, point4.z);
		glTexCoord2f(0.5f, 1.0f);
		glVertex3f(point4.x, point4.y, point4.z);
	}
	if (isSideVisible[2]) {
		glNormal3f(point1.x, point1.y, point1.z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(point1.x, point1.y, point1.z);

		glNormal3f(point4.x, point4.y, point4.z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(point4.x, point4.y, point4.z);

		glNormal3f(point5.x, point5.y, point5.z);
		glTexCoord2f(0.5f, 1.0f);
		glVertex3f(point5.x, point5.y, point5.z);
	}
	if (isSideVisible[3]) {
		glNormal3f(point1.x, point1.y, point1.z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(point1.x, point1.y, point1.z);

		glNormal3f(point5.x, point5.y, point5.z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(point5.x, point5.y, point5.z);

		glNormal3f(point2.x, point2.y, point2.z);
		glTexCoord2f(0.5f, 1.0f);
		glVertex3f(point2.x, point2.y, point2.z);
	}
	glEnd();

	if (isSideVisible[4]) {
		glBegin(GL_QUADS);
			glNormal3f(1.0, -1.0, 1.0);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(point5.x, point5.y, point5.z);

			glNormal3f(1.0, -1.0, 1.0);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(point4.x, point4.y, point4.z);

			glNormal3f(1.0, -1.0, 1.0);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(point3.x, point3.y, point3.z);

			glNormal3f(1.0, -1.0, 1.0);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(point2.x, point2.y, point2.z);
		glEnd();
	}
}

void Mouse(int btn, int state, int x, int y) {

	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		x_pos_old = x;
		y_pos_old = y;

		left_pressed = true;
	}
	else {
		left_pressed = false;
	}

	if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		x_pos_old = x;

		right_pressed = true;
	}
	else {
		right_pressed = false;
	}
}

void Motion(GLsizei x, GLsizei y) {

	delta_x = x - x_pos_old;
	delta_y = y - y_pos_old;

	x_pos_old = x;
	y_pos_old = y;

	glutPostRedisplay();
}

void RenderScene(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	const float ROTATION_SPEDD = 150;

	viewer[0] = R * cos(theta) * cos(phi);
	viewer[1] = R * sin(phi);
	viewer[2] = R * sin(theta) * cos(phi);
	gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, UP_y, 0.0);

	if (left_pressed) {
		theta += (delta_x * pixels2radians) * ROTATION_SPEDD;
		phi += (delta_y * pixels2radians) * ROTATION_SPEDD;

		if (phi > 2 * M_PI) { phi -= 2 * M_PI; }
		else if (phi < -(2 * M_PI)) { phi += 2 * M_PI; }

		if (theta > 2 * M_PI) { theta -= 2 * M_PI; }
		else if (theta < -(2 * M_PI)) { theta += 2 * M_PI; }

		if (abs(phi) > M_PI / 2 && abs(phi) <= 3 * M_PI / 2) { UP_y = -1.0; }
		else { UP_y = 1.0; }
	}

	if (right_pressed) {
		R += (delta_x / 100.f);
	}

	drawPyramid({ 0.0, 4.0, 0.0 }, { -4.0, -4.0, 4.0 }, { 4.0, -4.0, 4.0 }, { 4.0, -4.0, -4.0 }, { -4.0, -4.0, -4.0 });

	glFlush();
	glutSwapBuffers();
}

void Keys(unsigned char key, int x, int y) {

	if (key == '1') { isSideVisible[0] = !isSideVisible[0]; }
	if (key == '2') { isSideVisible[1] = !isSideVisible[1]; }
	if (key == '3') { isSideVisible[2] = !isSideVisible[2]; }
	if (key == '4') { isSideVisible[3] = !isSideVisible[3]; }
	if (key == '5') { isSideVisible[4] = !isSideVisible[4]; }
	RenderScene();
}

void MyInit(void) {

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	GLbyte *pBytes;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;

	pBytes = LoadTGAImage("textury/P3_t.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);

	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void ChangeSize(GLsizei horizontal, GLsizei vertical) {

	pixels2radians = ((M_PI / 180) / (float)horizontal);

	GLfloat AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;

	if (vertical == 0) { vertical = 1; }
	glViewport(0, 0, horizontal, vertical);


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(70, AspectRatio, 1.0, 40.0);

	if (horizontal <= vertical)
		glViewport(-7.5, 7.5, -7.5 / AspectRatio, 7.5 / AspectRatio);
	else
		glViewport(-7.5*AspectRatio, 7.5*AspectRatio, -7.5, 7.5);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char* argv[]) {
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Teksturowanie piramidy");

	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	MyInit();
	glEnable(GL_DEPTH_TEST);

	glutKeyboardFunc(Keys);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutMainLoop();
}
