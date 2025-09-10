#include "ClearScene.h"

#include "Draw2D.h"
#include "GPUParticle.h"
#include "Input.h"
#include "Object3dBasic.h"
#include "SceneManager.h"
#include "SpriteBasic.h"

#ifdef _DEBUG
#include"ImGui.h"
#include "DebugCamera.h"
#endif

void ClearScene::Initialize()
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

        // 背景スプライトの初期化
    bgSprite_ = std::make_unique<Sprite>();
    bgSprite_->Initialize("black_BG.png");
    bgSprite_->SetPos(bgPos_);
    bgSprite_->SetColor(Vector4(0.2f, 0.2f, 0.2f, 0.8f));

    // GameOverスプライトの初期化
    clearSprite_ = std::make_unique<Sprite>();
    clearSprite_->Initialize("gameClear_Text.png");
    clearSprite_->SetPos(clearPos_);
    clearSprite_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    // ボタンスプライトの初期化
    buttonSprite_ = std::make_unique<Sprite>();
    buttonSprite_->Initialize("press_text.png");
    buttonSprite_->SetPos(buttonPos_);
    buttonSprite_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
}


void ClearScene::Finalize()
{

}

void ClearScene::Update()
{
#ifdef _DEBUG
    if (Input::GetInstance()->TriggerKey(DIK_F1)) {
        Object3dBasic::GetInstance()->SetDebug(!Object3dBasic::GetInstance()->GetDebug());
        Draw2D::GetInstance()->SetDebug(!Draw2D::GetInstance()->GetDebug());
        GPUParticle::GetInstance()->SetIsDebug(!GPUParticle::GetInstance()->GetIsDebug());
        isDebug_ = !isDebug_;
    }

    if (isDebug_) {
        DebugCamera::GetInstance()->Update();
    }
#endif
    /// ================================== ///
    ///              更新処理               ///
    /// ================================== ///

    // スプライトの更新処理
    bgSprite_->Update();
    clearSprite_->Update();
    buttonSprite_->Update();

    if (Input::GetInstance()->TriggerKey(DIK_SPACE) || Input::GetInstance()->TriggerButton(XButtons.A))
    {
        SceneManager::GetInstance()->ChangeScene("title");
    }
}

void ClearScene::Draw()
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

void ClearScene::DrawWithoutEffect()
{
    /// ================================== ///
    ///              描画処理               ///
    /// ================================== ///

    //------------------背景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();

    bgSprite_->Draw();


    //-------------------Modelの描画-------------------//
    // 3Dモデル共通描画設定
    Object3dBasic::GetInstance()->SetCommonRenderSetting();





    //------------------前景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();

    clearSprite_->Draw();
    buttonSprite_->Draw();


}

void ClearScene::DrawImGui()
{
#ifdef _DEBUG

    /// ================================== ///
    ///             ImGuiの描画              ///
    /// ================================== ///


#endif // _DEBUG
}
