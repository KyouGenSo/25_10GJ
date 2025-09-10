#include "PlayerCollider.hpp"

#include "Collision/CollisionTypeIdDef.h"
#include "Object/Player/Player.h"
#include "Terrain/Block/Block.h"

PlayerCollider::PlayerCollider(Player* _owner)
    :player_(_owner){
    SetOwner(player_);
}

void PlayerCollider::OnCollisionEnter(Collider* other) {
    if (other->GetTypeID() != static_cast<uint32_t>(CollisionTypeId::kTerrain))return;
    
    Block* block = static_cast<Block*>(other->GetOwner());
    auto blockTransform = other->GetTransform();
    auto playerTransform = player_->GetTransform();
    
    float blockTop = blockTransform->translate.y + Block::kScale / 2.f;
    float blockBottom = blockTransform->translate.y - Block::kScale / 2.f;
    float playerBottom = playerTransform.translate.y - player_->kSize / 2.f;
    float playerTop = playerTransform.translate.y + player_->kSize / 2.f;
    
    // 床との衝突判定（プレイヤーがブロックの上にいる場合）
    if (playerBottom <= blockTop && playerBottom > blockBottom) {
        // 床の処理
        playerTransform.translate.y = blockTop + player_->kSize / 2.f;
        player_->SetTransform(playerTransform);
        player_->OnGround();
    }
    // 壁との衝突判定（プレイヤーがブロックの横にいる場合）
    else if (playerTop > blockBottom && playerBottom < blockTop) {
        // 壁の処理 - XZ平面での押し戻し
        float blockLeft = blockTransform->translate.x - block->kScale / 2.f;
        float blockRight = blockTransform->translate.x + block->kScale / 2.f;
        float blockFront = blockTransform->translate.z - block->kScale / 2.f;
        float blockBack = blockTransform->translate.z + block->kScale / 2.f;
        
        float playerLeft = playerTransform.translate.x - player_->kSize / 2.f;
        float playerRight = playerTransform.translate.x + player_->kSize / 2.f;
        float playerFront = playerTransform.translate.z - player_->kSize / 2.f;
        float playerBack = playerTransform.translate.z + player_->kSize / 2.f;
        
        // X軸方向の重複量
        float overlapX = 0.0f;
        if (playerRight > blockLeft && playerLeft < blockRight) {
            float rightOverlap = playerRight - blockLeft;
            float leftOverlap = blockRight - playerLeft;
            overlapX = (rightOverlap < leftOverlap) ? rightOverlap : -leftOverlap;
        }
        
        // Z軸方向の重複量
        float overlapZ = 0.0f;
        if (playerBack > blockFront && playerFront < blockBack) {
            float backOverlap = playerBack - blockFront;
            float frontOverlap = blockBack - playerFront;
            overlapZ = (backOverlap < frontOverlap) ? backOverlap : -frontOverlap;
        }
        
        // より小さい重複量の方向に押し戻し
        if (abs(overlapX) < abs(overlapZ)) {
            playerTransform.translate.x -= overlapX;
        } else {
            playerTransform.translate.z -= overlapZ;
        }
        
        player_->SetTransform(playerTransform);
    }
}

void PlayerCollider::OnCollisionExit(Collider* other) {
    (void) other;
    player_->OffGround();
}