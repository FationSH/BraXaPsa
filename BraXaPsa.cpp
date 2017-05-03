// compile with g++ BraXaPsa.cpp -lglut -lGL -lGLU

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

static int g_Width = 616; // initial window height 15*40 (+15 columns)
static int g_Height = 574; // initial window width 12*40+80 (+12 lines)
int score = 0; // player total score
int totalMoves; // player total moves
int moves; // player moves left
int round = 0; // number of games player played
int calledby = 0; // flag to see if a call was made by renderObjects or disp function

int matrix[15][12]; // block type matrix
int ptCtr = 0; // need to have 2 states when clicking to swap
int bpressed = 0; // check when player pressed b

// paper rock and scissors .pgm file values are store here
unsigned char* paper;
unsigned char* rock;
unsigned char* scissors;

string msg; // message string containing display message in comment section of the window

// this class stores x and y position of the window the player clicked
class scrPt {
public:
	GLint x, y;
};

void init();
void processNormalKeys(unsigned char key, int x, int y);
void display();
void renderObjects();
void mouseButton(GLint button, GLint action, GLint xMouse, GLint yMouse);
void swapBlocks(scrPt endPt1, scrPt endPt2);
unsigned char* loadPgm(const char *filename);
void updateMatrix();
void triples();
void checkdist(int x, int y, int distMatrixtype);

