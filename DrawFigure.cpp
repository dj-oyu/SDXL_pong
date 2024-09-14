#include "DxLib.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>

typedef struct {
	int c_x, c_y, w, h;
	int color;
} Racket;

typedef struct {
	int x, y, r;
	int c;
	double vx, vy;
} Ball;

typedef struct {
	int x, y, w, h;
} Boundary;

const int stroke = 8;
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

// Scenes
enum { TITLE, GAME, GAME_OVER, RESULT };
int Scene = TITLE;

void randomBallVelocity(double* vx, double* vy) {
	double rad = (rand() % 120 + 30) * M_PI / 180.0;
	*vx = cos(rad) * 4;
	*vy = sin(rad) * 7;
}

Ball* spawnBall(int x, int y, int r, int c, double vx, double vy) {
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

Racket* spawnRacket(int x, int y, int color, int w, int h) {
	Racket* racket = (Racket*)malloc(sizeof Racket);
	if (racket == NULL) return NULL;

	racket->c_x = x;
	racket->c_y = y;
	racket->color = color;
	racket->w = w;
	racket->h = h;
	return racket;
}

int drawRect(Boundary* boundary, int color) {
	int result = 0;
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

void moveBall(Ball* ball) {
	ball->x += ball->vx;
	ball->y += ball->vy;
}

int drawRacket(Racket* racket) {
	return DrawBox(racket->c_x - racket->w / 2, racket->c_y - racket->h / 2, racket->c_x + racket->w / 2, racket->c_y + racket->h / 2, racket->color, TRUE);
}

void moveRacket(Racket* racket, int dx, int dy) {
	racket->c_x = min(max(racket->c_x + dx, racket->w / 2), WIDTH - racket->w / 2);
	racket->c_y = min(max(racket->c_y + dy, racket->h / 2), HEIGHT - racket->h / 2);
}

int isHitBoundary(Ball* ball, Boundary* boundary) {
	int hit = 0;
	if (ball->vx < 0 && ball->x - ball->r < boundary->x + -ball->vx / 2) {
		ball->vx = -ball->vx;
		hit |= 1;
	}
	else if (ball->vx > 0 && ball->x + ball->r > boundary->x + boundary->w - ball->vx / 2) {
		ball->vx = -ball->vx;
		hit |= 2;
	}
	if (ball->vy < 0 && ball->y - ball->r < boundary->y + -ball->vy / 2) {
		ball->vy = -ball->vy;
		hit |= 4;
	}
	else if (ball->vy > 0 && ball->y + ball->r > boundary->y + boundary->h - ball->vy / 2) {
		ball->vy = -ball->vy;
		hit |= 8;
	}
	return hit;
}

int isHitRacket(Ball* ball, Racket* racket) {
	if (ball->vy > 0 && ball->y + ball->r > racket->c_y - racket->h / 2 - ball->vy / 2 && ball->y + ball->r < racket->c_y + racket->h / 2) {
		if (ball->x + ball->r > racket->c_x - racket->w / 2 && ball->x - ball->r < racket->c_x + racket->w / 2) {
			ball->vy = -ball->vy;
			return 1;
		}
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	srand((unsigned)time(NULL));

	SetWindowText("Pong");
	SetGraphMode(WIDTH, HEIGHT, 32);
	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1) return -1;
	SetBackgroundColor(0, 0, 0);
	SetDrawScreen(DX_SCREEN_BACK);

	const int ball_x = WIDTH / 2, ball_y = HEIGHT / 4, ball_r = 6;
	double ball_vx, ball_vy; randomBallVelocity(&ball_vx, &ball_vy);

	Ball* ball = spawnBall(ball_x, ball_y, ball_r, color[GREEN], ball_vx, ball_vy);
	Boundary* boundary = spawnBoundary(stroke, stroke, WIDTH - 2 * stroke, HEIGHT - 2 * stroke);
	Racket* racket = spawnRacket(WIDTH / 2, HEIGHT - 50, color[WHITE], 100, 20);
	int timer = 0, hitFlag = 0, hitStop = 0, spd = 0, spdUpFlag = 0;
	int score = 0, highScore = 0;

	// load assets
	int bg = LoadGraph("image/bg.png");
	int bgm = LoadSoundMem("sound/bgm.mp3");
	int gameover = LoadSoundMem("sound/gameover.mp3");
	int hit = LoadSoundMem("sound/hit.mp3");
	ChangeVolumeSoundMem(128, bgm);
	ChangeVolumeSoundMem(128, gameover);
	
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, bg, FALSE);

		if (CheckHitKey(KEY_INPUT_LEFT)) moveRacket(racket, -5, 0);
		if (CheckHitKey(KEY_INPUT_RIGHT)) moveRacket(racket, 5, 0);

		drawRacket(racket);
		drawRect(boundary, color[RED]);

		switch (Scene) {
		case TITLE:
			// alpha blending
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
			DrawBox(0, 0, WIDTH, HEIGHT, color[CYAN], TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			DrawString(100, 100, "Pong", color[WHITE]);
			DrawString(100, 200, "Press Enter to start", color[WHITE]);
			if (CheckHitKey(KEY_INPUT_RETURN)) 
			{
				Scene = GAME;
				PlaySoundMem(bgm, DX_PLAYTYPE_LOOP);
			}
			break;
		
		case GAME:
			timer++;

			DrawFormatString(10, 10, color[WHITE], "Time: %d", timer);
			DrawFormatString(10, 30, color[WHITE], "Speed: %2.2lf %2.2lf", ball->vx, ball->vy);
			DrawFormatString(WIDTH - 150, 10, color[WHITE], "Score: %d", score);
			DrawFormatString(WIDTH - 150, 30, color[WHITE], "High Score: %d", highScore);

			drawBall(ball);
			if (hitStop == 0) {
				hitFlag = 0;
				if ((timer & 0x3FF) == 0) {
					spdUpFlag++;
				}
				moveBall(ball);
			}
			else {
				if (hitFlag & 1) DrawBox(boundary->x - stroke, boundary->y - stroke, boundary->x, boundary->y + boundary->h + stroke, color[BLUE], TRUE);
				if (hitFlag & 2) DrawBox(boundary->x + boundary->w, boundary->y - stroke, boundary->x + boundary->w + stroke, boundary->y + boundary->h + stroke, color[BLUE], TRUE);
				if (hitFlag & 4) DrawBox(boundary->x - stroke, boundary->y - stroke, boundary->x + boundary->w + stroke, boundary->y, color[BLUE], TRUE);
				if (hitFlag & 8) {
					DrawBox(boundary->x - stroke, boundary->y + boundary->h, boundary->x + boundary->w + stroke, boundary->y + boundary->h + stroke, color[BLUE], TRUE);
					Scene = GAME_OVER;
					StopSoundMem(bgm);
					PlaySoundMem(gameover, DX_PLAYTYPE_BACK);
					WaitTimer(500);
					break;
				}

				hitStop--;
			}

			if (hitFlag == 0 && (hitFlag = isHitBoundary(ball, boundary)) != 0) {
				hitStop = 8;
				if (spdUpFlag) {
					spd += spdUpFlag;
					ball->vy += ball->vy > 0 ? spd : -spd;
					spdUpFlag = 0;
				}
			}
			if (hitFlag == 0 && isHitRacket(ball, racket)) {
				PlaySoundMem(hit, DX_PLAYTYPE_BACK);

				hitStop = 3;
				score += 10;
				highScore = max(highScore, score);

				if (rand() % 3 == 0) {
					double d = rand() % 100 / 100.0 * pow(-1, rand() % 2);
					ball->vx += d;
				}
				if (spdUpFlag) {
					spd += spdUpFlag;
					ball->vy += ball->vy > 0 ? spd : -spd;
					if ((rand() & 0x3) == 0) {
						ball->vx += ball->vx > 0 ? spd : -spd;
					}
					spdUpFlag = 0;
				}
			}
			break;

		case GAME_OVER:
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
			DrawBox(0, 0, WIDTH, HEIGHT, color[BLACK], TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			DrawString(100, 100, "Game Over", color[WHITE]);
			DrawString(100, 200, "Press Enter to restart", color[WHITE]);

			if (score == highScore) DrawFormatString(100, 300, color[WHITE], "New High Score!\nScore: %d", score);
			else DrawFormatString(100, 300, color[WHITE], "Score: %d", score);

			if (CheckHitKey(KEY_INPUT_RETURN)) { 
				Scene = GAME;
				StopSoundMem(gameover);
				PlaySoundMem(bgm, DX_PLAYTYPE_LOOP);
				// Reset all
				ball->x = ball_x;
				ball->y = ball_y;
				randomBallVelocity(&ball->vx, &ball->vy);
				timer = 0;
				hitFlag = 0;
				hitStop = 0;
				spd = 1;
				score = 0;
			}
			break;
		}

		ScreenFlip();
		WaitTimer(16);
		if (ProcessMessage() != 0) break;
		if (CheckHitKey(KEY_INPUT_ESCAPE)) break;
	}

	DxLib::DxLib_End();

	return 0;
}