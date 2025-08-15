#include "Particle.hlsli"

// ���\�[�X�o�C���f�B���O
RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);

ConstantBuffer<PerFrame> gPerFrame : register(b0);

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint particleIndex = DTid.x;
    
    // �L���Ȕ͈͂̃p�[�e�B�N���̂ݏ���
    if (particleIndex < kMaxParticles)
    {
        // �A���t�@�l��0���傫�����A�N�e�B�u�ȃp�[�e�B�N���̂ݍX�V
        if (any((gParticles[particleIndex].startColor.a > 0.0f) && (gParticles[particleIndex].endColor.a > 0.0f)))
        {
            // �ʒu�̍X�V
            gParticles[particleIndex].translate += gParticles[particleIndex].velocity;
            
            // �o�ߎ��Ԃ̍X�V
            gParticles[particleIndex].currentTime += gPerFrame.deltaTime;
            
            // �����Ɋ�Â��ăA���t�@�l���v�Z
            float alpha = 1.0f - (gParticles[particleIndex].currentTime / gParticles[particleIndex].lifeTime);
            gParticles[particleIndex].startColor.a = saturate(alpha);
            gParticles[particleIndex].endColor.a = saturate(alpha);
            
            // �����؂�Ȃ�t���[���X�g�ɖ߂�����
            if (alpha <= 0.0f)
            {
                gParticles[particleIndex].startColor.a = 0.0f;
                gParticles[particleIndex].endColor.a = 0.0f;
                gParticles[particleIndex].scale = float3(0.0f, 0.0f, 0.0f);
                
                // �t���[���X�g�ɒǉ�
                int freeListIndex;
                InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
                
                // �͈̓`�F�b�N
                if (freeListIndex >= 0 && (freeListIndex + 1) < kMaxParticles)
                {
                    gFreeList[freeListIndex + 1] = particleIndex;
                }
                else
                {
                    // �G���[�P�[�X�̏���
                    InterlockedAdd(gFreeListIndex[0], -1);
                }
            }
        }
    }
}