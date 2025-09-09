#include "CellBasedFiltering.h"
#include <array>
#include <set>
#include <Draw2D.h>
#include <Features/Color/Color.h>
#include <imgui.h>



void CellBasedFiltering::Initialize(int cellSize, int worldWidth, int worldHeight)
{
    cellSize_ = cellSize;
    worldWidth_ = worldWidth;
    worldHeight_ = worldHeight;
    numCellsX_ = (worldWidth_ + cellSize_ - 1) / cellSize_;
    numCellsZ_ = (worldHeight_ + cellSize_ - 1) / cellSize_;
    grid_.resize(static_cast<size_t>(numCellsX_ * numCellsZ_));
}

void CellBasedFiltering::UnregisterAll(CollisionManager* pManager)
{
    for (const auto& collider : potentialColliders_)
    {
        pManager->RemoveCollider(collider);
    }
    potentialColliders_.clear();
}

void CellBasedFiltering::ReassignToGridAll(int cellSize)
{
    cellSize_ = cellSize;
    numCellsX_ = (worldWidth_ + cellSize_ - 1) / cellSize_;
    numCellsZ_ = (worldHeight_ + cellSize_ - 1) / cellSize_;
    std::vector<AABBCollider*> assignedColliders{};
    for (auto& cell : grid_)
    {
        for (auto& collider : cell)
        {
            assignedColliders.push_back(collider);
        }
    }

    grid_.clear();
    grid_.resize(static_cast<size_t>(numCellsX_ * numCellsZ_));

    for (auto& collider : assignedColliders)
    {
        AssignToGrid(collider);
    }
}

void CellBasedFiltering::Draw2d()
{
    for (auto& collider : potentialColliders_)
    {
        auto aabb = collider->GetAABB();
        Draw2D::GetInstance()->DrawAABB(aabb, Color(0xff00ffff).Vec4());
    }
}

void CellBasedFiltering::DrawImGui()
{
    if (ImGui::Begin("Cell Based Filtering"))
    {
        ImGui::Checkbox("Enable modify", &isModifyMode_);

        if (!isModifyMode_)
        {
            ImGui::Text("Cell Size: %d", cellSize_);
        }
        else
        {
            ImGui::SliderInt("Cell Size", &cellSize_, 2, 20);
            if (ImGui::Button("Reassign All"))
            {
                ReassignToGridAll(cellSize_);
                isModifyMode_ = false;
            }
        }

        ImGui::Text("World Size: %d x %d", worldWidth_, worldHeight_);
        ImGui::Text("Num Cells: %d x %d", numCellsX_, numCellsZ_);
        ImGui::Text("Total Cells: %zu", grid_.size());
        ImGui::Text("Potential Colliders: %zu", potentialColliders_.size());

        for (auto& uniqueCell : uniqueCells_)
        {
            ImGui::Text("Potencial cell index: %llu", uniqueCell);
        }
    }
    ImGui::End();
}

void CellBasedFiltering::AssignToGrid(AABBCollider* collider)
{
    auto aabb = collider->GetAABB();
    Vector3 min = aabb.min;
    Vector3 max = aabb.max;
    Vector3 corners[4] = {
        {min.x, 0.0f, min.z},
        {max.x, 0.0f, min.z},
        {min.x, 0.0f, max.z},
        {max.x, 0.0f, max.z}
    };

    std::set<uint64_t> uniqueCells{};
    for (const auto& corner : corners)
    {
        uint64_t cellIndex = ToCellIndex(corner);
        uniqueCells.insert(cellIndex);
    }

    for (const auto& cellIndex : uniqueCells)
    {
        if (cellIndex < grid_.size())
        {
            grid_[cellIndex].push_back(collider);
        }
    }
}

void CellBasedFiltering::RegisterPotentials(AABBCollider* pCollider)
{
    auto aabb = pCollider->GetAABB();

    Vector3 min = aabb.min;
    Vector3 max = aabb.max;

    Vector3 corners[4] = {
        {min.x, 0.0f, min.z},
        {max.x, 0.0f, min.z},
        {min.x, 0.0f, max.z},
        {max.x, 0.0f, max.z}
    };

    uniqueCells_.clear();
    for (const auto& corner : corners)
    {
        uint64_t cellIndex = ToCellIndex(corner);
        if (cellIndex < grid_.size())
        uniqueCells_.insert(cellIndex);
    }

    for (const auto& cellIndex : uniqueCells_)
    {
        if (cellIndex < grid_.size())
        {
            for (const auto& collider : grid_[cellIndex])
            {
                potentialColliders_.insert(collider);
            }
        }
    }
}

void CellBasedFiltering::RegisterAll(CollisionManager* pManager)
{
    for (const auto& collider : potentialColliders_)
    {
        pManager->AddCollider(collider);
    }
}

uint64_t CellBasedFiltering::ToCellIndex(const Vector3& position) const
{
    // ワールド座標からセルのインデックスを計算
    uint64_t ix = static_cast<uint64_t>(position.x) / cellSize_;
    uint64_t iz = static_cast<uint64_t>(position.z) / cellSize_;
    return numCellsX_ * iz + ix;
}
