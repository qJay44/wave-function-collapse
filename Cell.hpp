#include "tile.hpp"
#include "utils.hpp"
#include <stdexcept>
#include <vector>
#include <iostream>

class Cell {
  public:
    sf::Sprite sprite;
    std::vector<TileType> options {
      TileType::BLANK,
      TileType::DOWN,
      TileType::LEFT,
      TileType::RIGHT,
      TileType::UP,
    };

    Cell() {}

    Cell(const float& scaleX, const float& scaleY) {
      sprite.setScale(scaleX, scaleY);
    }

    Cell(
      const float& scaleX,
      const float& scaleY,
      std::vector<TileType> options,
      bool collapsed = false
    ) : options(options), collapsed(collapsed) {
      sprite.setScale(scaleX, scaleY);
    }

    static void validateOptions(std::vector<TileType>& currOptions, std::vector<TileType>& validOptions) {
      for (TileType opt : currOptions) {
        if (std::find(validOptions.begin(), validOptions.end(), opt) == validOptions.end())
          currOptions.erase(std::remove(currOptions.begin(), currOptions.end(), opt), currOptions.end());
      }
    }

    void setRandomOption() {
      TileType option = options[random(0, options.size() - 1)];
      options.clear();
      options.push_back(option);
      collapsed = true;
    }

    void printOptionsSize() const {
      std::cout << options.size() << "\n";
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
};

