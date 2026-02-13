#define PI 3.14159265358979323846

typedef struct {
	float r, g, b;
}color;

typedef struct {
	float x, y;
}position;

typedef struct {
	int row;
	int col;
}indexPosition;

typedef struct {
	color btnColor;
	position btnPos;
	bool isHovering;
}themeBtn;

typedef struct {
	int tileRow;
	int tileCol;
	bool isFlagged;
	bool isRevealed;
	bool isMine;
	int adjacentMines;
	bool isHovering;
}tile;

void vprint(float x, float y, void* font, const char* string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++) glutBitmapCharacter(font, str[i]);
}

void drawRect(float x, float y, float w, float h, color c) {
	glColor3ub(c.r, c.g, c.b);
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y - h);
	glVertex2f(x, y - h);
	glEnd();
}

void drawLine(float x, float y, float w, float h, color c) {
	glColor3ub(c.r, c.g, c.b);
	glBegin(GL_LINES);
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y - h);
	glVertex2f(x + w, y - h);
	glVertex2f(x, y - h);
	glVertex2f(x, y - h);
	glVertex2f(x, y);
	glEnd();
}

void drawCircle(float x, float y, float r) {
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++) {
		angle = 2 * i * PI / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void drawFlag(float x, float y) {

	glColor3ub(255,0,0);
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + 5, y);
	glVertex2f(x + 5, y - 30);
	glVertex2f(x, y - 30);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(x + 5, y - 3);
	glVertex2f(x + 16, y - 3);
	glVertex2f(x + 25, y - 18);
	glVertex2f(x + 5, y - 18);
	glEnd();
}

void drawMine(float x, float y) {
	glColor3ub(0, 0, 0);
	drawCircle(x, y, 12);
}

int getRandom(int min, int max) {
	return (rand() % (max - min + 1)) + min;
}

void initTiles(tile tiles[][10]) {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			tiles[i][j].tileRow = i;
			tiles[i][j].tileCol = j;
			tiles[i][j].isFlagged = false;
			tiles[i][j].isRevealed = false;
			tiles[i][j].isMine = false;
			tiles[i][j].adjacentMines = 0;
		}
	}
}

void placeMines(tile tiles[][10]) {
	int minesPlaced = 0;
	while (minesPlaced < 15) {
		int row = getRandom(0, 9);
		int col = getRandom(0, 9);

		if (!tiles[row][col].isMine) {
			tiles[row][col].isMine = true;
			minesPlaced++;
		}
	}
}

void TileAssertNumbers(tile tiles[][10]) {

	for (int i=0;i<10;i++){
		for (int k = 0; k < 10; k++) {

			if (!tiles[i][k].isMine) {
				int count = 0;
				for(int row = i - 1;row <= i+1; row++){
					for (int col = k - 1; col <= k + 1; col++) {
						if (row >= 0 && row < 10 && col >= 0 && col < 10) {
							if (tiles[row][col].isMine == true) count++;
						}
					}		
				
				}
				tiles[i][k].adjacentMines = count;
			}

		}	
	}
}

void CheckHovering(float x, float y, tile tiles[10][10]) {
	int row = (int)(-y + 200) / 40;
	int col = (int)(200 + x) / 40;

	for (int i = 0; i < 10; i++) {
		for (int k = 0; k < 10; k++) {
			tiles[i][k].isHovering = (i == row && k == col);
		}
	}
}

void resetHovering(tile tiles[10][10]) {
	for (int i = 0; i < 10; i++) {
		for (int k = 0; k < 10; k++) {
			tiles[i][k].isHovering = false;
		}
	}
}

bool isLost(tile tiles[][10]) {
	for (int i = 0; i < 10; i++) {
		for (int k = 0; k < 10; k++) {
			if(tiles[i][k].isRevealed && tiles[i][k].isMine) return true;
		}
	}
	return false;
}

bool isWin(tile tiles[][10]) {
	int minecount = 0;
	for (int i = 0; i < 10; i++) {
		for (int k = 0; k < 10; k++) {
			if (!tiles[i][k].isRevealed) minecount++;
		}
	}
	if (minecount == 15) return true;
	return false;
}

indexPosition MouseClickTile(int x, int y) {
	indexPosition temp;

	temp.col = (int)(x + 200) /40;
	temp.row = (int)(200 - y) /40;
	
	return temp;
}

bool starterTiles(tile tiles[][10],indexPosition pos) {

	if (tiles[pos.row][pos.col].isMine) return false;
	for (int i = pos.row - 1; i <= pos.row + 1; i++) {
		for (int k = pos.col - 1; k <= pos.col + 1; k++) {
			if (i >= 0 && i < 10 && k >= 0 && k < 10) {
				if (tiles[i][k].isMine == true) return false;
			}
		}

	}
	return true;
}

void recRevealer(tile tiles[10][10], indexPosition pos) {

	if (pos.row < 0 || pos.row >= 10 || pos.col < 0 || pos.col >= 10) return;

	if (tiles[pos.row][pos.col].isRevealed || tiles[pos.row][pos.col].isFlagged) return;

	tiles[pos.row][pos.col].isRevealed = true;

	if (tiles[pos.row][pos.col].adjacentMines > 0) return;

	for (int i = pos.row - 1; i <= pos.row + 1; i++) {
		for (int k = pos.col - 1; k <= pos.col + 1; k++) {
			indexPosition nextPos;
			nextPos.row = i;
			nextPos.col = k;
			recRevealer(tiles, nextPos);
		}
	}
}