/*
	This function takes (x, y) position as arguments and creates submatrix of distances of main matrix.
*/
void checkdist(int x, int y, int distMatrixtype) {

	int i, j, k, temp, dist, scorecounter, randnum;
	int flag = 0;

	if (distMatrixtype == 0) { // use horizontal distance matrix
		// init distance matrix
		int distancesMatrix[7][9] = {
			{matrix[x-3][y-3], matrix[x-2][y-3], matrix[x-1][y-3], matrix[x][y-3], matrix[x+1][y-3], matrix[x+2][y-3], matrix[x+3][y-3], matrix[x+4][y-3], matrix[x+5][y-3]},
			{matrix[x-3][y-2], matrix[x-2][y-2], matrix[x-1][y-2], matrix[x][y-2], matrix[x+1][y-2], matrix[x+2][y-2], matrix[x+3][y-2], matrix[x+4][y-2], matrix[x+5][y-2]},
			{matrix[x-3][y-1], matrix[x-2][y-1], matrix[x-1][y-1], matrix[x][y-1], matrix[x+1][y-1], matrix[x+2][y-1], matrix[x+3][y-1], matrix[x+4][y-1], matrix[x+5][y-1]},
			{matrix[x-3][y], matrix[x-2][y], matrix[x-1][y], matrix[x][y], matrix[x+1][y], matrix[x+2][y], matrix[x+3][y], matrix[x+4][y], matrix[x+5][y]},
			{matrix[x-3][y+1], matrix[x-2][y+1], matrix[x-1][y+1], matrix[x][y+1], matrix[x+1][y+1], matrix[x+2][y+1], matrix[x+3][y+1], matrix[x+4][y+1], matrix[x+5][y+1]},
			{matrix[x-3][y+2], matrix[x-2][y+2], matrix[x-1][y+2], matrix[x][y+2], matrix[x+1][y+2], matrix[x+2][y+2], matrix[x+3][y+2], matrix[x+4][y+2], matrix[x+5][y+2]},
			{matrix[x-3][y+3], matrix[x-2][y+3], matrix[x-1][y+3], matrix[x][y+3], matrix[x+1][y+3], matrix[x+2][y+3], matrix[x+3][y+3], matrix[x+4][y+3], matrix[x+5][y+3]}
		};
		for (i = 0; i < 7; i++) {
			for (j = 0; j < 9; j++) {
				flag = 0;
				if ((i != x-1 && j != y) || (i != x-2 && j !=y) || (i != x-3 && j != y)) { // don't include the triple
					if (i >= -1 && i <= 3 && j >=-1 && j<= 1) { // if we are in distance 1
						if (distancesMatrix[x][y] == 0) { // if paper then eat rock and also paper scissors/blue/reds
							if (distancesMatrix[i][j] == 1 || distancesMatrix[i][j] == 3 || distancesMatrix[i][j] == 4) {
								// drop all blocks 1 position lower
								for (k = j; k >= 0; k--) {
									temp = matrix[i][k];
									matrix[i][k] = matrix[i][k+1];
									matrix[i][k+1] = temp;
									updateMatrix();
								}
								// then first block needs a new random value
								randnum = rand() % 5;
								matrix[i][0] = randnum;
								updateMatrix();
								score += 3;
								flag = 1;
								score += 2;
							}
						} else if (distancesMatrix[x][y] == 1) { // if rock then eat scissors and also destroy rock/blue/reds
							if (distancesMatrix[i][j] == 2 || distancesMatrix[i][j] == 3 || distancesMatrix[i][j] == 4) {
								// drop all blocks 1 position lower
								for (k = j; k >= 0; k--) {
									temp = matrix[i][k];
									matrix[i][k] = matrix[i][k+1];
									matrix[i][k+1] = temp;
									updateMatrix();
								}
								// then first block needs a new random value
								randnum = rand() % 5;
								matrix[i][0] = randnum;
								updateMatrix();
								flag = 1;
								score += 2;
							}
						} else if (distancesMatrix[x][y] == 2) { // if scissors then eat paper and also destroy scissors/blue/reds
							if (distancesMatrix[i][j] == 0 || distancesMatrix[i][j] == 3 || distancesMatrix[i][j] == 4) {
								// drop all blocks 1 position lower
								for (k = j; k >= 0; k--) {
									temp = matrix[i][k];
									matrix[i][k] = matrix[i][k+1];
									matrix[i][k+1] = temp;
									updateMatrix();
								}
								// then first block needs a new random value
								randnum = rand() % 5;
								matrix[i][0] = randnum;
								updateMatrix();
								flag = 1;
								score += 2;
							}
						}
						if (flag == 0) { // if no distance 1 block was deleted
							score -= 1;
						}
					} else { // else if we are in distance 2 or 3
						if (distancesMatrix[x][y] == 0) { // if paper then eat rock
							if (distancesMatrix[i][j] == 1) {
								// drop all blocks 1 position lower
								for (k = j; k >= 0; k--) {
									temp = matrix[i][k];
									matrix[i][k] = matrix[i][k+1];
									matrix[i][k+1] = temp;
									updateMatrix();
								}
								// then first block needs a new random value
								randnum = rand() % 5;
								matrix[i][0] = randnum;
								updateMatrix();
								score += 3;
							}
						} else if (distancesMatrix[x][y] == 1) { // if rock then eat scissors
							if (distancesMatrix[i][j] == 2) {
								// drop all blocks 1 position lower
								for (k = j; k >= 0; k--) {
									temp = matrix[i][k];
									matrix[i][k] = matrix[i][k+1];
									matrix[i][k+1] = temp;
									updateMatrix();
								}
								// then first block needs a new random value
								randnum = rand() % 5;
								matrix[i][0] = randnum;
								updateMatrix();
								score += 3;
							}
						} else if (distancesMatrix[x][y] == 2) { // if scissors then eat paper
							if (distancesMatrix[i][j] == 0) {
								// drop all blocks 1 position lower
								for (k = j; k >= 0; k--) {
									temp = matrix[i][k];
									matrix[i][k] = matrix[i][k+1];
									matrix[i][k+1] = temp;
									updateMatrix();
								}
								// then first block needs a new random value
								randnum = rand() % 5;
								matrix[i][0] = randnum;
								updateMatrix();
								score += 3;
							}
						}
					}
				}
			}
		}
	} else { // use vertical distance matrix
		// init distance matrix
		int distancesMatrix[9][7] = {
			{matrix[x-3][y-3], matrix[x-2][y-3], matrix[x-1][y-3], matrix[x][y-3], matrix[x+1][y-3], matrix[x+2][y-3], matrix[x+3][y-3]},
			{matrix[x-3][y-2], matrix[x-2][y-2], matrix[x-1][y-2], matrix[x][y-2], matrix[x+1][y-2], matrix[x+2][y-2], matrix[x+3][y-2]},
			{matrix[x-3][y-1], matrix[x-2][y-1], matrix[x-1][y-1], matrix[x][y-1], matrix[x+1][y-1], matrix[x+2][y-1], matrix[x+3][y-1]},
			{matrix[x-3][y], matrix[x-2][y], matrix[x-1][y], matrix[x][y], matrix[x+1][y], matrix[x+2][y], matrix[x+3][y]},
			{matrix[x-3][y+1], matrix[x-2][y+1], matrix[x-1][y+1], matrix[x][y+1], matrix[x+1][y+1], matrix[x+2][y+1], matrix[x+3][y+1]},
			{matrix[x-3][y+2], matrix[x-2][y+2], matrix[x-1][y+2], matrix[x][y+2], matrix[x+1][y+2], matrix[x+2][y+2], matrix[x+3][y+2]},
			{matrix[x-3][y+3], matrix[x-2][y+3], matrix[x-1][y+3], matrix[x][y+3], matrix[x+1][y+3], matrix[x+2][y+3], matrix[x+3][y+3]},
			{matrix[x-3][y+4], matrix[x-2][y+4], matrix[x-1][y+4], matrix[x][y+4], matrix[x+1][y+4], matrix[x+2][y+4], matrix[x+3][y+4]},
			{matrix[x-3][y+5], matrix[x-2][y+5], matrix[x-1][y+5], matrix[x][y+5], matrix[x+1][y+5], matrix[x+2][y+5], matrix[x+3][y+5]}
		};
		for (j = 0; j < 9; j++) {
			for (i = 0; i < 7; i++) {
				flag = 0;
				if ((i != y-1 && j != x) || (i != y-2 && j != x) || (i != y-3 && j != x)) { // don't include the triple
					if (i >= -1 && i <= 1 && j >=-1 && j<= 3) { // if we are in distance 1
						if (distancesMatrix[x][y] == 0) { // if paper then eat rock and also paper scissors/blue/reds
							if (distancesMatrix[i][j] == 1 || distancesMatrix[i][j] == 3 || distancesMatrix[i][j] == 4) {
								// drop all blocks 1 position lower
								for (k = j; k >= 0; k--) {
									temp = matrix[i][k];
									matrix[i][k] = matrix[i][k+1];
									matrix[i][k+1] = temp;
									updateMatrix();
								}
								// then first block needs a new random value
								randnum = rand() % 5;
								matrix[i][0] = randnum;
								updateMatrix();
								score += 3;
								flag = 1;
								score += 2;
							}
						} else if (distancesMatrix[x][y] == 1) { // if rock then eat scissors and also destroy rock/blue/reds
							if (distancesMatrix[i][j] == 2 || distancesMatrix[i][j] == 3 || distancesMatrix[i][j] == 4) {
								// drop all blocks 1 position lower
								for (k = j; k >= 0; k--) {
									temp = matrix[i][k];
									matrix[i][k] = matrix[i][k+1];
									matrix[i][k+1] = temp;
									updateMatrix();
								}
								// then first block needs a new random value
								randnum = rand() % 5;
								matrix[i][0] = randnum;
								updateMatrix();
								flag = 1;
								score += 2;
							}
						} else if (distancesMatrix[x][y] == 2) { // if scissors then eat paper and also destroy scissors/blue/reds
							if (distancesMatrix[i][j] == 0 || distancesMatrix[i][j] == 3 || distancesMatrix[i][j] == 4) {
								// drop all blocks 1 position lower
								for (k = j; k >= 0; k--) {
									temp = matrix[i][k];
									matrix[i][k] = matrix[i][k+1];
									matrix[i][k+1] = temp;
									updateMatrix();
								}
								// then first block needs a new random value
								randnum = rand() % 5;
								matrix[i][0] = randnum;
								updateMatrix();
								flag = 1;
								score += 2;
							}
						}
						if (flag == 0) { // if no distance 1 block was deleted
							score -= 1;
						}
					} else { // else if we are in distance 2 or 3
						if (distancesMatrix[x][y] == 0) { // if paper then eat rock
							if (distancesMatrix[i][j] == 1) {
								// drop all blocks 1 position lower
								for (k = j; k >= 0; k--) {
									temp = matrix[i][k];
									matrix[i][k] = matrix[i][k+1];
									matrix[i][k+1] = temp;
									updateMatrix();
								}
								// then first block needs a new random value
								randnum = rand() % 5;
								matrix[i][0] = randnum;
								updateMatrix();
								score += 3;
							}
						} else if (distancesMatrix[x][y] == 1) { // if rock then eat scissors
							if (distancesMatrix[i][j] == 2) {
								// drop all blocks 1 position lower
								for (k = j; k >= 0; k--) {
									temp = matrix[i][k];
									matrix[i][k] = matrix[i][k+1];
									matrix[i][k+1] = temp;
									updateMatrix();
								}
								// then first block needs a new random value
								randnum = rand() % 5;
								matrix[i][0] = randnum;
								updateMatrix();
								score += 3;
							}
						} else if (distancesMatrix[x][y] == 2) { // if scissors then eat paper
							if (distancesMatrix[i][j] == 0) {
								// drop all blocks 1 position lower
								for (k = j; k >= 0; k--) {
									temp = matrix[i][k];
									matrix[i][k] = matrix[i][k+1];
									matrix[i][k+1] = temp;
									updateMatrix();
								}
								// then first block needs a new random value
								randnum = rand() % 5;
								matrix[i][0] = randnum;
								updateMatrix();
								score += 3;
							}
						}
					}
				}
			}
		}
	}
}

