#include "./Terrain.h"
#include <stdexcept>
#include <imgui.h>
#include <Draw2D.h>
#include <Features/Color/Color.h>


void Terrain::Initialize(CellBasedFiltering* pCellFilter)
{
    const float blockScale = Block::kScale;

    // Initialize InstancedObject3d
    instancedObjectBlock_ = std::make_unique<InstancedObject3d>();
    instancedObjectBlock_->Initialize("cube.obj");

    // Initialize terrain blocks
    Transform tf = {};

    for (int x = 0; x < kSize; ++x)
    {
        for (int y = 0; y < kHeight; ++y)
        {
            for (int z = 0; z < kSize; ++z)
            {
                bool isUpper = (x < kWidth && z < kWidth) || (x >= kWidth && z >= kWidth);
                tf.translate = Vector3(static_cast<float>(x), static_cast<float>(-y), static_cast<float>(z));
                tf.translate *= blockScale;
                tf.translate.x += blockScale * 0.5f;
                tf.translate.y -= blockScale * 0.5f; // yは下方向に伸びているので補正
                tf.translate.z += blockScale * 0.5f;

                if (isUpper)
                {
                    auto block = std::make_unique<Block>();
                    block->Initialize(instancedObjectBlock_->CreateInstance(tf));
                    if (pCellFilter) pCellFilter->AssignToGrid(block->GetCollider());
                    blocks_[x][y][z] = std::move(block);
                }
                else if (y == 1)
                {
                    auto block = std::make_unique<Block>();
                    block->Initialize(instancedObjectBlock_->CreateInstance(tf));
                    if (pCellFilter) pCellFilter->AssignToGrid(block->GetCollider());
                    blocks_[x][y][z] = std::move(block);
                }
            }
        }
    }
}

void Terrain::Finalize()
{
}

void Terrain::Update()
{
    instancedObjectBlock_->Update();

    for (int x = 0; x < kSize; ++x)
    {
        for (int y = 0; y < kHeight; ++y)
        {
            for (int z = 0; z < kSize; ++z)
            {
                if (blocks_[x][y][z])
                {
                    blocks_[x][y][z]->Update();
                }
            }
        }
    }
}

void Terrain::Draw()
{
    instancedObjectBlock_->Draw();

    for (int x = 0; x < kSize; ++x)
    {
        for (int y = 0; y < kHeight; ++y)
        {
            for (int z = 0; z < kSize; ++z)
            {
                if (blocks_[x][y][z])
                {
                    blocks_[x][y][z]->Draw();
                }
            }
        }
    }
}

void Terrain::ImGui()
{
    bool isOpen = ImGui::Begin("Terrain");
    if (isOpen)
    {
        ImGui::SeparatorText("Select block");
        ImGui::Indent(15.0f);
        int x = static_cast<int>(selectPosition_.x);
        int y = static_cast<int>(selectPosition_.y);
        int z = static_cast<int>(selectPosition_.z);
        ImGui::SliderInt("X", &x, 0, kSize-1);
        ImGui::SliderInt("Y", &y, -static_cast<int>(kHeight) + 1, 0);
        ImGui::SliderInt("Z", &z, 0, kSize-1);

        selectPosition_ = Vector3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));

        AABB aabb = {};
        aabb.min = selectPosition_ * Block::kScale;
        aabb.min.y -= Block::kScale; // yは下方向に伸びているので補正

        aabb.max = aabb.min + Vector3(Block::kScale, Block::kScale, Block::kScale);
        Draw2D::GetInstance()->DrawAABB(aabb, Color(0x4688ecff).Vec4());

        ImGui::Separator();
        ImGui::Indent(15.0f);

        auto& block = blocks_[x][-y][z];
        if (block)
        {
            blocks_[x][-y][z]->ImGui();
        }
        else
        {
            ImGui::Text("This is air block");
        }

        ImGui::Unindent(15.0f);
        ImGui::Unindent(15.0f);
    }
    ImGui::End();
};

Vector3 Terrain::ToLocalVector3(uint32_t index)
{
    // kSize * kHeight * kSize
    int y = -static_cast<int>(index / (kSize * kSize));
    int x = (index / kSize) % kSize;
    int z = index % kSize;

    return Vector3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
}

Block::Colors Terrain::GetBlockColorAt(const Vector3& position)
{
    Block* block = this->GetBlockAt(position);
    if (block)
    {
        return block->GetColor();
    }

    // 見つからなかったとき
    return Block::Colors::White;
}

Block* Terrain::GetBlockAt(const Vector3& position)
{
    // 範囲外の場合はvector側でExeptionされる
    auto normPosition = position / Block::kScale;
    int x = static_cast<int>(normPosition.x);
    int y = static_cast<int>(-normPosition.y);
    int z = static_cast<int>(normPosition.z);
    return blocks_[x][y][z].get();
}

bool Terrain::HasBlockAt(const Vector3& position)
{
    try
    {
        Block* block = this->GetBlockAt(position);
        return block != nullptr;
    }
    catch (const std::out_of_range&)
    {
        return false;
    }
}

void Terrain::SetBlockColorAt(const Vector3& position, Block::Colors color)
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
        throw std::out_of_range("Terrain::GetMaxYAt: x or z is out of range");
    }

    for (int y = 0; y < static_cast<int>(kHeight); ++y)
    {
        if (blocks_[ix][y][iz])
        {
            return -static_cast<float>(y) * Block::kScale;
        }
    }

    // ブロックが見つからなかった場合、地面の高さを返す
    return 0.0f;
}
