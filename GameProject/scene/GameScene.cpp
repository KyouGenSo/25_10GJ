#include "GameScene.h"
#include "ModelManager.h"
#include "Object3dBasic.h"
#include "SpriteBasic.h"
#include "Input.h"
#include "DebugCamera.h"
#include "Draw2D.h"
#include "GlobalVariables.h"
#include "FrameTimer.h"
#include "GPUParticle.h"
#include "SceneManager.h"
#include "EmitterManager.h"
#include "Object3d.h"
#include "Model.h"

#include <numbers>

#ifdef _DEBUG
#include"ImGui.h"
#include "DebugCamera.h"
#endif

void GameScene::Initialize()
{
#ifdef _DEBUG
    DebugCamera::GetInstance()->Initialize();
    Object3dBasic::GetInstance()->SetDebug(false);
    Draw2D::GetInstance()->SetDebug(false);
    GPUParticle::GetInstance()->SetIsDebug(false);
#endif
    /// ================================== ///
    ///              初期化処理              ///
    /// ================================== ///

    // SkyBoxの初期化
    skyBox_ = std::make_unique<SkyBox>();
    skyBox_->Initialize("my_skybox.dds");

    // 床モデルのUV変換設定
    groundUvTransform_.translate = Vector3(0.0f, 0.0f, 0.0f);
    groundUvTransform_.rotate = Vector3(0.0f, 0.0f, 0.0f);
    groundUvTransform_.scale = Vector3(100.0f, 100.0f, 100.0f);
    // 床モデルの初期化
    ground_ = std::make_unique<Object3d>();
    ground_->Initialize();
    ground_->SetModel("ground_black.gltf");
    ground_->SetUvTransform(groundUvTransform_);

}

void GameScene::Finalize()
{

}

void GameScene::Update()
{
#ifdef _DEBUG
    if (Input::GetInstance()->TriggerKey(DIK_F1))
    {
        Object3dBasic::GetInstance()->SetDebug(!Object3dBasic::GetInstance()->GetDebug());
        Draw2D::GetInstance()->SetDebug(!Draw2D::GetInstance()->GetDebug());
        GPUParticle::GetInstance()->SetIsDebug(!GPUParticle::GetInstance()->GetIsDebug());
        isDebug_ = !isDebug_;
    }

    if (isDebug_)
    {
        DebugCamera::GetInstance()->Update();
    }
#endif
    /// ================================== ///
    ///              更新処理               ///
    /// ================================== ///

    skyBox_->Update();
    ground_->Update();

    // シーン遷移
    if (Input::GetInstance()->TriggerKey(DIK_RETURN))
    {
        SceneManager::GetInstance()->ChangeScene("title");
    }
}

void GameScene::Draw()
{
    /// ================================== ///
    ///              描画処理               ///
    /// ================================== ///

    skyBox_->Draw();

    //------------------背景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();



    //-------------------Modelの描画-------------------//
    // 3Dモデル共通描画設定
    Object3dBasic::GetInstance()->SetCommonRenderSetting();

    ground_->Draw();

    //------------------前景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();



}

void GameScene::DrawWithoutEffect()
{
    /// ================================== ///
    ///              描画処理               ///
    /// ================================== ///

  //------------------背景Spriteの描画------------------//
  // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();



  //-------------------Modelの描画-------------------//
  // 3Dモデル共通描画設定
    Object3dBasic::GetInstance()->SetCommonRenderSetting();



    //------------------前景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();



}

void GameScene::DrawImGui()
{
#ifdef _DEBUG


#endif // DEBUG
}
