#include "Tile.hpp"
#include <numeric>
#include <stdexcept>
#include <string>

class Cell {
  public:
    sf::Sprite sprite;
    std::vector<int> options;
    bool isDrawn = false;

    Cell() {}

    Cell(const float& scaleX, const float& scaleY, const int& optionsSize) {
      sprite.setScale(scaleX, scaleY);

      options = std::vector<int>(optionsSize);
      std::iota(options.begin(), options.end(), 0);
    }

    Cell(
      const float& scaleX,
      const float& scaleY,
      std::vector<int> options,
      bool collapsed = false
    ) : options(options), collapsed(collapsed) {
      sprite.setScale(scaleX, scaleY);
    }

    void validateOptions(
        const Cell& neighbour,
        const std::vector<Tile>& tiles,
        const std::string side
    ) {
      std::vector<int> validOptions;

      // Get valid options
      for (int option : neighbour.options) {
        std::vector<int> valid = tiles[option].sides.at(side);
        std::copy(valid.begin(), valid.end(), std::inserter(validOptions, validOptions.end()));
      }

      // Leave only valid opitons
      options.erase(std::remove_if(options.begin(), options.end(), [&](int opt) {
        return std::count(validOptions.begin(), validOptions.end(), opt) == 0;
      }), options.end());
    }

    void setRandomOption() {
      const int option = options[random(0, options.size() - 1)];
      options.clear();
      options.push_back(option);
      collapsed = true;
    }

    const int getSingleOption() const {
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

