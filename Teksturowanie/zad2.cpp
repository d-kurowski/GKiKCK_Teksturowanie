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

const int N = 40;
point3 points[N + 1][N + 1];
point3 normal_vectors[N + 1][N + 1];
point2 texture_points[N + 1][N + 1];

GLbyte* LoadTGAImage(const char* FileName, GLint* ImWidth, GLint* ImHeight, GLint* ImComponents, GLenum* ImFormat) {

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

	FILE* pFile;
	TGAHEADER tgaHeader;
	unsigned long lImageSize;
	short sDepth;
	GLbyte* pbitsperpixel = NULL;

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

void drawEgg() {
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			glBegin(GL_TRIANGLES);
				glNormal3fv(normal_vectors[i][j]);
				glTexCoord2fv(texture_points[i][j]);
				glVertex3fv(points[i][j]);

				glNormal3fv(normal_vectors[i + 1][j]);
				glTexCoord2fv(texture_points[i + 1][j]);
				glVertex3fv(points[i + 1][j]);

				glNormal3fv(normal_vectors[i + 1][j + 1]);
				glTexCoord2fv(texture_points[i + 1][j + 1]);
				glVertex3fv(points[i + 1][j + 1]);
			glEnd();

			glBegin(GL_TRIANGLES);
				glNormal3fv(normal_vectors[i][j]);
				glTexCoord2fv(texture_points[i][j]);
				glVertex3fv(points[i][j]);

				glNormal3fv(normal_vectors[i][j + 1]);
				glTexCoord2fv(texture_points[i][j + 1]);
				glVertex3fv(points[i][j + 1]);

				glNormal3fv(normal_vectors[i + 1][j + 1]);
				glTexCoord2fv(texture_points[i + 1][j + 1]);
				glVertex3fv(points[i + 1][j + 1]);
			glEnd();
		}
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

	const float ROTATION_SPEDD = 200;

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

	drawEgg();

	glFlush();
	glutSwapBuffers();
}

void MyInit(void) {

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	GLbyte* pBytes;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;

	pBytes = LoadTGAImage("textury/D1_t.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);

	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/*************************************************************************************/
	const float offset = 1.0f / N;

	float v = 0.0f;
	float u = 0.0f;

	float x_u, x_v, y_u, y_v, z_u, z_v;

	for (int i = 0; i <= N; ++i) {
		u = 0.0f;

		for (int j = 0; j <= N; ++j) {
			// Generowanie punktow w przestrzeni
			points[i][j][0] =
				(-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(M_PI * v);
			points[i][j][1] =
				(160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2)) - 5;
			points[i][j][2] =
				(-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(M_PI * v);

			// Generowanie punktow w przestrzeni dla tekstury
			texture_points[i][j][0] = u;
			texture_points[i][j][1] = v;

			// Generowanie wektora normalnego do zadanego punktu
			x_u = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * cos(M_PI * v);
			x_v = M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * sin(M_PI * v);
			y_u = 640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u;
			y_v = 0;
			z_u = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * sin(M_PI * v);
			z_v = -M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * cos(M_PI * v);

			normal_vectors[i][j][0] = y_u * z_v - z_u * y_v;
			normal_vectors[i][j][1] = z_u * x_v - x_u * z_v;
			normal_vectors[i][j][2] = x_u * y_v - y_u * x_v;

			if (i == 0 || i == N) {
				normal_vectors[i][j][0] =  0;
				normal_vectors[i][j][1] = -1;
				normal_vectors[i][j][2] =  0;
			}
			else if (i == (N + 1) / 2) {
				normal_vectors[i][j][0] = 0;
				normal_vectors[i][j][1] = 1;
				normal_vectors[i][j][2] = 0;
			}
			else if (i > (N + 1) / 2) {
				normal_vectors[i][j][0] *= -1;
				normal_vectors[i][j][1] *= -1;
				normal_vectors[i][j][2] *= -1;
			}

			// Normalizacja wektora
			float vector_lenght = sqrt(pow(normal_vectors[i][j][0], 2) + pow(normal_vectors[i][j][1], 2) + pow(normal_vectors[i][j][2], 2));
			normal_vectors[i][j][0] /= vector_lenght;
			normal_vectors[i][j][1] /= vector_lenght;
			normal_vectors[i][j][2] /= vector_lenght;

			u += offset;
		}
		v += offset;
	}
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
		glViewport(-7.5 * AspectRatio, 7.5 * AspectRatio, -7.5, 7.5);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char* argv[]) {
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Teksturowanie jajka");

	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	MyInit();
	glEnable(GL_DEPTH_TEST);

	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutMainLoop();
}