/*
	This function checks if 3 same block types have been created and calculates
	score accordingly.
*/
void triples() {

	int i, j, drop, drop2, temp, randnum;

	if (bpressed == 0) { // game didn't start yet
		return;
	}

	updateMatrix();
	for (i = 0; i < 15; i++) {
		//updateMatrix();
		for (j = 0; j < 12; j++) {
			if (j+2 < 12 && j+1 < 12 && matrix[i][j] == matrix[i][j+1] && matrix[i][j] == matrix[i][j+2]) { // vertical triple
				if (calledby == 0) { // if explosion made when rendering
					score += 2;
					calledby = 1;
				} else {
					score += 10;
				}
				if (j > 2) {
					checkdist(i, j, 1);
					matrix[i][j] = matrix[i][j-3];
					updateMatrix();
					matrix[i][j+1] = matrix[i][j-2];
					updateMatrix();
					matrix[i][j+2] = matrix[i][j-1];
					updateMatrix();
					for (drop = j; drop >= 0; drop--) {
						temp = matrix[i][drop];
						matrix[i][drop] = matrix[i][drop+1];
						updateMatrix();
						matrix[i][drop+1] = temp;
						updateMatrix();
					}
				} else if (j == 2) {
					checkdist(i, j, 1);
					matrix[i][j] = randnum;
					updateMatrix();
					matrix[i][j+1] = matrix[i][j-2];
					updateMatrix();
					matrix[i][j+2] = matrix[i][j-1];
					updateMatrix();
				} else if (j == 1) {
					checkdist(i, j, 1);
					randnum = rand() % 5;
					matrix[i][j] = randnum;
					updateMatrix();
					randnum = rand() % 5;
					matrix[i][j+1] = randnum;
					updateMatrix();
					matrix[i][j+2] = matrix[i][j-1];
					updateMatrix();
				} else {
					checkdist(i, j, 1);
					randnum = rand() % 5;
					matrix[i][j] = randnum;
					updateMatrix();
					randnum = rand() % 5;
					matrix[i][j+1] = randnum;
					updateMatrix();
					randnum = rand() % 5;
					matrix[i][j+2] = randnum;
					updateMatrix();
				}
			} else if (i+1 < 15 && i+2 < 15 && matrix[i][j] == matrix[i+1][j] && matrix[i][j] == matrix[i+2][j]) { // horizontal triple
				if (calledby == 0) { // if explosion made when rendering
					score += 2;
					calledby = 1;
				} else {
					score += 10;
				}
				if (i > 2) {
					checkdist(i, j, 0);
					matrix[i][j] = matrix[i][j+1];
					updateMatrix();
					matrix[i+1][j] = matrix[i+1][j+1];
					updateMatrix();
					matrix[i+2][j] = matrix[i+2][j+1];
					updateMatrix();
					for (drop = i; drop >= 0; drop--) {
						for (drop2 = j; drop >= 0; drop--) {
							temp = matrix[drop][drop2];
							matrix[drop][drop2] = matrix[drop][drop2+1];
							updateMatrix();
							matrix[drop][drop2+1] = temp;
							updateMatrix();
						}
					}
				} else if (i == 2) {
					checkdist(i, j, 0);
					randnum = rand() % 5;
					matrix[i][j] = randnum;
					updateMatrix();
					matrix[i+1][j] = matrix[i+1][j+1];
					updateMatrix();
					matrix[i+2][j] = matrix[i+2][j+1];
					updateMatrix();
				} else if (i == 1) {
					checkdist(i, j, 0);
					randnum = rand() % 5;
					matrix[i][j] = randnum;
					updateMatrix();
					randnum = rand() % 5;
					matrix[i+1][j] = randnum;
					updateMatrix();
					matrix[i+2][j] = matrix[i+2][j+1];
					updateMatrix();
					checkdist(i, j, 0);
				} else {
					checkdist(i, j, 0);
					randnum = rand() % 5;
					matrix[i][j] = randnum;
					updateMatrix();
					randnum = rand() % 5;
					matrix[i+1][j] = randnum;
					updateMatrix();
					randnum = rand() % 5;
					matrix[i+2][j] = randnum;
					updateMatrix();
				}
			}
		}
	}
}

