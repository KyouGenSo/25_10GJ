#include "Particle.hlsli"

// ���\�[�X�o�C���f�B���O
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// �s�N�Z���V�F�[�_�[�o��
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

// �s�N�Z���V�F�[�_�[���C���֐�
PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // �e�N�X�`������J���[���擾
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    // �p�[�e�B�N���̐F�Ə�Z
    output.color = textureColor * input.color;
    
    // �A���t�@�l��0�ȉ��Ȃ�`�悵�Ȃ�
    if (output.color.a <= 0.0f)
    {
        discard;
    };
    
    return output;
}