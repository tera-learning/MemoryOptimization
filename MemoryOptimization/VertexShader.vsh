///////////////////////////////////////////////////////
// �O���[�o��
///////////////////////////////////////////////////////
cbuffer ConstBuffer : register(b0)
{
    int2 gCenterWindow;
    int gHeight;
    int gWidth;
}

///////////////////////////////////////////////////////
// �\���̐錾�u���b�N
///////////////////////////////////////////////////////
struct vertexInput
{
    float2 pos : POSITION0;
    float2 centerpos : POSITION1;
    float2 tex : TEXCOORD0;
    float4 col : COLOR0;
};

struct vertexOutput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 col : COLOR0;
};

///////////////////////////////////////////////////////
// ���_�V�F�[�_�v���O�����u���b�N
///////////////////////////////////////////////////////
vertexOutput vs_main(vertexInput inData)
{
    vertexOutput outData;
    
    float x = inData.centerpos[0] - (gWidth / 2.0f) + (inData.pos[0] * gWidth);
    float y = inData.centerpos[1] - (gHeight / 2.0f) + (inData.pos[1] * gHeight);
    
    float2 center = float2(gCenterWindow[0], gCenterWindow[1]);
    float2 vertex = float2(x, y);
    
    outData.pos[0] = (vertex[0] - center[0]) / center[0];
    outData.pos[1] = (center[1] - vertex[1]) / center[1];
    outData.pos[2] = 1.0f;
    outData.pos[3] = 1.0f;
    
    outData.tex = inData.tex;
    outData.col = inData.col;
    
    return outData;
}
