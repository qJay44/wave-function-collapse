#include "tile.hpp"
#include <stdexcept>
#include <vector>

struct Cell {
  public:
    sf::Sprite sprite;

    Cell() {}

    Cell(const float& scaleX, const float& scaleY) {
      sprite.setScale(scaleX, scaleY);
    }

    void temp() {
      options.pop_back();
      if (options.size() == 1)
        collapsed = true;
    }

    void setSingleOption(TileType option) {
      options.clear();
      options.push_back(option);
      collapsed = true;
    }

    const int getOptionsSize() const {
      return options.size();
    }

    const TileType getLastOption() const {
      if (options.size() > 1)
        throw std::runtime_error("There are more than one option to get");

      return options[0];
    }

    const bool& isCollapsed() const {
      return collapsed;
    }

  private:
    bool collapsed = false;
    std::vector<TileType> options {
      TileType::BLANK,
      TileType::DOWN,
      TileType::LEFT,
      TileType::RIGHT,
      TileType::UP,
    };
};

