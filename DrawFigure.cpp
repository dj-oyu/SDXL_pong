#include "DxLib.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// ウィンドウモードに設定
	ChangeWindowMode(TRUE);

	// ＤＸライブラリ初期化処理
	if (DxLib_Init() == -1)
	{
		return -1;	// エラーが起きたら直ちに終了
	}

	// 画面モードの設定
	SetGraphMode(640, 480, 16);

	// 画面の初期化
	ClearDrawScreen();

	// 図形の描画
	DrawBox(100, 100, 200, 200, GetColor(255, 0, 0), TRUE);
	DrawCircle(320, 240, 100, GetColor(0, 255, 0), TRUE);
	DrawLine(0, 0, 640, 480, GetColor(0, 0, 255));

	// 画面の更新
	ScreenFlip();

	// キー入力待ち
	WaitKey();

	// ＤＸライブラリの後始末
	DxLib_End();

	return 0;
}