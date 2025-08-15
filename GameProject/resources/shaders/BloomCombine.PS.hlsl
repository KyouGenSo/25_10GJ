#include "FullScreen.hlsli"

cbuffer Param : register(b0)
{
    float intensity;
};

Texture2D<float4> baseTex : register(t0); // ���摜
Texture2D<float4> bloomTex : register(t1); // �u���[���摜
SamplerState smp : register(s0);

float4 main(VertexShaderOutput input) : SV_TARGET
{
    float4 baseColor = baseTex.Sample(smp, input.texCoord);
    float4 bloomColor = bloomTex.Sample(smp, input.texCoord);

	//// �u���[�����ʂ̋��x����
    //bloomColor *= intensity;

    // �u���[���F�̋����i����`�����j
    bloomColor.rgb = pow(bloomColor.rgb, 0.8) * intensity * 1.5;
    
    // ���摜�ƃu���[�����ʂ����Z����
    float3 result = baseColor.rgb + bloomColor.rgb;
    
    // �g�[���}�b�s���O�I�ȏ����Ŏ��R�Ȗ��邳�ɖ߂�
    //result = result / (1.0 + result);
    
    return float4(result, 1.0f);
}