/*
	This function is called when we change the block type matrix to update values in the grid.
*/
void updateMatrix() {

	int i, j, x, y;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	for (i = 0; i < 15; i++) {
		x = i*40+1+i;
		for (j = 0; j < 12; j++) {
			y = 534-j*40-j;
			if (matrix[i][j] == 0) { // paper
				glRasterPos2d(x, y);
				glDrawPixels(40, 40, GL_LUMINANCE, GL_UNSIGNED_BYTE, paper);
			} else if (matrix[i][j] == 1) { // rock
				glRasterPos2d(x, y);
				glDrawPixels(40, 40, GL_LUMINANCE, GL_UNSIGNED_BYTE, rock);
			} else if (matrix[i][j] == 2) { // scissors
				glRasterPos2d(x, y);
				glDrawPixels(40, 40, GL_LUMINANCE, GL_UNSIGNED_BYTE, scissors);
			} else if (matrix[i][j] == 3) { // blue
				glBegin(GL_QUADS);
				glColor3f(0.0, 0.0, 1.0);
				glVertex2f(x, y);
				glVertex2f(x+40, y);
				glVertex2f(x+40, y+40);
				glVertex2f(x, y+40);
				glEnd();
			} else { // red
				glBegin(GL_QUADS);
				glColor3f(1.0, 0.0, 0.0);
				glVertex2f(x, y);
				glVertex2f(x+40, y);
				glVertex2f(x+40, y+40);
				glVertex2f(x, y+40);
				glEnd();
			}
		}
	}
	glColor3f(1.0, 1.0, 1.0);

	glPopMatrix();
	glFlush();
}

