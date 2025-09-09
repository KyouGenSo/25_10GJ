#pragma once
#include <memory>
#include "Transform.h"

class Boss;
class Object3d;
class EnergyCoreCollider;

class EnergyCore
{
public:
    EnergyCore();
    ~EnergyCore();

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize(Boss* boss);

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

    /// <summary>
    /// 破壊処理
    /// </summary>
    void Destroy();

    /// <summary>
    /// 再生成処理
    /// </summary>
    void Respawn();

    //-----------------------------Getters/Setters------------------------------//
    void SetTransform(const Transform& transform) { transform_ = transform; }
    void SetIsDestroyed(bool destroyed) { isDestroyed_ = destroyed; }

    const Transform& GetTransform() const { return transform_; }
    Transform* GetTransformPtr() { return &transform_; }
    bool GetIsDestroyed() const { return isDestroyed_; }

private:
    std::unique_ptr<Object3d> model_;           ///< モデル
    Transform transform_{};                     ///< 変形情報
    Boss* pBoss_ = nullptr;                     ///< ボスへの参照
    bool isDestroyed_ = false;                  ///< 破壊状態
    
    // Collider
    std::unique_ptr<EnergyCoreCollider> collider_;
};