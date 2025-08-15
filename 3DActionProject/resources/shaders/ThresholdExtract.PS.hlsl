#include "FullScreen.hlsli"

cbuffer Param : register(b0)
{
    float threshold;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

//float4 BloomExtract(float2 texcoord)
//{
//    float4 color = gTexture.Sample(gSampler, texcoord);
//    // 臒l�͈̔͂��`
//    float minThreshold = threshold - 0.1f;
//    float maxThreshold = threshold;
//    // smoothstep�Ŋ��炩��臒l�K�p
//    float brightness = max(color.r, max(color.g, color.b));
//    float factor = smoothstep(minThreshold, maxThreshold, brightness);
//    return color * factor;
//}

float4 main(VertexShaderOutput input) : SV_TARGET
{
    float4 color = gTexture.Sample(gSampler, input.texCoord);
    
    // �P�x�v�Z�iRGB���P�x�ւ̕ϊ��j
    float brightness = dot(color.rgb, float3(0.299, 0.587, 0.114));
    
    // 臒l�ȏ�̖��邳�̂ݒ��o
    float4 output = brightness > threshold ? color : float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    // ���ʂ��o��
    return output;

}