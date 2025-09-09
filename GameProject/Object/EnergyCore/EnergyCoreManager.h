#pragma once
#include <memory>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "Transform.h"

class Boss;
class Terrain;
class EnergyCore;

class EnergyCoreManager
{
public:
    EnergyCoreManager();
    ~EnergyCoreManager();

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize(Boss* boss, Terrain* terrain);

    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize();

    /// <summary>
    /// 更新
    /// </summary>
    void Update();

    /// <summary>
    /// 描画
    /// </summary>
    void Draw();

    /// <summary>
    /// ImGuiの描画
    /// </summary>
    void DrawImGui();

    //-----------------------------Getters/Setters------------------------------//
    size_t GetCoreCount() const { return energyCores_.size(); }
    EnergyCore* GetCore(size_t index);

private:
    /// <summary>
    /// エネルギーコアをTerrain上にランダム配置
    /// </summary>
    void PlaceRandomly();

    /// <summary>
    /// 全エネルギーコアが破壊されたかチェック
    /// </summary>
    bool CheckAllDestroyed() const;

    /// <summary>
    /// 全エネルギーコアを再生成
    /// </summary>
    void RespawnAll();

private:
    static const int kCoreCount = 4;  ///< エネルギーコアの数
    
    std::vector<std::unique_ptr<EnergyCore>> energyCores_;  ///< エネルギーコア
    Boss* pBoss_ = nullptr;                                 ///< ボスへの参照
    Terrain* pTerrain_ = nullptr;                          ///< Terrainへの参照
};