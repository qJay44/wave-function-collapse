#include "tile.hpp"
#include <stdexcept>
#include <vector>

struct Cell {
  public:
    static sf::Sprite originalSprite;

    static void setSize(
        const float& width,
        const float& height,
        const sf::Vector2u& textureSize
      ) {
      const float spriteScaledWidth = width / textureSize.x;
      const float spriteScaledHeight = height / textureSize.y;

      originalSprite.setScale(spriteScaledWidth, spriteScaledHeight);
    }

    Cell() {
      this->sprite = originalSprite;
    }

    const sf::Sprite& setTexture(const sf::Texture &texture) {
      this->sprite.setTexture(texture);

      return this->sprite;
    }

    void removeValue(const TileType &type) {
      if (this->options.size() == 1)
        this->collapsed = true;

      options.erase(
          std::remove(options.begin(), options.end(), type),
          options.end()
      );
    }

    const TileType getLastOption() const {
      if (this->options.size() > 1)
        throw std::runtime_error("There are more than one option to get");

      return options[0];
    }

    const bool& isCollapsed() const {
      return this->collapsed;
    }

  private:
    bool collapsed = false;
    sf::Sprite sprite;
    std::vector<TileType> options {
      TileType::BLANK,
      TileType::DOWN,
      TileType::LEFT,
      TileType::RIGHT,
      TileType::UP,
    };
};

