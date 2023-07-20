#include "tile.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>
#include <set>

class Cell {
  public:
    sf::Sprite sprite;
    std::set<TileType> options {
      TileType::BLANK,
      TileType::DOWN,
      TileType::LEFT,
      TileType::RIGHT,
      TileType::UP,
    };
    bool isDrawn = false;

    Cell() {}

    Cell(const float& scaleX, const float& scaleY) {
      sprite.setScale(scaleX, scaleY);
    }

    Cell(
      const float& scaleX,
      const float& scaleY,
      std::set<TileType> options,
      bool collapsed = false
    ) : options(options), collapsed(collapsed) {
      sprite.setScale(scaleX, scaleY);
    }

    static void validateOptions(std::set<TileType>& currOptions, std::set<TileType>& validOptions) {
      for (TileType opt : currOptions) {
        if (validOptions.find(opt) == validOptions.end())
          currOptions.erase(opt);
      }
    }

    void setRandomOption() {
      TileType option = *next(options.begin(), random(0, options.size() - 1));
      options.clear();
      options.insert(option);
      collapsed = true;
    }

    const TileType getLastOption() const {
      if (options.size() > 1)
        throw std::runtime_error("There are more than one option to get");

      return *options.begin();
    }

    const bool& isCollapsed() const {
      return collapsed;
    }

  private:
    bool collapsed = false;
};

