#include "Bar2d.h"
#include <TextureManager.h>
#include <imgui.h>

#define TO_VECTOR2(class) Vector2{(class).x, (class).y}
#define TO_VECTOR4(class) Vector4{(class).x, (class).y, (class).z, (class).w}

const Color Bar2d::COLOR_BAR_NORMAL = Color(0x2762e8ff);
const Color Bar2d::COLOR_BAR_BG = Color(0xabababff);
const Color Bar2d::COLOR_BAR_LOW = Color(0x2762e8ff);

const Vector2 Bar2d::SPACING_HEAD_TO_DECO = { 0.0f, 10.0f };

void Bar2d::Initialize(const std::string& _nameTexturePath, const Vector2& _barSize, bool _enable_smoothing_color)
{
    nameTexturePath_ = _nameTexturePath;
    barSize_ = _barSize;

    isDisplay_name_ = !nameTexturePath_.empty();
    isEnable_lerp_color_ = _enable_smoothing_color;

    // テクスチャの読み込み
    auto* tm = TextureManager::GetInstance();
    tm->LoadTexture(PATH_BAR);
    tm->LoadTexture(PATH_DECORATION);

    if (isDisplay_name_) tm->LoadTexture(nameTexturePath_);

    // スプライトの初期化
    bar_ = std::make_unique<Sprite>();
    bar_->Initialize(PATH_BAR);
    bar_->SetColor(COLOR_BAR_NORMAL.Vec4());

    for (auto& deco : decorations_)
    {
        deco = std::make_unique<Sprite>();
        deco->Initialize(PATH_DECORATION);
        deco->SetColor(COLOR_BAR_NORMAL.Vec4());
        Vector2 size = { barSize_.x, barSize_.y };
        float y = size.y + size.y / 5.0f;
        deco->SetSize({ y / 5.0f , y });
    }

    if (isDisplay_name_)
    {
        name_ = std::make_unique<Sprite>();
        name_->Initialize(nameTexturePath_);
    }

    background_ = std::make_unique<Sprite>();
    background_->Initialize(PATH_BAR);
    background_->SetColor(COLOR_BAR_BG.Vec4());
}

void Bar2d::Update()
{
    this->UpdateTransform();

    float t = currentValue_ - 0.1f / maxValue_ + 0.1f;
    if (t > 1.0f) t = 1.0f;
    if (t < 0.0f) t = 0.0f;

    this->UpdateColor();

    for (auto& deco : decorations_)
    {
        if (deco) deco->Update();
    }
    if (bar_) bar_->Update();
    if (background_) background_->Update();
    if (name_) name_->Update();
}

void Bar2d::Draw2D()
{
    for (const auto& deco : decorations_)
    {
        if (deco) deco->Draw();
    }
    if (background_) background_->Draw();
    if (bar_) bar_->Draw();
    if (name_) name_->Draw();
    for (const auto& number : numbers_)
    {
        if (number.second) number.second->Draw();
    }
}

void Bar2d::ImGui()
{
    bool isOpen = false;
    if (nameTexturePath_.empty())
    {
        isOpen = ImGui::Begin("Bar2d (No Name)");
    }
    else
    {
        isOpen = ImGui::Begin(nameTexturePath_.c_str());
    }

    if (isOpen)
    {
        ImGui::Text("Position");
        ImGui::DragFloat2("Position", &position_.x, 0.1f);
        ImGui::Text("Max Value");
        ImGui::DragFloat("Max Value", &maxValue_, 0.1f, FLT_MIN);
        ImGui::Text("Current Value");
        ImGui::DragFloat("Current Value", &currentValue_, 0.1f, FLT_MIN, maxValue_);
    }
    ImGui::End();
}

void Bar2d::UpdateTransform()
{
    Vector2 leftTop = position_ - mul(anchor_, barSize_);
    Vector2 cPos = leftTop;

    if (name_) 
    {
        name_->SetPos(TO_VECTOR2(cPos));
        cPos.y += name_->GetSize().y;
    }

    cPos += SPACING_HEAD_TO_DECO;
    decorations_[0]->SetPos(TO_VECTOR2(cPos));
    Vector2 decoSize = { decorations_[0]->GetSize().x, decorations_[0]->GetSize().y };
    cPos.x += decoSize.x * 3.0f;
    cPos.y += decoSize.y / 2.0f - bar_->GetSize().y / 2.0f;
    bar_->SetPos(TO_VECTOR2(cPos));

    float ratio = currentValue_ / maxValue_;
    bar_->SetSize({ barSize_.x * ratio, barSize_.y });

    background_->SetPos(TO_VECTOR2(cPos));
    background_->SetSize(TO_VECTOR2(barSize_));

    cPos.x += barSize_.x;
    cPos.x += decoSize.x * 2.0f;
    cPos.y -= decoSize.y / 2.0f - bar_->GetSize().y / 2.0f;

    decorations_[1]->SetPos(TO_VECTOR2(cPos));
}

void Bar2d::UpdateColor()
{
    if (isEnable_lerp_color_)
    {
        Vector4 color = {};
        color.Lerp(COLOR_BAR_LOW.Vec4(), COLOR_BAR_NORMAL.Vec4(), currentValue_ / maxValue_);
        bar_->SetColor(TO_VECTOR4(color));
    }
    else
    {
        if (currentValue_ < maxValue_ * BORDER_DANGER)
        {
            bar_->SetColor(COLOR_BAR_LOW.Vec4());
        }
        else
        {
            bar_->SetColor(COLOR_BAR_NORMAL.Vec4());
        }
    }
}

Vector2 Bar2d::mul(const Vector2& lv, const Vector2& rv)
{
    return Vector2(lv.x * rv.x, lv.y * rv.y);
}
