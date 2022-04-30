#include <windows.h>
#include <stdio.h>
#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <math.h>
#define PI 3.14159265359
#define P2 (PI/2)
#define P3 (3*PI/2)
#define DEGREE 0.0174532925

using namespace std;

float px, py, pdx, pdy, pa; // player position

void drawPlayer() {
	glColor3f(1,1,0);
	glPointSize(8);
	glBegin(GL_POINTS);
		glVertex2i(px, py);
	glEnd();
	glLineWidth(4);
	glBegin(GL_LINES);
		glVertex2f(px, py);
		glVertex2f(px + pdx * 5, py + pdy * 5);
	glEnd();
}

const int mapX = 16, mapY = 16, mapS = 32;

auto map = vector<int>{
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
	1,0,1,0,0,0,0,0,0,0,0,0,1,0,0,1,
	1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,
	1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,1,1,0,0,0,0,0,0,1,0,0,0,0,1,
	1,0,0,1,0,0,0,1,0,0,1,0,0,0,0,1,
	1,1,0,1,0,0,0,1,0,0,1,1,1,0,1,1,
	1,0,0,0,0,0,0,1,0,0,1,0,0,0,0,1,
	1,0,0,0,0,0,0,1,0,0,1,0,0,0,0,1,
	1,0,0,1,0,0,0,1,0,0,1,0,0,0,0,1,
	1,0,0,1,0,0,0,1,0,0,1,0,0,0,0,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

double dist(int ax, int ay, int bx, int by){
	return sqrt((ax - bx)*(ax - bx) + (ay - by)*(ay - by));
}

void drawMap2D() {
	int x, y, xo, yo;
	for (int y = 0; y < mapY; ++y) {
		for (int x = 0; x < mapX; ++x) {
			if (map[y * mapX + x] == 0) glColor3f(0, 0, 0);
			if (map[y * mapX + x] == 1)	glColor3f(0.7, 0.7, 0.7);
			if (map[y * mapX + x] == 2)	glColor3f(0, 0, 0.7);
			
			xo = x * mapS;
			yo = y * mapS;
			glBegin(GL_QUADS);
				glVertex2f(xo + 1		,yo + 1);
				glVertex2f(xo + mapS - 1,yo + 1);
				glVertex2f(xo + mapS - 1, yo + mapS - 1);
				glVertex2f(xo + 1		, yo + mapS - 1);
			glEnd();
		}
	}
}

void drawBackGround() {
	glColor3f(0, 0, 1);
	glBegin(GL_QUADS);
	glVertex2f(14, 512);
	glVertex2f(494, 512);
	glVertex2f(494, 752);
	glVertex2f(14, 752);
	glEnd();
	glColor3f(0, 1, 0);
	glBegin(GL_QUADS);
	glVertex2f(14, 752);
	glVertex2f(494, 752);
	glVertex2f(494, 1024);
	glVertex2f(14, 1024);
	glEnd();
}

void radianOverflowCheck(float& ra) {
	if (ra < 0) ra += 2 * PI;
	if (ra > 2 * PI) ra -= 2 * PI;
}

void drawRays2D() {
	int r, mx, my, mp, dof=0;
	float rx=0, ry=0, ra, xo, yo;

	ra = pa-(30 * DEGREE);
	radianOverflowCheck(ra);

	for (r = 0; r < 60; ++r) {
		// --水平判定--
		dof = 0;
		float aTan = -1 / tan(ra);
		float disH = 1000000, hx = px, hy = py;
		if (ra > PI) {	// 往上看
			ry = (((int)py >> 5) << 5) - 0.0001;
			rx = (py - ry) * aTan + px;
			yo = -mapS;
			xo = -yo * aTan;
		}
		if (ra < PI) {	// 往下看
			ry = (((int)py >> 5) << 5) + mapS;
			rx = (py - ry) * aTan + px;
			yo = mapS;
			xo = -yo * aTan;
		}
		if (ra == 0 || ra == PI) {	// 水平看
			rx = px;
			ry = py;
			dof = 16;
		}
		while (dof < 16) {
			mx = (int)(rx) >> 5;
			my = (int)(ry) >> 5;
			mp = my * mapX + mx;
			if (mp >= 0 && mp < mapX * mapY && map[mp] > 0) {
				dof = 16;
				hx = rx;
				hy = ry;
				disH = dist(px, py, rx, ry);
			}
			else rx += xo, ry += yo, dof += 1;
		}

		// --垂直判定--
		dof = 0;
		float nTan = -tan(ra);
		float disV = 1000000, vx = px, vy = py;
		if (ra > P2 && ra < P3) {	// 往左看
			rx = (((int)px >> 5) << 5) - 0.0001;
			ry = (px - rx) * nTan + py;
			xo = -mapS;
			yo = -xo * nTan;
			
		}
		if (ra < P2 || ra > P3) {	// 往右看
			rx = (((int)px >> 5) << 5) + mapS;
			ry = (px - rx) * nTan + py;
			xo = mapS;
			yo = -xo * nTan;
		}
		if (ra == 0 || ra == PI) {	// 水平看
			rx = px;
			ry = py;
			dof = 16;
		}
		while (dof < 16) {
			mx = (int)(rx) >> 5;
			my = (int)(ry) >> 5;
			mp = my * mapX + mx;
			if (mp >= 0 && mp < mapX * mapY && map[mp] > 0) {
				dof = 16;
				vx = rx;
				vy = ry;
				disV = dist(px, py, rx, ry);
			}
			else rx += xo, ry += yo, dof += 1;
		}

		// ----- 3D場景-----
		if (disH < disV) glColor3f(0.9, 0, 0);
		else glColor3f(0.7, 0, 0);
		glLineWidth(2);
		glBegin(GL_LINES);
			glVertex2i(px, py);
			if (disH < disV) glVertex2i(hx, hy);
			else glVertex2i(vx, vy);
		glEnd();

		float ca = pa - ra;
		radianOverflowCheck(ca);
		float disT = min(disH, disV) * cos(ca);

		float lineH = (mapS) * 512 / disT;
		if (lineH > 512) lineH = 512;
		float lineO = 240 - lineH / 2;
		if (disH < disV) glColor3f(0.9, 0.9, 0.9);
		else glColor3f(0.7, 0.7, 0.7);
		glLineWidth(8);
		glBegin(GL_LINES);
			glVertex2i(r * 8 + 18, 530 + lineO);
			glVertex2i(r * 8 + 18, 530 + lineO + lineH);
		glEnd();
		// -----------------
		ra += DEGREE;
		radianOverflowCheck(ra);
	}
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawMap2D();
	drawPlayer();
	drawBackGround();
	drawRays2D();
	glutSwapBuffers();
}

void buttons(unsigned char key, int x, int y) {
	if (key == 'a') {
		pa -= 10*DEGREE;
		if (pa < 0) pa += 2 * PI;
		pdx = cos(pa) * 5;
		pdy = sin(pa) * 5;
	}
	if (key == 'd') {
		pa += 10*DEGREE;
		if (pa > 2*PI) pa -= 2 * PI;
		pdx = cos(pa) * 5;
		pdy = sin(pa) * 5;
	}
	if (key == 'w') px += pdx, py += pdy;
	if (key == 's') px -= pdx, py -= pdy;
	glutPostRedisplay();
}

void init() {
	glClearColor(0.3, 0.3, 0.3, 0);
	gluOrtho2D(0, 512, 1024, 0);
	px = 50, py = 50, pdx = cos(pa) * 5, pdy = sin(pa) * 5;
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(512, 1024);
	glutCreateWindow("Raycaster");
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(buttons);
	glutMainLoop();
}