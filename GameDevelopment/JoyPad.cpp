#include "JoyPad.h"

// ���o�����W���C�p�b�h�̏����i�[���邽�߂̍\����
struct SearchJoyPadSet
{
	// DirectInput
	LPDIRECTINPUT8 dinput;

	// �e�R���g���[�����Ƃ̃f�o�C�X
	std::vector<LPDIRECTINPUTDEVICE8> joyPadDev;

	// �R���X�g���N�^
	SearchJoyPadSet()
	{
		dinput = NULL;
	}
};

BOOL CALLBACK EnumObjectCallBack(LPCDIDEVICEOBJECTINSTANCE pInstance, void* pvRef)
{


	// ��������
	return DIENUM_CONTINUE;
}

// ���o�����W���C�p�b�h���ƂɌĂ΂��R�[���o�b�N
BOOL CALLBACK EnumJoyPadCallBack(LPCDIDEVICEINSTANCE lpddi, void* pvRef)
{
	// ����
	HRESULT ret;

	SearchJoyPadSet* param = reinterpret_cast<SearchJoyPadSet*>(pvRef);

	// DirectInputDevice
	LPDIRECTINPUTDEVICE8 dev;

	// �R���g���[�����ƂɃf�o�C�X�𐶐�
	ret = param->dinput->CreateDevice(
		lpddi->guidInstance,
		&dev,
		NULL);

	// ���s������
	if (ret != DI_OK)
		goto NEXT;



NEXT:
	// ��������
	return DIENUM_CONTINUE;
}

JoyPad::JoyPad()
{
}

JoyPad::~JoyPad()
{
}

bool JoyPad::Initialize(HWND window)
{
	// ����
	HRESULT ret;

	// DirectInput�̍쐬
	ret = DirectInput8Create(
		GetModuleHandle(NULL),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pDInput,
		NULL);

	// ���s������
	if (ret != DI_OK)
		return false;

	// �󂯓n���p�p�����[�^
	SearchJoyPadSet param;
	param.dinput = m_pDInput;

	// ���p�\�ȃf�o�C�X��񋓂���
	ret = m_pDInput->EnumDevices(
		DI8DEVCLASS_GAMECTRL,
		EnumJoyPadCallBack,
		&param,
		DIEDFL_ATTACHEDONLY);

	// ���s������
	if (ret != DI_OK)
		return false;

	return true;
}
