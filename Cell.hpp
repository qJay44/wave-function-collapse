#include "tile.hpp"
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

    static void validateOptions(std::vector<int>& currOptions, std::vector<int>& validOptions) {
      currOptions.erase(std::remove_if(currOptions.begin(), currOptions.end(), [&](int opt) {
        return std::count(validOptions.begin(), validOptions.end(), opt) == 0;
      }), currOptions.end());
    }

    void setRandomOption() {
      int option = *next(options.begin(), random(0, options.size() - 1));
      options.clear();
      options.push_back(option);
      collapsed = true;
    }

    const int getLastOption() const {
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

