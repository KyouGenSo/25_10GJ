#include "TitleScene.h"
#include "SceneManager.h"
#include "TextureManager.h"
#include "Object3dBasic.h"
#include "SpriteBasic.h"
#include "ModelManager.h"
#include "Draw2D.h"
#include "GlobalVariables.h"
#include "GPUParticle.h"

#ifdef _DEBUG
#include"ImGui.h"
#include "DebugCamera.h"
#endif

void TitleScene::Initialize()
{
    #ifdef _DEBUG
    DebugCamera::GetInstance()->Initialize();
    Object3dBasic::GetInstance()->SetDebug(false);
    Draw2D::GetInstance()->SetDebug(false);
    GPUParticle::GetInstance()->SetIsDebug(false);
    #endif

    input_ = Input::GetInstance();

    /// ================================== ///
    ///              初期化処理              ///
    /// ================================== ///
    this->SpritesInitialize();

    terrain_ = std::make_unique<Terrain>();
    terrain_->Initialize();

    this->AnimationsInitialize();
}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
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

    this->AnimationsUpdate();
    this->SpritesUpdate();
    this->TerrainUpdate();

    if (Input::GetInstance()->TriggerKey(DIK_RETURN))
    {
        SceneManager::GetInstance()->ChangeScene("game");
    }
}

void TitleScene::Draw()
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
    terrain_->Draw();



    //------------------前景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();



}

void TitleScene::DrawWithoutEffect()
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
    pSpriteBackground_->Draw();
    pSpriteToonRaider1_->Draw();
    pSpriteToonRaider2_->Draw();
}

void TitleScene::DrawImGui()
{
    #ifdef _DEBUG

    /// ================================== ///
    ///             ImGuiの描画              ///
    /// ================================== ///
    pTitleAnimation_->ImGui();

    #endif // _DEBUG
}

void TitleScene::SpritesInitialize()
{
    float screenRatio = static_cast<float>(WinApp::clientWidth) / 1920.0f;

    const Vector2 screenSize =
    {
        static_cast<float>(WinApp::clientWidth),
        static_cast<float>(WinApp::clientHeight)
    };

    const Vector2 position =
    {
        screenSize.x / 2.0f,
        screenSize.y / 2.0f
    };

    TextureManager::GetInstance()->LoadTexture("ToonRaider.png");
    TextureManager::GetInstance()->LoadTexture("ToonRaiderBordered.png");

    {
        pSpriteToonRaider1_ = std::make_unique<Sprite>();
        pSpriteToonRaider1_->Initialize("ToonRaider.png");
        const auto size = pSpriteToonRaider1_->GetSize();
        pSpriteToonRaider1_->SetSize(Vector2(size.x * screenRatio, size.y * screenRatio));
        pSpriteToonRaider1_->SetPos(position);
        pSpriteToonRaider1_->SetAnchorPoint(Vector2(0.5f, 0.5f));
        pSpriteToonRaider1_->SetColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    }

    {
        pSpriteToonRaider2_ = std::make_unique<Sprite>();
        pSpriteToonRaider2_->Initialize("ToonRaiderBordered.png");
        const auto size = pSpriteToonRaider2_->GetSize();
        pSpriteToonRaider2_->SetSize(Vector2(size.x * screenRatio, size.y * screenRatio));
        pSpriteToonRaider2_->SetPos(position);
        pSpriteToonRaider2_->SetAnchorPoint(Vector2(0.5f, 0.5f));
        pSpriteToonRaider2_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    pSpriteBackground_ = std::make_unique<Sprite>();
    pSpriteBackground_->Initialize("white.png");
    pSpriteBackground_->SetSize(screenSize);
    pSpriteBackground_->SetColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
}

void TitleScene::SpritesUpdate()
{
    pSpriteToonRaider1_->Update();
    pSpriteToonRaider2_->Update();
    pSpriteBackground_->Update();
}

void TitleScene::TerrainUpdate()
{
    uint32_t random = std::rand() % Terrain::kNumBlocks;
    terrain_->SetBlockColorAt(random, Block::Color::Orange);
    random = std::rand() % Terrain::kNumBlocks;
    terrain_->SetBlockColorAt(random, Block::Color::Purple);
    random = std::rand() % Terrain::kNumBlocks;
    terrain_->SetBlockColorAt(random, Block::Color::Blue);

    terrain_->Update();
}

void TitleScene::AnimationsInitialize()
{
    pTitleAnimation_ = std::make_unique<TitleAnimation>(
        pSpriteToonRaider1_.get(),
        pSpriteToonRaider2_.get(),
        pSpriteBackground_.get()
    );
    pTitleAnimation_->Initialize();
}

void TitleScene::AnimationsUpdate()
{
    pTitleAnimation_->Update();
}