/*
	This function takes .pgm filename as argument and stores its values in a structure.
*/
unsigned char* loadPgm(const char *filename) {

	ifstream textureFile;
	int textureWidth, textureHeight, maxGrayScaleValue, dataSize, value, index;
	string inputLine;
	unsigned char* texture;

	textureFile.open(filename, ios::in);
	inputLine = "";

	getline(textureFile, inputLine); // first line P2
	getline(textureFile, inputLine); // second line # comment
	textureFile >> textureWidth; // read width
	textureFile >> textureHeight; // read height
	textureFile >> maxGrayScaleValue; // read maximum gray value (255 in this case)

	dataSize = textureWidth*textureHeight;
	texture = new unsigned char[dataSize];
	index = 0;

	while(index < dataSize) {
        textureFile >> value;
        texture[index++] = (unsigned char) value;
        if(textureFile.eof()) // make sure it is reached EOF
            break;
    }
	textureFile.close();

	return texture;
}

/*
	This function takes 2 (x, y) points and makes them swap only if they are next to each other.
*/
void swapBlocks(scrPt endPt1, scrPt endPt2) {

	int i, j, temp, x1, y1, x2, y2;

	x1 = endPt1.x/41;
	y1 = 14-(endPt1.y/41)-1;

	x2 = endPt2.x/41;
	y2 = 14-(endPt2.y/41)-1;

	// if player clicked in comment section do nothing
	if (y1 == 12 || y1 == 13 || y2 == 12 || y2 == 13) {
		return;
	}
	// if x1,y1 is next to x2,y2
	if ((x1 == x2 && y2 == y1+1) || (x1 == x2 && y2 == y1-1) || (x2 == x1+1 && y1 == y2) || (x2 == x1-1 && y1 == y2)) {
		temp = matrix[x1][y1];
		matrix[x1][y1] = matrix[x2][y2];
		matrix[x2][y2] = temp;
		updateMatrix();
		calledby = 1;
		triples();
		moves--;
	} else {
		return;
	}
}

