#pragma once
#include <vector>
#include <AABBCollider.h>
#include <CollisionManager.h>
#include <set>

// コライダーのセル分割によるフィルタリング
// 衝突判定マネージャ登録ヘルパークラス
class CellBasedFiltering
{
public:
    CellBasedFiltering() = default;
    ~CellBasedFiltering() = default;

    void Initialize(int cellSize, int maxWorldX, int maxWorldZ);
    void Draw2d();
    void DrawImGui();

    void AssignToGrid(AABBCollider* collider);
    void RegisterPotentials(AABBCollider* pCollider);
    void RegisterAll(CollisionManager* pManager);
    void UnregisterAll(CollisionManager* pManager);

private:
    uint64_t ToCellIndex(const Vector3& position) const;

    int cellSize_       = 0;
    int worldWidth_     = 0;
    int worldHeight_    = 0;
    int numCellsX_      = 0;
    int numCellsZ_      = 0;

    std::vector<std::vector<AABBCollider*>> grid_;
    std::set<AABBCollider*> potentialColliders_;
    std::set<uint64_t> uniqueCells_;
};