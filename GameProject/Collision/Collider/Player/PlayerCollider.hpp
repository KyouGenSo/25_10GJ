#ifndef PlayerCollider_HPP_
#define PlayerCollider_HPP_
#include "AABBCollider.h"

class Player;

class PlayerCollider final : public AABBCollider{
    Player* player_ = nullptr;
public:
    PlayerCollider(Player* _owner);
    void OnCollisionStay(Collider* other) override;
    void OnCollisionExit(Collider* other) override;

private:

}; // class PlayerCollider

#endif // PlayerCollider_HPP_
