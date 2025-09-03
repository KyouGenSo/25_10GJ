#include "./Terrain.h"

void Terrain::Initialize()
{
    // (x * y * z) を意識
    blocks_.reserve(kNumBlocks);

    // Initialize terrain blocks
    for (int i = 0; i < kNumBlocks; ++i)
    {
        auto block = std::make_unique<Block>();
        block->SetPosition(this->ToVector3(i));
        blocks_.emplace_back(std::move(block));
    }
}

void Terrain::Finalize()
{
    blocks_.clear();
}

void Terrain::Update()
{
    for (auto& block : blocks_)
    {
        block->Update();
    }
}

void Terrain::Draw()
{
    for (auto& block : blocks_)
    {
        block->Draw();
    }
}

Vector3 Terrain::ToVector3(uint32_t index)
{
    // kSize * kHeight * kSize
    int y = index / (kSize * kSize);
    int x = (index / kSize) % kSize;
    int z = index % kSize;

    return Vector3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
}
