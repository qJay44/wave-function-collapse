#include <string>
#include "SFML/Graphics.hpp"
#include "TileType.hpp"

struct Tile {
  public:
    const sf::Texture *texturePtr = &texture;

    Tile(const std::string &path){
      this->texture.loadFromFile(path);
    }

  private:
    sf::Texture texture;
};

