#pragma once
#include "FrameRate.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "FrameRate.h"
#include <memory>
#include <wrl/client.h>
#include <d3d11.h>
#include <atltypes.h>

struct DebugMenuLineup
{
	double m_FrameRate;
	unsigned int m_SpriteNum;
};

class DebugMenu
{
private:
	int m_CharacterTextureHorizontalNum = 16;
	int m_CharacterTextureVerticalNum = 8;

	Texture m_TextureImage;
	VertexBuffer m_Vertex;
	std::vector<DWORD> m_IndexList;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TextureView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_Texture;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SamplerState;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_ConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_BlendState;
	Microsoft::WRL::ComPtr<ID3D11Device> m_Device;

public:
	DebugMenu();
	DebugMenu(const DebugMenu& debugDisp) = delete;
	DebugMenu& operator=(const DebugMenu& debugDisp) = delete;
	/* virtual */ ~DebugMenu();

	HRESULT Create(const Microsoft::WRL::ComPtr<ID3D11Device>& device);
	void ExecDispString(HWND hwnd, const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& dContext, const std::string& str, int x, int y) ;
	CRect GetRectCharacterTexture(int asciicode) const;
	CPoint GetDispPointCharacterTexture(int x, int y, int charactorNo) const;
	void DrawString(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& dContext);
};

