#include "HealPotion.h"
#include "Object.h"
#include "Entity.h"
// ----------------- HealPotion Implementation -----------------

HealPotion::HealPotion(const sf::Vector2i& pos, int amount, const std::string& texturePath) 
    : Object(pos, "Healing Potion", amount, texturePath), healAmount(amount) {}

bool HealPotion::use(Entity& entity) {
    if (collected) {
        entity.hp += healAmount;
        return true; // Successfully used
    }
    return false; // Not collected yet
}

int HealPotion::getHealAmount() const { 
    return healAmount; 
}

std::string HealPotion::getDescription() const {
    return name + " (+" + std::to_string(healAmount) + " HP)";
}