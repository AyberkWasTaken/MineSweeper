/*
	Dev: Burak Ayberk Caglar
	GitHub: AyberkWasTaken
	Start-Date: 11.02.2026
*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "PreDefinedDraw.h"
#include <time.h>
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#define TIMER_PERIOD 1000/30

int SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
int SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);

int gameWidth = 600, gameHeight = 600;

color light = { 1,1,0.8f};
color dark = { 0.15f,0.15f,0.01f };
color systemColor = dark;

bool isDarkMode = true;
bool isLosen = false;
bool isWon = false;
bool firstClick = false;
int counter = 0;
int mineCounter = 0;

themeBtn themeButton = { { 255,255,255 },{ -295,295 },false };

tile tiles[10][10];

void DrawThemeButton() {
	if (themeButton.isHovering) {
		if (!isDarkMode) themeButton.btnColor = { 200,200,200 };
		else themeButton.btnColor = {150, 150, 150};	
	}
	else {
		if (!isDarkMode) themeButton.btnColor = { 255,255,255 };
		else themeButton.btnColor = { 50, 50, 50 };
	}

	drawRect(themeButton.btnPos.x, themeButton.btnPos.y, 110, 40, themeButton.btnColor);

	if(!isDarkMode) glColor3ub(0, 0, 0);
	else glColor3ub(255, 255, 255);
	glBegin(GL_LINE_LOOP);
	glVertex2f(themeButton.btnPos.x, themeButton.btnPos.y);
	glVertex2f(themeButton.btnPos.x + 110, themeButton.btnPos.y);
	glVertex2f(themeButton.btnPos.x + 110, themeButton.btnPos.y - 40);
	glVertex2f(themeButton.btnPos.x, themeButton.btnPos.y - 40);
	glEnd();

	vprint(themeButton.btnPos.x + 26, themeButton.btnPos.y - 27, GLUT_BITMAP_HELVETICA_18, "Theme");
	vprint(-50, 270, GLUT_BITMAP_TIMES_ROMAN_24, "Counter: %d", counter);

	if (isLosen) {
		glColor3ub(255, 0, 0);
		vprint(-55, 245, GLUT_BITMAP_TIMES_ROMAN_24, "YOU LOST");
		vprint(-80, 220, GLUT_BITMAP_TIMES_ROMAN_24, "press R to Restart");
	}
	if (isWon) {
		glColor3ub(0, 255, 0);
		vprint(-52, 245, GLUT_BITMAP_TIMES_ROMAN_24, "YOU WON");
		vprint(-80, 220, GLUT_BITMAP_TIMES_ROMAN_24, "press R to Restart");

	}

}

void displayTiles() {
	float leftCornerX = -200;
	float leftCornerY = 200;
	color tileColor = { 230,230,230 };
	color revealedTileColor = { 150,150,150 };

	for (int i = 0; i < 10; i++) {
		for(int k=0;k<10;k++){
			if(!tiles[i][k].isRevealed) drawRect(leftCornerX + k * 40, leftCornerY - i * 40, 40, 40, tileColor);
			else {
				if (tiles[i][k].isMine) drawRect(leftCornerX + k * 40, leftCornerY - i * 40, 40, 40, { 255,0,0 });
				else drawRect(leftCornerX + k * 40, leftCornerY - i * 40, 40, 40, revealedTileColor);
			}
			drawLine(leftCornerX + k * 40, leftCornerY - i * 40, 40, 40, { 0,0,0 });

			if(tiles[i][k].isHovering && !(tiles[i][k].isRevealed && tiles[i][k].isMine)) {
			
				drawRect(leftCornerX + k * 40 + 3, leftCornerY - i * 40 - 3, 34, 34, { 150,150,150 });	
				vprint(leftCornerX + k * 40 + 7, leftCornerY - i * 40 - 33, GLUT_BITMAP_HELVETICA_18, "%d", tiles[i][k].adjacentMines);
			}
			if (tiles[i][k].adjacentMines > 0  && tiles[i][k].isRevealed && !tiles[i][k].isFlagged) {
				glColor3ub(0, 0, 0);
				vprint(leftCornerX + k * 40 + 15, leftCornerY - i * 40 - 25, GLUT_BITMAP_TIMES_ROMAN_24, "%d", tiles[i][k].adjacentMines);
			}
			if (tiles[i][k].isFlagged) {
				drawFlag(leftCornerX + k * 40 + 8, leftCornerY - i * 40 - 5);
			}
			if (tiles[i][k].isMine && isLosen) {
				drawMine(leftCornerX + k * 40 + 20, leftCornerY - i * 40 - 20);
			}
				
		}
	}
}

void display() {
	glClearColor(systemColor.r,systemColor.g,systemColor.b,1);
	glClear(GL_COLOR_BUFFER_BIT);

	DrawThemeButton();
	displayTiles();

	glColor3ub(180, 180, 180);

	glutSwapBuffers();
}

void onTimer(int v) {
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);


	glutPostRedisplay();
}

void counterTimer(int v) {
	glutTimerFunc(1000, counterTimer, 0);

	if (!isLosen && !isWon && firstClick) counter++;

	glutPostRedisplay();
}

void onResize(int w, int h) {
	if (w != gameWidth || h != gameHeight) {
		glutReshapeWindow(gameWidth, gameHeight);
	}

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-300, 300, -300, 300, -1, 1);
	glMatrixMode(GL_MODELVIEW);
}

void onMouseMove(int x, int y) {
	float mx = x - 300;
	float my = 300 - y;

	if (mx >= themeButton.btnPos.x && mx <= themeButton.btnPos.x + 110 && my <= themeButton.btnPos.y && my >= themeButton.btnPos.y - 40) {
		themeButton.isHovering = true;
	}
	else {
		themeButton.isHovering = false;
	}

	if (mx >= -200 && mx <= 200 && my <= 200 && my >= -200) {
		CheckHovering(mx, my,tiles);
	}
	else {
		if (mx >= -220 && mx <= 220 && my <= 220 && my >= -220) {
			resetHovering(tiles);
		}
	}
}

void onClick(int button, int state, int x, int y) {

	float mx = x - 300;
	float my = 300 - y;
	
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (mx >= themeButton.btnPos.x && mx <= themeButton.btnPos.x + 110 &&
			my <= themeButton.btnPos.y && my >= themeButton.btnPos.y - 40) {

			isDarkMode = !isDarkMode;

			if (isDarkMode) {
				systemColor = dark;
			}
			else {
				systemColor = light;
			}
		}


		if (!firstClick) {
			if (mx >= -200 && mx < 200 && my >= -200 && my < 200) {
				indexPosition temp = MouseClickTile(mx, my);

				do {
					initTiles(tiles);
					placeMines(tiles);
					TileAssertNumbers(tiles);
					
				} while (!starterTiles(tiles,temp));
				recRevealer(tiles, temp);
				firstClick = true;
			}
		}

		else if (!isLosen) {
			if (mx >= -200 && mx < 200 && my >= -200 && my < 200) {
				indexPosition temp = MouseClickTile(mx, my);

				if (!tiles[temp.row][temp.col].isRevealed && !tiles[temp.row][temp.col].isFlagged) {

					if (tiles[temp.row][temp.col].isMine){
						tiles[temp.row][temp.col].isRevealed = true;
						isLosen = isLost(tiles);
					}
					else recRevealer(tiles, temp);
					isWon = isWin(tiles);
				}				
			}
		}

	}
	if (!isLosen) {
		if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {

			indexPosition temp = MouseClickTile(mx, my);
			if(!tiles[temp.row][temp.col].isRevealed) tiles[temp.row][temp.col].isFlagged = !tiles[temp.row][temp.col].isFlagged;

		}
	}

	glutPostRedisplay();
}

void onKeyDown(unsigned char key, int x, int y) {

	if (key == 'r' || key == 'R') {
		isWon = false;
		isLosen = false;
		firstClick = false;
		counter = 0;
		initTiles(tiles);
	}

	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	srand(time(NULL));

	glutInit(&argc, argv);
	glutInitWindowSize(gameWidth, gameHeight);
	glutInitWindowPosition(SCREEN_WIDTH /2 - 300 , SCREEN_HEIGHT / 2 - 300);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("3D World");

	glutPassiveMotionFunc(onMouseMove);
	glutMouseFunc(onClick);

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	glutKeyboardFunc(onKeyDown);

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	glutTimerFunc(1000, counterTimer, 0);

	if (isLosen) return 0;

	glutMainLoop();
	return 0;
}
