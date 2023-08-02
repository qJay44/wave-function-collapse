#include "Tile.hpp"
#include <numeric>
#include <stdexcept>
#include <string>

class Cell {
  public:
    bool checkedNeighbours = false;

    Cell(
      const float& scaleX,
      const float& scaleY,
      const int& optionsSize,
      sf::Font& font
    ) {
      sprite.setScale(scaleX, scaleY);
      options = std::vector<int>(optionsSize);
      std::iota(options.begin(), options.end(), 0);

      optionsText.setFont(font);
      optionsText.setScale(scaleX, scaleY);
      optionsText.setOutlineColor(sf::Color(31, 30, 31));
      optionsText.setOutlineThickness(3.f);
    }

    void validateOptions(
        const Cell& neighbour,
        const std::vector<Tile>& tiles,
        const std::string side
    ) {
      if (!isCollapsed()) {
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
    }

    void setRandomOption(bool forced = false) {
      if (isCollapsed() && !forced)
        throw std::runtime_error("Cell already collapsed");

      const int option = options[random(0, options.size() - 1)];
      options.clear();
      options.push_back(option);
    }

    const sf::Sprite* prepareSprite(const sf::Texture& texture, float x, float y) {
      sprite.setTexture(texture);
      sprite.setPosition(x, y);

      return &sprite;
    }

    sf::Text* updateText(float x, float y) {
      sf::FloatRect textRect = optionsText.getLocalBounds();
      optionsText.setOrigin(
        textRect.left + textRect.width / 2.f,
        textRect.top  + textRect.height / 2.f
      );
      optionsText.setCharacterSize(30);
      optionsText.setPosition(x, y);
      optionsText.setString(std::to_string(options.size()));

      return &optionsText;
    }

    const int getSingleOption() const {
      if (!isCollapsed())
        throw std::runtime_error("Cell is not collapsed");

      return *options.begin();
    }

    const bool isCollapsed() const {
      return options.size() == 1;
    }

    const bool isDrawn() const {
      return sprite.getTexture() != NULL;
    }

    const int getOptionsSize() const {
      return options.size();
    }

  private:
    sf::Sprite sprite;
    std::vector<int> options;
    sf::Text optionsText;
};

