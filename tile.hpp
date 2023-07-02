#include "SFML/Graphics.hpp"
#include <string>

struct Tile {
  public:
    Tile(const std::string& path) {
      this->texture.loadFromFile(path);
      this->sprite.setTexture(this->texture);
    }

    ~Tile() {}

    const sf::Sprite& get() const {
      return this->sprite;
    }

    const int width() const {
      return texture.getSize().x;
    }

    const int height() const {
      return texture.getSize().y;
    }

    void setPosition(const float& x, const float& y) {
      this->sprite.setPosition(x, y);
    }

  private:
    sf::Texture texture;
    sf::Sprite sprite;
};