/*
	This function calculates 2 points (x, y) the user clicked.
*/
void mouseButton(GLint button, GLint action, GLint xMouse, GLint yMouse) {

	static scrPt endPt1, endPt2;

	if (ptCtr == 0) {
		if (button == GLUT_LEFT_BUTTON && action == GLUT_DOWN) {
			endPt1.x = xMouse;
			endPt1.y = g_Height-yMouse;
			ptCtr = 1;
		}
	} else {
		if (button == GLUT_LEFT_BUTTON && action == GLUT_DOWN) {
			endPt2.x = xMouse;
			endPt2.y = g_Height-yMouse;
			swapBlocks(endPt1, endPt2);
			ptCtr = 0;
		}
	}
	glFlush();
}

/*
	This function renders the scene using random number generator.
*/
void renderObjects() {

	int i, j, x, y, block_type;

	paper = loadPgm("images/paper.pgm");
	rock = loadPgm("images/rock.pgm");
	scissors = loadPgm("images/scissors.pgm");

	srand(time(NULL)); // seed random number generator

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	for (i = 0; i < 15; i++) {
		x = i*40+1+i;
		for (j = 0; j < 12; j++) {
			y = 534-j*40-j;
			block_type = rand() % 5;
			if (block_type == 0) { // paper
				glRasterPos2d(x, y);
				glDrawPixels(40, 40, GL_LUMINANCE, GL_UNSIGNED_BYTE, paper);
				matrix[i][j] = 0;
			} else if (block_type == 1) { // rock
				glRasterPos2d(x, y);
				glDrawPixels(40, 40, GL_LUMINANCE, GL_UNSIGNED_BYTE, rock);
				matrix[i][j] = 1;
			} else if (block_type == 2) { // scissors
				glRasterPos2d(x, y);
				glDrawPixels(40, 40, GL_LUMINANCE, GL_UNSIGNED_BYTE, scissors);
				matrix[i][j] = 2;
			} else if (block_type == 3) { // blue
				glBegin(GL_QUADS);
				glColor3f(0.0, 0.0, 1.0);
				glVertex2f(x, y);
				glVertex2f(x+40, y);
				glVertex2f(x+40, y+40);
				glVertex2f(x, y+40);
				glEnd();
				matrix[i][j] = 3;
			} else { // red
				glBegin(GL_QUADS);
				glColor3f(1.0, 0.0, 0.0);
				glVertex2f(x, y);
				glVertex2f(x+40, y);
				glVertex2f(x+40, y+40);
				glVertex2f(x, y+40);
				glEnd();
				matrix[i][j] = 4;
			}
		}
	}
	glColor3f(1.0, 1.0, 1.0);

	glPopMatrix();
	glFlush();
}

