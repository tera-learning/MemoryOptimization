#include "DebugMenu.h"
#include "DrawManager.h"

DebugMenu::DebugMenu()
{
	m_TextureImage.LoadTexture("AsciiCode.nbmp");
}


DebugMenu::~DebugMenu()
{
}

HRESULT DebugMenu::Create(const Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
	m_Device = device;

	//////////////////////////////////////////////////////////////////////////////////
	//�e�N�X�`���̐ݒ�
	//////////////////////////////////////////////////////////////////////////////////
	D3D11_TEXTURE2D_DESC texture2DDesc;
	ZeroMemory(&texture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texture2DDesc.Width = m_TextureImage.GetWidth();
	texture2DDesc.Height = m_TextureImage.GetHeight();
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
	subTextureResource.pSysMem = m_TextureImage.GetTextureBuffer();
	subTextureResource.SysMemPitch = m_TextureImage.GetWidth() * m_TextureImage.GetPixelByte();

	HRESULT hresult = device->CreateTexture2D(&texture2DDesc, &subTextureResource, &m_Texture);

	if (FAILED(hresult))
		return hresult;

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.Format = texture2DDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	hresult = device->CreateShaderResourceView(m_Texture.Get(), &shaderResourceViewDesc, &m_TextureView);

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

	hresult = device->CreateSamplerState(&samplerDesc, &m_SamplerState);

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

	hresult = device->CreateBuffer(&constantBufferDesc, nullptr, &m_ConstantBuffer);

	if (FAILED(hresult))
		return hresult;


	//////////////////////////////////////////////////////////////////////////////////
	//�u�����h�X�e�[�g�̐ݒ�
	//////////////////////////////////////////////////////////////////////////////////
	D3D11_BLEND_DESC BlendDesc;
	ZeroMemory( &BlendDesc, sizeof( BlendDesc ) );

	BlendDesc.AlphaToCoverageEnable = FALSE;
	BlendDesc.IndependentBlendEnable = FALSE;
	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hresult = device->CreateBlendState( &BlendDesc, &m_BlendState );

	
	return hresult;
}

void DebugMenu::ExecDispString(HWND hwnd, const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& dContext, const char* str, int strnum, int x, int y) 
{

	//�E�B���h�E�������W���擾
	CRect rect;
	CPoint center;
	GetClientRect(hwnd, &rect);
	center = rect.CenterPoint();

	float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

	for (unsigned int i = 0; i < strnum; i++)
	{

		//�����\���ʒu���擾
		CPoint charDispPoint = GetDispPointCharacterTexture(x, y, i);
		float dispx = static_cast<float>(charDispPoint.x);
		float dispy = static_cast<float>(charDispPoint.y);

		//�\�����镶���̃e�N�X�`����̈ʒu���擾����
		CRect charTexRect = GetRectCharacterTexture(str[i]);

		float left = static_cast<float>(charTexRect.left) / m_TextureImage.GetWidth();
		float top = static_cast<float>(charTexRect.top) / m_TextureImage.GetHeight();
		float right = static_cast<float>(charTexRect.right) / m_TextureImage.GetWidth();
		float bottom = static_cast<float>(charTexRect.bottom) / m_TextureImage.GetHeight();
		unsigned int index = m_Vertex.GetSize();

		m_Vertex.AddVertex({ {0.0f, 0.0f}, {dispx, dispy}, {left, top}, {1.0f, 0.0f, 0.0f, 0.0f} });
		m_Vertex.AddVertex({ {1.0f, 0.0f}, {dispx, dispy}, {right, top}, {1.0f, 0.0f, 0.0f, 0.0f} });
		m_Vertex.AddVertex({ {0.0f, 1.0f}, {dispx, dispy}, {left, bottom}, {1.0f, 0.0f, 0.0f, 0.0f} });
		m_Vertex.AddVertex({ {1.0f, 1.0f}, {dispx, dispy}, {right, bottom}, {1.0f, 0.0f, 0.0f, 0.0f} });

		m_IndexList.push_back(index);
		m_IndexList.push_back(index + 1);
		m_IndexList.push_back(index + 2);
		m_IndexList.push_back(index + 1);
		m_IndexList.push_back(index + 3);
		m_IndexList.push_back(index + 2);
	}

	//�N���C�A���g�̈�̒��S��ConstantBuffer�ɐݒ�
	ConstantBuffer constantBuffer;
	constantBuffer.centerWindow[0] = center.x;
	constantBuffer.centerWindow[1] = center.y;
	constantBuffer.height = m_TextureImage.GetHeight() / m_CharacterTextureVerticalNum;
	constantBuffer.width = m_TextureImage.GetWidth() / m_CharacterTextureHorizontalNum;


	//////////////////////////////////////////////////////////////////////////////////
	//�`��
	//////////////////////////////////////////////////////////////////////////////////
	dContext->PSSetShaderResources(0, 1, m_TextureView.GetAddressOf());						//�e�N�X�`���̐ݒ�
	dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);				//���_�o�b�t�@���ǂ̏��ԂŎO�p�`����邩
	dContext->PSSetSamplers(0, 1, m_SamplerState.GetAddressOf());							//�T���v���[�̐ݒ�
	dContext->VSSetConstantBuffers(0, 1, m_ConstantBuffer.GetAddressOf());					//�萔�o�b�t�@�̐ݒ�	
	dContext->UpdateSubresource(m_ConstantBuffer.Get(), 0, nullptr, &constantBuffer, 0, 0);	//�萔�o�b�t�@�̍X�V
	dContext->OMSetBlendState( m_BlendState.Get(), blendFactor, 0xffffffff );				//�A���t�@�u�����h�̐ݒ�

}

