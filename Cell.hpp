#include "Tile.hpp"
#include <numeric>
#include <stdexcept>
#include <string>

class Cell {
  public:
    sf::Sprite sprite;
    std::vector<int> options;
    bool isDrawn = false;

    Cell(const float& scaleX, const float& scaleY, const int& optionsSize) {
      sprite.setScale(scaleX, scaleY);

      options = std::vector<int>(optionsSize);
      std::iota(options.begin(), options.end(), 0);
    }

    void checkNeighbours(
      const int i,
      const int j,
      const int DIM,
      std::vector<Cell>& grid,
      std::vector<Tile>& tileMap
    ) {
      // Look above
      if (j > 0) {
        const Cell& above = grid[i + (j - 1) * DIM];
        this->validateOptions(above, tileMap, "under");
      }

      // Look right
      if (i < DIM - 1) {
        const Cell& right = grid[i + 1 + j * DIM];
        this->validateOptions(right, tileMap, "left");
      }
      // Look under
      if (j < DIM - 1) {
        const Cell& under = grid[i + (j + 1) * DIM];
        this->validateOptions(under, tileMap, "above");
      }

      // Look left
      if (i > 0) {
        const Cell& left = grid[i - 1 + j * DIM];
        this->validateOptions(left, tileMap, "right");
      }
    }

    void setRandomOption() {
      if (collapsed)
        throw std::runtime_error("Cell already collapsed");

      const int option = options[random(0, options.size() - 1)];
      options.clear();
      options.push_back(option);
      collapsed = true;
    }

    const int getSingleOption() const {
      if (!collapsed || options.size() > 1)
        throw std::runtime_error("Cell is not collapsed");

      return *options.begin();
    }

    const bool& isCollapsed() const {
      return collapsed;
    }

  private:
    bool collapsed = false;

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
};

