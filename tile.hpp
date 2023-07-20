#include "SFML/Graphics.hpp"
#include "utils.hpp"

class Tile {
  public:
    sf::Texture texture;
    const std::vector<std::vector<TileType>> rules;

    Tile() {}

    Tile(const std::string &path, const std::vector<std::vector<TileType>>& rules)
      : rules(rules) {
      this->texture.loadFromFile(path);
    }
};

