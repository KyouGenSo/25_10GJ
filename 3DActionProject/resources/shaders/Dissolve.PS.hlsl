#include "FullScreen.hlsli"

struct DissolvePram
{
    float threshold;
    float edgeThickness;
    float4 edgeColor;
};

ConstantBuffer<DissolvePram> gParam : register(b0);
Texture2D<float4> gTexture : register(t0);     // �V�[���̃J���[�e�N�X�`��
Texture2D<float> gMaskTexture : register(t1);  // �}�X�N�e�N�X�`��
Texture2D<float4> gBaseTexture : register(t2); // ���n�̃J���[�e�N�X�`��
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

float4 main(VertexShaderOutput input) : SV_TARGET
{
    PixelShaderOutput output;

	// ���n�̐F���擾
    float4 baseColor = gBaseTexture.Sample(gSampler, input.texCoord);

	// �}�X�N�̒l���擾
    float mask = gMaskTexture.Sample(gSampler, input.texCoord);

    // �}�X�N�̒l��臒l�𒴂��Ă���ꍇ�A���n�̐F���g�p
    if (mask <= gParam.threshold)
    {
        return baseColor;
    }

    float edgeJudge = gParam.threshold + gParam.edgeThickness;
    if (gParam.threshold > 1.0f)
    {
        edgeJudge = 1.0f;
    }
    float edge = 1.0f - smoothstep(gParam.threshold, edgeJudge, mask);

	// �V�[���̐F���擾
    output.color = gTexture.Sample(gSampler, input.texCoord);

    output.color.rgb += edge * gParam.edgeColor.rgb; // �G�b�W�̐F�����Z

    return output.color;

}