#include "DxLib.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// �E�B���h�E���[�h�ɐݒ�
	ChangeWindowMode(TRUE);

	// �c�w���C�u��������������
	if (DxLib_Init() == -1)
	{
		return -1;	// �G���[���N�����璼���ɏI��
	}

	// ��ʃ��[�h�̐ݒ�
	SetGraphMode(640, 480, 16);

	// ��ʂ̏�����
	ClearDrawScreen();

	// �}�`�̕`��
	DrawBox(100, 100, 200, 200, GetColor(255, 0, 0), TRUE);
	DrawCircle(320, 240, 100, GetColor(0, 255, 0), TRUE);
	DrawLine(0, 0, 640, 480, GetColor(0, 0, 255));

	// ��ʂ̍X�V
	ScreenFlip();

	// �L�[���͑҂�
	WaitKey();

	// �c�w���C�u�����̌�n��
	DxLib_End();

	return 0;
}