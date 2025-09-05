#include "./Terrain.h"


void Terrain::Initialize()
{
    // (x * y * z) を意識
    blocks_.reserve(kNumBlocks);

    const float blockScale = Block::kScale;

    // Initialize terrain blocks
    for (int i = 0; i < kNumBlocks; ++i)
    {
        Vector3 position = this->ToVector3(i) * blockScale;
        position.y -= blockScale * .5f;

        auto block = std::make_unique<Block>();
        block->Initialize();
        block->SetPosition(position);
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
    int y = -static_cast<int>(index / (kSize * kSize));
    int x = (index / kSize) % kSize;
    int z = index % kSize;

    return Vector3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
}
