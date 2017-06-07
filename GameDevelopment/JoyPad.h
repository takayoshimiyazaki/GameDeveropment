//
// JoyPad�������N���X
//
#pragma once

// DirectInput�̃o�[�W������ݒ�
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <vector>

// �����L�[�̃t���O�̒�`
#define PAD_DIR_UP		0x01	// ��
#define PAD_DIR_LEFT	0x02	// ��
#define PAD_DIR_DOWN	0x04	// ��
#define PAD_DIR_RIGHT	0x08	// �E
#define PAD_DIR_NONE	0x10	// �����Ă��Ȃ�

template<typename T>
void SafeRelease(T*& p)
{
	if (p)
	{
		p->Release();
		p = NULL;
	}
}

class JoyPad
{
public:
	JoyPad();
	~JoyPad();

	// ������
	bool Initialize(HWND window);
	// �X�V
	void Update();

	bool buttonPush(int padId, int buttonId);

	bool buttonTrigger(int padId, int buttonId);

	bool buttonRelease(int padId, int buttonId);

	int buttonPushTime(int padId, int buttonId);

	LONG getStickLX(int padId);

	LONG getStickLY(int padId);

	LONG getStickLZ(int padId);

	LONG getStickLRX(int padId);

	LONG getStickLRY(int padId);

	LONG getStickLRZ(int padId);

	BYTE getDirectionKey(int padId);

private:
	// �W���C�p�b�h1���̏��
	struct JoyPadSet
	{
		JoyPadSet()
		{
			inputDevice = NULL;
		}
		// �f�o�C�X
		LPDIRECTINPUTDEVICE8 inputDevice;
		// ���͏��
		DIJOYSTATE2 joypad;
		// �O��̓��͏��
		DIJOYSTATE2 joypadOld;
	};

	// DirectInput�C���X�^���X
	LPDIRECTINPUT8 m_pDInput;

	// �W���C�p�b�h�z��
	std::vector<JoyPadSet> m_joyPadSet;
		
};
