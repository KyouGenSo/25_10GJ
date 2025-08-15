#include "Particle.hlsli"

// ���\�[�X�o�C���f�B���O
StructuredBuffer<Particle> gParticles : register(t0);
ConstantBuffer<PerView> gPerView : register(b0);

// ���_�V�F�[�_�[����
struct VertexShaderInput
{
    float4 pos : POSITION;
    float2 texcoord : TEXCOORD0;
};

// ���_�V�F�[�_�[���C���֐�
VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexShaderOutput output;
    
    // �C���X�^���XID�ɑΉ�����p�[�e�B�N���f�[�^���擾
    Particle particle = gParticles[instanceID];
    
    // �r���{�[�h�s����擾
    float4x4 worldMat = gPerView.billboardMat;
    
    // �p�[�e�B�N���̃X�P�[����K�p
    worldMat[0] *= particle.scale.x;
    worldMat[1] *= particle.scale.y;
    worldMat[2] *= particle.scale.z;
    
    // �p�[�e�B�N���̈ʒu��K�p
    worldMat[3].xyz = particle.translate;
    
    // ���_�ʒu�̌v�Z
    output.pos = mul(input.pos, mul(worldMat, gPerView.viewProj));
    
    // �e�N�X�`�����W�����̂܂܏o��
    output.texcoord = input.texcoord;
    
    // �F�����o��
    // �����Ɋ�Â��ĐF����`���
    float lifeRatio = particle.currentTime / particle.lifeTime;
    output.color = lerp(particle.startColor, particle.endColor, lifeRatio);
    
    return output;
}