#ifndef PlayerCollider_HPP_
#define PlayerCollider_HPP_
#include "AABBCollider.h"
#include "Object/Player/Player.h"


class PlayerCollider final : public AABBCollider{
    Player* owner_ = nullptr;
public:
    PlayerCollider(Player* _owner);
    void OnCollisionStay(Collider* other) override;

private:

}; // class PlayerCollider

#endif // PlayerCollider_HPP_
