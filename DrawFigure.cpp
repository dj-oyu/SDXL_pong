#include "DxLib.h"

typedef struct {
	int x, y, r;
	int c;
	int vx, vy;
} Ball;

typedef struct {
	int x, y, w, h;
} Boundary;

Ball* spawnBall(int x, int y, int r, int c, int vx, int vy) {
	Ball* ball = (Ball*) malloc(sizeof Ball);
	if (ball == NULL) return NULL;

	ball->x = x;
	ball->y = y;
	ball->r = r;
	ball->c = c;
	ball->vx = vx;
	ball->vy = vy;
	return ball;
}

Boundary* spawnBoundary(int x, int y, int w, int h) {
	Boundary* boundary = (Boundary*)malloc(sizeof Boundary);
	if (boundary == NULL) return NULL;

	boundary->x = x;
	boundary->y = y;
	boundary->w = w;
	boundary->h = h;
	return boundary;
}

int drawRect(Boundary* boundary, int color) {
	int result = 0, stroke = 8;
	// left wall
	result |= DrawBox(boundary->x - stroke, boundary->y - stroke, boundary->x, boundary->y + boundary->h, color, TRUE);
	// bottom wall
	result |= DrawBox(boundary->x - stroke, boundary->y + boundary->h, boundary->x + boundary->w, boundary->y + boundary->h + stroke, color, TRUE);
	// right wall
	result |= DrawBox(boundary->x + boundary->w, boundary->y, boundary->x + boundary->w + stroke, boundary->y + boundary->h + stroke, color, TRUE);
	// top wall
	result |= DrawBox(boundary->x, boundary->y - stroke, boundary->x + boundary->w + stroke, boundary->y, color, TRUE);
	return result;
}

int drawBall(Ball* ball) {
	return DrawCircle(ball->x, ball->y, ball->r, ball->c, TRUE);
}

int moveBall(Ball* ball) {
	ball->x += ball->vx;
	ball->y += ball->vy;
	return 0;
}

int isHitBoundary(Ball* ball, Boundary* boundary) {
	int hit = 0;
	if (ball->vx < 0 && ball->x - ball->r < boundary->x + ball->vx / 2) {
		ball->vx = -ball->vx;
		hit |= 1;
	}
	else if (ball->vx > 0 && ball->x + ball->r > boundary->x + boundary->w - ball->vx / 2) {
		ball->vx = -ball->vx;
		hit |= 2;
	}
	if (ball->vy < 0 && ball->y - ball->r < boundary->y + ball->vy / 2) {
		ball->vy = -ball->vy;
		hit |= 4;
	}
	else if (ball->vy > 0 && ball->y + ball->r > boundary->y + boundary->h - ball->vy / 2) {
		ball->vy = -ball->vy;
		hit |= 8;
	}
	return hit;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	const int WIDTH = 960, HEIGHT = 640;
	enum { BLACK, WHITE, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, GRAY };
	int color[9] = {
		GetColor(0, 0, 0),
		GetColor(255, 255, 255),
		GetColor(255, 0, 0),
		GetColor(0, 255, 0),
		GetColor(0, 0, 255),
		GetColor(255, 255, 0),
		GetColor(0, 255, 255),
		GetColor(255, 0, 255),
		GetColor(128, 128, 128)
	};

	SetWindowText("Pong");
	SetGraphMode(WIDTH, HEIGHT, 32);
	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1) return -1;
	SetBackgroundColor(0, 0, 0);
	SetDrawScreen(DX_SCREEN_BACK);

	Ball* ball = spawnBall(WIDTH / 2, HEIGHT / 2, 6, color[GREEN], 5, 10);
	drawBall(ball);

	Boundary* boundary = spawnBoundary(WIDTH / 2 - 100, HEIGHT / 2 - 100, 200, 200);
	int hitFlag = 0;

	while (1) {
		ClearDrawScreen();
		hitFlag = 0;

		drawRect(boundary, color[RED]);
		moveBall(ball);
		drawBall(ball);

		if ((hitFlag = isHitBoundary(ball, boundary)) != 0) {
			int stroke = 8;
			if (hitFlag & 1) DrawBox(boundary->x - stroke, boundary->y, boundary->x, boundary->y + boundary->h, color[BLUE], TRUE);
			if (hitFlag & 2) DrawBox(boundary->x + boundary->w, boundary->y, boundary->x + boundary->w + stroke, boundary->y + boundary->h, color[BLUE], TRUE);
			if (hitFlag & 4) DrawBox(boundary->x, boundary->y - stroke, boundary->x + boundary->w, boundary->y, color[BLUE], TRUE);
			if (hitFlag & 8) DrawBox(boundary->x, boundary->y + boundary->h, boundary->x + boundary->w, boundary->y + boundary->h + stroke, color[BLUE], TRUE);
			ScreenFlip();
			WaitTimer(33 * 6);
		}
		else {
			ScreenFlip();
			WaitTimer(16);
		}

		if (ProcessMessage() != 0) break;
		if (CheckHitKey(KEY_INPUT_ESCAPE)) break;
	}

	DxLib_End();

	return 0;
}