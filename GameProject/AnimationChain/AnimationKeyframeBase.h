#pragma once
#include <cstdint>

class AnimationKeyframeBase
{
public:
    AnimationKeyframeBase() = default;
    ~AnimationKeyframeBase() = default;

    inline uint32_t GetNumFrame() const { return numFrame; }

protected:
    uint32_t numFrame;
};