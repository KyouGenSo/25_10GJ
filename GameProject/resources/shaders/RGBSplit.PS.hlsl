#include "FullScreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// RGB Split�p�����[�^
struct RGBSplitParam
{
    float2 redOffset;
    float2 greenOffset;
    float2 blueOffset;
    float intensity;
};

// CBV(Constant Buffer View)
ConstantBuffer<RGBSplitParam> gRGBSplitParam : register(b0);

float4 main(VertexShaderOutput input) : SV_TARGET
{
  // ���̃e�N�X�`�����W
    float2 texCoord = input.texCoord;
  
  // �e�`�����l���̃T���v�����O�ʒu���I�t�Z�b�g����
    float r = gTexture.Sample(gSampler, texCoord + gRGBSplitParam.redOffset * gRGBSplitParam.intensity).r;
    float g = gTexture.Sample(gSampler, texCoord + gRGBSplitParam.greenOffset * gRGBSplitParam.intensity).g;
    float b = gTexture.Sample(gSampler, texCoord + gRGBSplitParam.blueOffset * gRGBSplitParam.intensity).b;
    float a = gTexture.Sample(gSampler, texCoord).a;
  
  // ���ʂ�g�ݍ��킹��
    return float4(r, g, b, a);
}