/*
	This function handles display of the grid and the messages and checks for triples every loop.
*/
void display() {

	char buffer[64];
	int i, f;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear frame buffer and depth buffer

	glLineWidth(1.0);
	glBegin(GL_LINES);
	for (f = 81; f <= 616; f += 41) {
		glVertex2f(0, f);
		glVertex2f(616, f);
	}
	for (f = 1; f <= 575; f += 41) {
		glVertex2f(f, 82);
		glVertex2f(f, 574);
	}
	glVertex2f(616, 82);
	glVertex2f(616, 574);
	glEnd();

	// messages handling
	glColor3f(0.0, 1.0, 0.0);
	if (bpressed == 0) { // game did not start yet
		if (round == 0) {
			sprintf(buffer, "Press b to start or esc to exit    Moves: %d", moves);
			msg = buffer;
			glRasterPos2f(10, 60);
			for (i = 0; i < msg.length(); i++) {
				glutBitmapCharacter(GLUT_BITMAP_9_BY_15, msg[i]);
			}
			moves = totalMoves; // make sure player can't move when scene not rendered
		} else {
			sprintf(buffer, "Game Over! Press b to restart or esc to exit    Total score: %d", score);
			msg = buffer;
			glRasterPos2f(10, 30);
			for (i = 0; i < msg.length(); i++) {
				glutBitmapCharacter(GLUT_BITMAP_9_BY_15, msg[i]);
			}
			moves = totalMoves;
		}
	}
	if (bpressed == 1) { // game in progress
		if (moves <= 0) {
			bpressed = 0;
		} else {
			sprintf(buffer, "Moves: %d    Score: %d", moves, score);
			msg = buffer;
			glRasterPos2f(10, 60);
			for (i = 0; i < msg.length(); i++) {
				glutBitmapCharacter(GLUT_BITMAP_9_BY_15, msg[i]);
			}
		}
	}
	glColor3f(1.0, 1.0, 1.0);

	triples();

	glutPostRedisplay();
	glFlush();
}

/*
	This function handles keys, esc for exit and b to start/restart game.
*/
void processNormalKeys(unsigned char key, int x, int y) {

	switch (key) {
		case 27: // esc key
			exit(0);
			break;
		case 'b': // b key
		case 'B':
			if (bpressed == 0) {
				calledby = 0 ;
				score = 0;
				round++;
				renderObjects();
				triples();
				bpressed = 1;
			}
			break;
	}
}

/*
	This function is our own init() function, creates ortho2D viewport with window width and height.
*/
void init() {
	glClearColor(0, 0, 0, 0); // define our background color
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, g_Width, 0, g_Height); // define our viewing area
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/*
	This function is the main function.
*/
int main(int argc, char** argv) {

	cout << "Give number of moves:" << endl;
	cin >> moves;

	totalMoves = moves; // hold total moves in case player wants to restart a game

	// init glut and create window
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB); // RGB display mode
	glutInitWindowSize(g_Width, g_Height); // window width and height
	glutCreateWindow("BraXaPsa II"); // window name
	init(); // init clears color and sets ortho2D viewing area
	glutDisplayFunc(display); // set the display function
	glutKeyboardFunc(processNormalKeys); // set the normal keyboard function
	glutMouseFunc(mouseButton); // set the mouse button function
	glutMainLoop(); // enter glut event processing cycle

	return 0;
}
