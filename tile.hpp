#include <string>
#include "SFML/Graphics.hpp"
#include "TileType.hpp"

struct Tile {
  public:
    sf::Texture texture;

    Tile(const std::string &path){
      this->texture.loadFromFile(path);
    }
};