CRect DebugMenu::GetRectCharacterTexture(int asciicode) const 
{

	int unitWidth = m_TextureImage.GetWidth() / m_CharacterTextureHorizontalNum;
	int unitHeight = m_TextureImage.GetHeight() / m_CharacterTextureVerticalNum;
	int charx = asciicode % m_CharacterTextureHorizontalNum;
	int chary = asciicode / m_CharacterTextureHorizontalNum;

	CRect rect;
	rect.left = charx * unitWidth;
	rect.top = chary * unitHeight;
	rect.right = rect.left + unitWidth;
	rect.bottom = rect.top + unitHeight;

	return rect;
}

CPoint DebugMenu::GetDispPointCharacterTexture(int x, int y, int charctorNo) const
{
	CPoint point;

	float unitWidth = static_cast<float>(m_TextureImage.GetWidth()) / m_CharacterTextureHorizontalNum;

	point.x = x + static_cast<int>((charctorNo * unitWidth));
	point.y = y;

	return point;
}

void DebugMenu::DrawString(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& dContext) 
{
	UINT strides = sizeof(Vertex);
	UINT offsets = 0;

	//////////////////////////////////////////////////////////////////////////////////
	//���_�o�b�t�@�̐���
	//////////////////////////////////////////////////////////////////////////////////
	D3D11_BUFFER_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.ByteWidth = sizeof(Vertex) * m_Vertex.GetVertexNum();	//�o�b�t�@�[�̃T�C�Y (�o�C�g�P��)
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;									//�o�b�t�@�[�őz�肳��Ă���ǂݍ��݂���я������݂̕��@
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;						//�o�b�t�@�[���ǂ̂悤�Ƀp�C�v���C���Ƀo�C���h���邩
	bufferDesc.CPUAccessFlags = 0;											//CPU �A�N�Z�X�̃t���O
	bufferDesc.MiscFlags = 0;												//���̑��̃t���O
	bufferDesc.StructureByteStride = 0;										//�\���̂��\�����o�b�t�@�[��\���ꍇ�A���̍\���̂̃T�C�Y (�o�C�g�P��)

	D3D11_SUBRESOURCE_DATA subresource;
	ZeroMemory(&subresource, sizeof(D3D11_SUBRESOURCE_DATA));
	subresource.pSysMem = m_Vertex.GetVertexList();	//�������f�[�^�ւ̃|�C���^
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
	indexBufferDesc.ByteWidth = sizeof(DWORD) * m_IndexList.size();
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = m_IndexList.data();
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	hresult = m_Device->CreateBuffer(&indexBufferDesc, &subData, &m_IndexBuffer);

	if (FAILED(hresult))
		return ;


	dContext->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &strides, &offsets);	//���_�o�b�t�@�̐ݒ�
	dContext->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);	//���_�o�b�t�@�̐ݒ�
	dContext->DrawIndexed(m_IndexList.size(), 0, 0);	//�`�悷��

	m_Vertex.ClearVertex();
	m_IndexList.clear();
}
