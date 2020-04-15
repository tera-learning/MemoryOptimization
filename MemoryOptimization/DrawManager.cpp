#include "DrawManager.h"
#include "VertexBuffer.h"
#include "SpriteControllerKeyBinding.h"
#include "MoveSprite.h"
#include <atltypes.h>
#include <d3dcompiler.h>
#include <random>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

extern unsigned int totalAllocatedNum ;
extern unsigned int currentAllocatedNum ;
extern std::size_t totalMemorySize ;
extern std::size_t peakMemorySize ;

HRESULT DrawManager::Create(HWND hwnd)
{

	//////////////////////////////////////////////////////////////////////////////////
	//�E�B���h�E�T�C�Y�擾
	//////////////////////////////////////////////////////////////////////////////////
	CRect rect;
	GetClientRect(hwnd, &rect);


	//////////////////////////////////////////////////////////////////////////////////
	//�f�o�C�X�ƃX���b�v�`�F�C���̐���
	//////////////////////////////////////////////////////////////////////////////////
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
	desc.BufferCount = 1;									// �X���b�v�`�F�C���̃o�b�t�@��
	desc.BufferDesc.Width = rect.Width();					// �X���b�v�`�F�C���̃o�b�t�@�T�C�Y
	desc.BufferDesc.Height = rect.Height();					// �X���b�v�`�F�C���̃o�b�t�@�T�C�Y
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// �X���b�v�`�F�C���̃o�b�t�@�t�H�[�}�b�g
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// �o�b�t�@�������_�[�^�[�Q�b�g�Ƃ��Ďg�p
	desc.OutputWindow = hwnd;								// HWND�n���h��
	desc.SampleDesc.Count = 1;								// �}���`�T���v�����O�̃s�N�Z���P�ʂ̐�
	desc.SampleDesc.Quality = 0;							// �}���`�T���v�����O�̕i��
	desc.Windowed = TRUE;									// �E�B���h�E���[�h

	D3D_FEATURE_LEVEL level;

	HRESULT hresult = D3D11CreateDeviceAndSwapChain(
		nullptr,					//�ǂ̃r�f�I�A�_�v�^���g�p���邩IDXGIAdapter�̃A�h���X��n��.����Ȃ��nullptr
		D3D_DRIVER_TYPE_HARDWARE,	//�h���C�o�̃^�C�v
		nullptr,					//��L��D3D_DRIVER_TYPE_SOFTWARE�ɐݒ肵���ۂɁA���̏������s��DLL�̃n���h��
		0,							//�t���O�w��.D3D11_CREATE_DEVICE�񋓌^
		nullptr,					//�쐬�����݂�@�\���x���̏������w�肷��z��
		0,							//�쐬�����݂�@�\���x���̏������w�肷��z��̐�
		D3D11_SDK_VERSION,			//SDK�̃o�[�W����
		&desc,						//�X���b�v�`�F�C���̏������p�����[�^�[
		&m_Swapchain,				//�쐬�����X���b�v�`�F�C��
		&m_Device,					//�쐬�����f�o�C�X
		&level,						//�쐬���ꂽ�f�o�C�X�̋@�\���x��
		&m_Context					//�쐬�����f�o�C�X�R���e�L�X�g
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//�o�b�N�o�b�t�@�̎擾
	//////////////////////////////////////////////////////////////////////////////////
	ID3D11Texture2D* backBuffer;
	hresult = m_Swapchain->GetBuffer(
		0,							//�o�b�t�@�̃C���f�b�N�X(��{��0)
		IID_PPV_ARGS(&backBuffer)	//�o�b�t�@�̎擾��
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//�����_�����O�^�[�Q�b�g�̐���
	//////////////////////////////////////////////////////////////////////////////////
	hresult = m_Device->CreateRenderTargetView(
		backBuffer,			//�쐬����o�b�t�@�̃��\�[�X
		nullptr,			//�쐬����View�̐ݒ���e�f�[�^�̎w��(nullptr�Ńf�t�H���g�ݒ�ɂȂ�)
		&m_RenderTargetView	//�쐬���ꂽRenderTargetView
	);

	backBuffer->Release();
	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//�萔�o�b�t�@�̐ݒ�
	//////////////////////////////////////////////////////////////////////////////////
	D3D11_BUFFER_DESC constantBufferDesc;
	ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));
	constantBufferDesc.ByteWidth = ((sizeof(ConstantBuffer) - 1) / 16 + 1) * 16;
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;									//�o�b�t�@�[�őz�肳��Ă���ǂݍ��݂���я������݂̕��@
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;						//�o�b�t�@�[���ǂ̂悤�Ƀp�C�v���C���Ƀo�C���h���邩
	constantBufferDesc.CPUAccessFlags = 0;											//CPU �A�N�Z�X�̃t���O

	hresult = m_Device->CreateBuffer(&constantBufferDesc, nullptr, &m_ConstantBuffer);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//VertexShader�̐���
	//////////////////////////////////////////////////////////////////////////////////
	ID3DBlob* pBlob;

	hresult = D3DCompileFromFile(
		L"VertexShader.vsh",			//�t�@�C����
		nullptr,							//D3D_SHADER_MACRO�\���̂̔z����w��.�V�F�[�_�}�N�����`����ۂɐݒ肷��.
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	//�R���p�C�����C���N���[�h�t�@�C������舵�����߂Ɏg�p����ID3DInclude�C���^�t�F�[�X�ւ̃|�C���^
		"vs_main",							//�G���g���[�|�C���g�̃��\�b�h��
		"vs_5_0",							//�R���p�C���^�[�Q�b�g���w��
		0,									//�V�F�[�_�̃R���p�C���I�v�V����
		0,									//�G�t�F�N�g�t�@�C���̃R���p�C���I�v�V����
		&pBlob,								//�R���p�C�����ꂽ�R�[�h�փA�N�Z�X���邽�߂�ID3DBlob�C���^�t�F�[�X�̃|�C���^
		nullptr								//�R���p�C���G���[���b�Z�[�W�փA�N�Z�X���邽�߂�ID3DBlob�C���^�t�F�[�X�̃|�C���^
	);

	if (FAILED(hresult))
		return hresult;

	hresult = m_Device->CreateVertexShader(
		pBlob->GetBufferPointer(),	//�R���p�C���ς݃V�F�[�_�[�ւ̃|�C���^
		pBlob->GetBufferSize(),		//�R���p�C���ςݒ��_�V�F�[�_�[�̃T�C�Y
		nullptr,					//ID3D11ClassLinkage �C���^�[�t�F�[�X�ւ̃|�C���^
		&m_VertexShader				//ID3D11VertexShader �C���^�[�t�F�C�X�ւ̃|�C���^
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//���̓��C�A�E�g�̐���
	//////////////////////////////////////////////////////////////////////////////////
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] = {
		{ "POSITION",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "POSITION",	1, DXGI_FORMAT_R32G32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hresult = m_Device->CreateInputLayout(
		vertexDesc,					//���̓A�Z���u���[ �X�e�[�W�̓��̓f�[�^�^�̔z��
		ARRAYSIZE(vertexDesc),		//���͗v�f�̔z����̓��̓f�[�^�^�̐�
		pBlob->GetBufferPointer(),	//�R���p�C���ς݃V�F�[�_�[�ւ̃|�C���^
		pBlob->GetBufferSize(),		//�R���p�C���ς݃V�F�[�_�[�̃T�C�Y
		&m_InputLayout				//�쐬�������̓��C�A�E�g �I�u�W�F�N�g�ւ̃|�C���^
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//PixelShader�̐���
	//////////////////////////////////////////////////////////////////////////////////
	hresult = D3DCompileFromFile(
		L"PixelShader.psh",			//�t�@�C����
		nullptr,							//D3D_SHADER_MACRO�\���̂̔z����w��.�V�F�[�_�}�N�����`����ۂɐݒ肷��.
		D3D_COMPILE_STANDARD_FILE_INCLUDE,	//�R���p�C�����C���N���[�h�t�@�C������舵�����߂Ɏg�p����ID3DInclude�C���^�t�F�[�X�ւ̃|�C���^
		"ps_main",							//�G���g���[�|�C���g�̃��\�b�h��
		"ps_5_0",							//�R���p�C���^�[�Q�b�g���w��
		0,									//�V�F�[�_�̃R���p�C���I�v�V����
		0,									//�G�t�F�N�g�t�@�C���̃R���p�C���I�v�V����
		&pBlob,								//�R���p�C�����ꂽ�R�[�h�փA�N�Z�X���邽�߂�ID3DBlob�C���^�t�F�[�X�̃|�C���^
		nullptr								//�R���p�C���G���[���b�Z�[�W�փA�N�Z�X���邽�߂�ID3DBlob�C���^�t�F�[�X�̃|�C���^
	);

	if (FAILED(hresult))
		return hresult;

	hresult = m_Device->CreatePixelShader(
		pBlob->GetBufferPointer(),	//�R���p�C���ς݃V�F�[�_�[�ւ̃|�C���^
		pBlob->GetBufferSize(),		//�R���p�C���ςݒ��_�V�F�[�_�[�̃T�C�Y
		nullptr,					//ID3D11ClassLinkage �C���^�[�t�F�[�X�ւ̃|�C���^
		&m_PixelShader				//ID3D11PixelShader �C���^�[�t�F�C�X�ւ̃|�C���^
	);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//�e�N�X�`���̍쐬
	//////////////////////////////////////////////////////////////////////////////////
	m_TextureImage = std::make_shared<Texture>();
	m_TextureImage->LoadTexture("Image.nbmp");


	//////////////////////////////////////////////////////////////////////////////////
	//�e�N�X�`���̐ݒ�
	//////////////////////////////////////////////////////////////////////////////////
	D3D11_TEXTURE2D_DESC texture2DDesc;
	ZeroMemory(&texture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texture2DDesc.Width = m_TextureImage->GetWidth();
	texture2DDesc.Height = m_TextureImage->GetHeight();
	texture2DDesc.MipLevels = 1;
	texture2DDesc.ArraySize = 1;
	texture2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture2DDesc.SampleDesc.Count = 1;
	texture2DDesc.SampleDesc.Quality = 0;
	texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texture2DDesc.CPUAccessFlags = 0;
	texture2DDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subTextureResource;
	ZeroMemory(&subTextureResource, sizeof(D3D11_SUBRESOURCE_DATA));
	subTextureResource.pSysMem = m_TextureImage->GetTextureBuffer();
	subTextureResource.SysMemPitch = m_TextureImage->GetWidth() * m_TextureImage->GetPixelByte();

	hresult = m_Device->CreateTexture2D(&texture2DDesc, &subTextureResource, &m_Texture);

	if (FAILED(hresult))
		return hresult;

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.Format = texture2DDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	hresult = m_Device->CreateShaderResourceView(m_Texture.Get(), &shaderResourceViewDesc, &m_TextureView);

	if (FAILED(hresult))
		return hresult;
	
	
	//////////////////////////////////////////////////////////////////////////////////
	//�T���v���[�̍쐬
	//////////////////////////////////////////////////////////////////////////////////
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;	//�g��k�����̐F�̎擾���@
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;		//UV���W���͈͊O�̏ꍇ�̐F�̎擾���@
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;		//UV���W���͈͊O�̏ꍇ�̐F�̎擾���@
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;		//UV���W���͈͊O�̏ꍇ�̐F�̎擾���@

	hresult = m_Device->CreateSamplerState(&samplerDesc, &m_SamplerState);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//�r���[�|�[�g�̐���
	//////////////////////////////////////////////////////////////////////////////////
	m_ViewPort.TopLeftX = 0;					//�r���[�|�[�g�̍����� X �ʒu
	m_ViewPort.TopLeftY = 0;					//�r���[�|�[�g�̏㕔�� Y �ʒu
	m_ViewPort.Width = (FLOAT)rect.Width();		//�r���[�|�[�g�̕�
	m_ViewPort.Height = (FLOAT)rect.Height();	//�r���[�|�[�g�̍���
	m_ViewPort.MinDepth = 0.0f;					//�r���[�|�[�g�̍ŏ��[�x
	m_ViewPort.MaxDepth = 1.0f;					//�r���[�|�[�g�̍ő�[�x


	//////////////////////////////////////////////////////////////////////////////////
	//�f�o�b�O���j���[����
	//////////////////////////////////////////////////////////////////////////////////
	m_DebugMenu.Create(m_Device);
	m_FrameRate.StartMeasureTime();


	return hresult;
}

void DrawManager::Render(HWND hwnd)
{

	//�N���C�A���g�̈�擾
	CRect rect;
	CPoint center;
	GetClientRect(hwnd, &rect);
	center = rect.CenterPoint();

	UINT strides = sizeof(Vertex);
	UINT offsets = 0;
	FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	VertexBuffer vertex;
	std::vector<DWORD> indexList;
	std::vector<Sprite>::iterator it;
	for (it = m_SpriteList.begin(); it != m_SpriteList.end(); it++)
	{
		float x = rect.Width() * it->m_DrawX;
		float y = rect.Height() * it->m_DrawY;
		DWORD index = vertex.GetSize();

		vertex.AddVertex({ {0.0f, 0.0f}, {x, y}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f} });
		vertex.AddVertex({ {1.0f, 0.0f}, {x, y}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f} });
		vertex.AddVertex({ {0.0f, 1.0f}, {x, y}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f} });
		vertex.AddVertex({ {1.0f, 1.0f}, {x, y}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f} });

		indexList.push_back(index);
		indexList.push_back(index + 1);
		indexList.push_back(index + 2);
		indexList.push_back(index + 1);
		indexList.push_back(index + 3);
		indexList.push_back(index + 2);
	}

	if (vertex.GetSize() > 0)
	{
		//////////////////////////////////////////////////////////////////////////////////
		//���_�o�b�t�@�̐���
		//////////////////////////////////////////////////////////////////////////////////
		D3D11_BUFFER_DESC bufferDesc;

		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.ByteWidth = sizeof(Vertex) * vertex.GetVertexNum();	//�o�b�t�@�[�̃T�C�Y (�o�C�g�P��)
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;									//�o�b�t�@�[�őz�肳��Ă���ǂݍ��݂���я������݂̕��@
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;						//�o�b�t�@�[���ǂ̂悤�Ƀp�C�v���C���Ƀo�C���h���邩
		bufferDesc.CPUAccessFlags = 0;											//CPU �A�N�Z�X�̃t���O
		bufferDesc.MiscFlags = 0;												//���̑��̃t���O
		bufferDesc.StructureByteStride = 0;										//�\���̂��\�����o�b�t�@�[��\���ꍇ�A���̍\���̂̃T�C�Y (�o�C�g�P��)

		D3D11_SUBRESOURCE_DATA subresource;
		ZeroMemory(&subresource, sizeof(D3D11_SUBRESOURCE_DATA));
		subresource.pSysMem = vertex.GetVertexList();	//�������f�[�^�ւ̃|�C���^
		subresource.SysMemPitch = 0;							//�e�N�X�`���[�ɂ��� 1 �{�̐��̐�[����ׂ̐��܂ł̋��� (�o�C�g�P��) 
		subresource.SysMemSlicePitch = 0;						//1 �̐[�x���x���̐�[����ׂ̐[�x���x���܂ł̋��� (�o�C�g�P��)

		HRESULT hresult = m_Device->CreateBuffer(
			&bufferDesc,	//�o�b�t�@�[�̋L�q�ւ̃|�C���^
			&subresource,	//�������f�[�^�ւ̃|�C���^
			&m_VertexBuffer	//�쐬�����o�b�t�@�[�ւ̃|�C���^
		);

		if (FAILED(hresult))
			return;
		

		//////////////////////////////////////////////////////////////////////////////////
		//���_�C���f�b�N�X
		//////////////////////////////////////////////////////////////////////////////////
		D3D11_BUFFER_DESC indexBufferDesc;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * indexList.size();
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA subData;
		subData.pSysMem = indexList.data();
		subData.SysMemPitch = 0;
		subData.SysMemSlicePitch = 0;

		hresult = m_Device->CreateBuffer(&indexBufferDesc, &subData, &m_IndexList);

		if (FAILED(hresult))
			return ;


		m_Context->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &strides, &offsets);	//���_�o�b�t�@�̐ݒ�
		m_Context->IASetIndexBuffer(m_IndexList.Get(), DXGI_FORMAT_R32_UINT, 0);	//���_�o�b�t�@�̐ݒ�
	}

	m_Context->ClearRenderTargetView(m_RenderTargetView.Get(), color);						//�����_�[�^�[�Q�b�g���N���A����
	m_Context->IASetInputLayout(m_InputLayout.Get());										//�C���v�b�g���C�A�E�g�̐ݒ�
	m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);				//���_�o�b�t�@���ǂ̏��ԂŎO�p�`����邩
	m_Context->VSSetConstantBuffers(0, 1, m_ConstantBuffer.GetAddressOf());					//�萔�o�b�t�@�̐ݒ�	
	m_Context->VSSetShader(m_VertexShader.Get(), nullptr, 0);								//���_�V�F�[�_�[�̐ݒ�
	m_Context->RSSetViewports(1, &m_ViewPort);												//�r���[�|�[�g�̐ݒ�
	m_Context->PSSetShader(m_PixelShader.Get(), nullptr, 0);								//�s�N�Z���V�F�[�_�[�̐ݒ�
	m_Context->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), nullptr);			//�����_�[�^�[�Q�b�g�̐ݒ�
	m_Context->PSSetShaderResources(0, 1, m_TextureView.GetAddressOf());					//�e�N�X�`���̐ݒ�
	m_Context->PSSetSamplers(0, 1, m_SamplerState.GetAddressOf());							//�T���v���[�̐ݒ�

	//�N���C�A���g�̈�̒��S��ConstantBuffer�ɐݒ�
	ConstantBuffer constantBuffer;
	constantBuffer.centerWindow[0] = center.x;
	constantBuffer.centerWindow[1] = center.y;
	constantBuffer.height = m_TextureImage->GetHeight();
	constantBuffer.width = m_TextureImage->GetWidth();

	//////////////////////////////////////////////////////////////////////////////////
	//�`��
	//////////////////////////////////////////////////////////////////////////////////
	m_Context->UpdateSubresource(m_ConstantBuffer.Get(), 0, nullptr, &constantBuffer, 0, 0);//�萔�o�b�t�@�̍X�V
	m_Context->DrawIndexed(indexList.size(), 0, 0);										//�`�悷��

	// �t���[�����[�g�v��
	m_FrameRate.IncrFrame();

	// �f�o�b�O���j���[�\��
	std::string title = "< DebugDisp >";
	std::string strSpriteNum = "  SpriteNum : " + std::to_string(m_SpriteList.size());
	std::string strFrameRate = "  FrameRate : " + std::to_string(m_FrameRate.GetFrameRate());
	std::string strTotalNewCount = "  TotalAllocatedCount : " + std::to_string(totalAllocatedNum);
	std::string strCurrentNewNum = "  CurrentAllocatedNum : " + std::to_string(currentAllocatedNum);
	std::string strUsedMemorySize = "  TotalMemorySize : " + std::to_string(totalMemorySize);
	std::string strPeakMemorySize = "  PeakMemorySize : " + std::to_string(peakMemorySize);
	std::string strOneFrameTime = "  OneFrameTime [microsecond]: " + std::to_string(m_FrameRate.GetOneFrameTime());
	std::string strMaxOneFrameTime = "  MaxOneFrameTime [microsecond]: " + std::to_string(m_FrameRate.GetMaxOneFrameTime());
	std::string strPangram = "  Pangram: The quick brown fox jumps over the lazy dog.";

	//�����ŐF�w��ł���Ƃ悢
	//hwnd, m_Context�B���ł���Ƃ���
	m_DebugMenu.ExecDispString(hwnd, m_Context, title, 20, 20);
	m_DebugMenu.ExecDispString(hwnd, m_Context, strSpriteNum, 20, 40);
	m_DebugMenu.ExecDispString(hwnd, m_Context, strFrameRate, 20, 60);
	m_DebugMenu.ExecDispString(hwnd, m_Context, strTotalNewCount, 20, 80);
	m_DebugMenu.ExecDispString(hwnd, m_Context, strCurrentNewNum, 20, 100);
	m_DebugMenu.ExecDispString(hwnd, m_Context, strUsedMemorySize, 20, 120);
	m_DebugMenu.ExecDispString(hwnd, m_Context, strPeakMemorySize, 20, 140);
	m_DebugMenu.ExecDispString(hwnd, m_Context, strOneFrameTime, 20, 160);
	m_DebugMenu.ExecDispString(hwnd, m_Context, strMaxOneFrameTime, 20, 180);
	m_DebugMenu.ExecDispString(hwnd, m_Context, strPangram, 20, 200);
	m_DebugMenu.DrawString(m_Context);
	m_Swapchain->Present(0, 0);
}

void DrawManager::UpdateWindow()
{
	MoveSprite moveSprite;
	moveSprite.Exec(&m_SpriteList);
}

void DrawManager::UpdateSpriteList(WPARAM key)
{
	SpriteControllerKeyBinding spriteControllerKeyBinding;
	std::unique_ptr<ControlSprite> controlSprite = spriteControllerKeyBinding.CreateController(static_cast<int>(key));
	if (controlSprite != nullptr) {
		controlSprite->UpdateList(&m_SpriteList);
	}
}
