//
// JoyPad�������N���X
//
#pragma once

// DirectInput�̃o�[�W������ݒ�
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <vector>

class JoyPad
{
public:
	JoyPad();
	~JoyPad();

	bool Initialize(HWND window);

private:
	// DirectInput�C���X�^���X
	LPDIRECTINPUT8 m_pDInput;

};
