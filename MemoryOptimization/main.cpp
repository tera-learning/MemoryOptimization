#include "DrawManager.h"
#include <atltypes.h>
#include <Windows.h>

DrawManager dx11Manager;
unsigned int totalAllocatedNum = 0;
unsigned int currentAllocatedNum = 0;
std::size_t totalMemorySize = 0;
std::size_t peakMemorySize = 0;
const std::size_t memoryHeader = 16;

//�}�W�b�N�i���o�[���g��Ȃ��悤�ɂ���
struct MemoryHeader
{
	std::size_t size;
	char padding[8];
};

void* operator new(std::size_t n)
{
	//���蓖�Ă����v��(New���ꂽ��)
	totalAllocatedNum++;
	//���݊��蓖�ĂĂ鐔(���݊m�ۂ���Ă��鐔)
	currentAllocatedNum++;

	std::size_t allocateSize = n + memoryHeader;

	//���v�g�p��������
	totalMemorySize += allocateSize;
	//�s�[�N���̎g�p��������
	peakMemorySize = (totalMemorySize > peakMemorySize) ? totalMemorySize : peakMemorySize;
	
	//�������m��
	void* p = std::malloc(allocateSize);
	char* memory = nullptr;

	if (p != nullptr)
	{
		//�������Ɋm�ۂ����e�ʂ�ێ����Ă���
		*(static_cast<std::size_t*>(p)) = n;
		//���ۂɎg�p����̈��Ԃ�
		memory = static_cast<char *>(p) + memoryHeader;
	}
	
	return memory;
}

void operator delete(void* ptr)
{
	if (ptr != nullptr)
	{
		void* ptrHeader = static_cast<char *>(ptr) - memoryHeader;
		char* memory = static_cast<char *>(ptrHeader);

		if (memory != nullptr)
		{
			//New�����Ƃ��Ɋm�ۂ����T�C�Y���擾
			std::size_t size = *(static_cast<std::size_t*>(ptrHeader));
			//���v�g�p�������ʂ��獷������
			totalMemorySize -= (size + memoryHeader);
			//���݊��蓖�ĂĂ鐔����Ђ�
			currentAllocatedNum--;

			//�������̈���
			std::free(memory);
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		dx11Manager.UpdateSpriteList(wp);
		break;
	}

	return DefWindowProc(hWnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /* hPrevInstance */, LPSTR /* lpCmdLine */, int nCmdShow)
{

	WNDCLASSEX winc;

	//�\���̂̃T�C�Y
	winc.cbSize = sizeof(WNDCLASSEX);
	//�E�B���h�E�N���X�̊�{�X�^�C����\���萔
	winc.style = CS_HREDRAW | CS_VREDRAW;
	//�E�B���h�E�v���V�[�W���i�E�B���h�E�Ŕ��������C�x���g����������֐��j�̃|�C���g�^
	winc.lpfnWndProc = WndProc;
	//�N���X�̍\���̂̒ǉ��̈�
	winc.cbClsExtra = 0;
	//�E�B���h�E�\���̂̒ǉ��̈�
	winc.cbWndExtra = 0;
	//�C���X�^���X�n���h��
	winc.hInstance = hInstance;
	//�f�X�N�g�b�v���ɕ`�悳���A�C�R���̏������A�C�R���n���h��
	winc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	//�E�B���h�E�̃N���C�A���g�G���A��̃}�E�X�J�[�\��
	winc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//�N���C�A���g�G���A�̔w�i�F
	winc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	//�N���X���j���[
	winc.lpszMenuName = nullptr;
	//�E�B���h�E�N���X�̖��O
	winc.lpszClassName = TEXT("Polygon");
	//�^�X�N�o�[��^�C�g���o�[�ɕ\������鏬�����A�C�R���̏������A�C�R���n���h��
	winc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	if (!RegisterClassEx(&winc)) return 0;


	//////////////////////////////////////////////////////////////////////////////////
	//�E�B���h�E����
	//////////////////////////////////////////////////////////////////////////////////
	int width = 600;
	int height = 600;

	HWND hwnd = CreateWindow(
		winc.lpszClassName,
		TEXT("Polygon"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0,
		0,
		width,
		height,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	if (hwnd == NULL) return 0;


	//////////////////////////////////////////////////////////////////////////////////
	//�N���C�A���g�̈��ōĐ���
	//////////////////////////////////////////////////////////////////////////////////
	CRect crect, wrect;
	GetWindowRect(hwnd, &wrect);
	GetClientRect(hwnd, &crect);

	int newWidth = width + ((wrect.right - wrect.left) - (crect.right - crect.left));
	int newHeight = height + ((wrect.bottom - wrect.top) - (crect.bottom - crect.top));

	SetWindowPos(hwnd, nullptr, 0, 0, newWidth, newHeight, SWP_SHOWWINDOW);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	

	//////////////////////////////////////////////////////////////////////////////////
	//�`��
	//////////////////////////////////////////////////////////////////////////////////
	dx11Manager.Create(hwnd);


	//////////////////////////////////////////////////////////////////////////////////
	//�`��
	//////////////////////////////////////////////////////////////////////////////////
	MSG msg{};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			dx11Manager.UpdateWindow();
			dx11Manager.Render(hwnd);
		}
	}

	return static_cast<int>(msg.wParam);
}