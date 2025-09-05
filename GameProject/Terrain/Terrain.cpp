#include "./Terrain.h"
#include <stdexcept>


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

uint32_t Terrain::ToIndex(const Vector3& position)
{
    auto normPosition = position / Block::kScale;
    int x = static_cast<int>(normPosition.x);
    int y = static_cast<int>(-normPosition.y);
    int z = static_cast<int>(normPosition.z);
    return y * kSize * kSize + x * kSize + z;
}

Block::Color Terrain::GetBlockColorAt(const Vector3& position)
{
    Block* block = this->GetBlockAt(position);
    if (block)
    {
        return block->GetColor();
    }

    // 見つからなかったとき
    return Block::Color::White;
}

Block* Terrain::GetBlockAt(const Vector3& position)
{
    uint32_t i = Terrain::ToIndex(position);

    // 範囲外の場合はvector側でExeptionされる
    return blocks_.at(i).get();
}

bool Terrain::HasBlockAt(const Vector3& position)
{
    uint32_t i = Terrain::ToIndex(position);
    return i < kNumBlocks;
}

void Terrain::SetBlockColorAt(const Vector3& position, Block::Color color)
{
    Block* block = this->GetBlockAt(position);
    if (block)
    {
        block->SetColor(color);
    }
}

float Terrain::GetMaxYAt(float x, float z)
{
    int ix = static_cast<int>(x / Block::kScale);
    int iz = static_cast<int>(z / Block::kScale);

    if (ix < 0 || ix >= static_cast<int>(kSize) || iz < 0 || iz >= static_cast<int>(kSize))
    {
        throw std::out_of_range("XZ position is out of terrain bounds");
    }

    for (int y = 0; y < static_cast<int>(kHeight); ++y)
    {
        Vector3 pos(static_cast<float>(ix), static_cast<float>(-y), static_cast<float>(iz));
        if (this->HasBlockAt(pos))
        {
            Block* block = this->GetBlockAt(pos);
            if (block)
            {
                return (static_cast<float>(-y) + 0.5f) * Block::kScale;
            }
        }
    }

    // ブロックが見つからなかった場合
    return 0.0f;
}
