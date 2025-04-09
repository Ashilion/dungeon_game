#include "Object.h"
#include <random>

class Key : public Object {
public:
    Key(const sf::Vector2i& pos, const std::string& texturePath = "assets/key.jpg");

    static void generateKey(std::vector<std::vector<int>>& grid, std::vector<Object*>& worldObjects, std::mt19937& rng);

    std::string getDescription() const override;

    bool use(Entity& entity) override;
};