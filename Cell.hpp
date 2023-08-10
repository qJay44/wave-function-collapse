#include "Tile.hpp"
#include <numeric>
#include <deque>

#define RANDOM_COLLAPSE 0
#define VALIDATE_COLLAPSE 1

class Cell {
  static float scaleX;
  static float scaleY;
  static int optionsSize;
  static sf::Font font;

  sf::Sprite sprite;
  sf::Text optionsText;
  std::vector<int> options;
  std::map<int, Cell*> neighbours;

  int index;

  public:
    static bool zeroOptionsLeft;

    Cell() {
      sprite.setScale(scaleX, scaleY);
      options = std::vector<int>(optionsSize);
      std::iota(options.begin(), options.end(), 0);

      optionsText.setFont(font);
      optionsText.setOutlineColor(sf::Color(31, 30, 31));
      optionsText.setOutlineThickness(3.f);
    }

    static void setCoreValues(
      float scaleX,
      float scaleY,
      int optionsSize,
      sf::Font font
    ) {
      Cell::scaleX = scaleX;
      Cell::scaleY = scaleY;
      Cell::optionsSize = optionsSize;
      Cell::font = font;
    }

    void validateOptions(const std::vector<Tile>& tiles) {
      for (const auto& [direction, neighbour] : neighbours) {
        if (neighbour->getEntropy() == tiles.size()) continue;

        std::set<int> validOptions;
        for (int option : neighbour->options) {
          std::set<int> valid = tiles[option].getSideOptions((direction + 2) % 4);
          std::copy(valid.begin(), valid.end(), std::inserter(validOptions, validOptions.end()));
        }

        // Leave only valid opitons
        options.erase(std::remove_if(options.begin(), options.end(),
          [&validOptions] (int opt) {
            return std::find(validOptions.begin(), validOptions.end(), opt) == validOptions.end();
          }
        ), options.end());

        if (zeroOptionsLeft)
          zeroOptionsLeft = true;
      }
    }

    void setPosition(int i, int j, float w, float h, int DIM) {
      index = i + j * DIM;
      float x = (i + 0.5f) * w;
      float y = (j + 0.5f) * h;

      sprite.setPosition({ x, y });
      optionsText.setPosition({ x, y });
      optionsText.setCharacterSize(w * 0.5f);
    }

    void setNeighbours(std::map<int, Cell*> neighbours) {
      if (this->neighbours.size() != 0)
        throw std::runtime_error("Already has neighbours");

      this->neighbours = neighbours;
    }

    void setRandomOption(bool forced = false) {
      if (isCollapsed() && !forced)
        throw std::runtime_error("Cell already collapsed");

      int option = random(options);
      options.clear();
      options.push_back(option);
    }

    const sf::Sprite& prepareSprite(const Tile& tile) {
      sprite.setTexture(tile.getTexture());
      sf::FloatRect textRect = sprite.getLocalBounds();
      sprite.setOrigin(
        textRect.left + textRect.width / 2.f,
        textRect.top  + textRect.height / 2.f
        );
      sprite.setRotation(tile.getRotation());

      return sprite;
    }

    const sf::Text& prepareText() {
      optionsText.setString(std::to_string(options.size()));
      sf::FloatRect textRect = optionsText.getLocalBounds();
      optionsText.setOrigin(
        textRect.left + textRect.width / 2.f,
        textRect.top  + textRect.height / 2.f
      );

      return optionsText;
    }

    void reset() {
      options = std::vector<int>(optionsSize);
      std::iota(options.begin(), options.end(), 0);

      sf::Vector2f pos = sprite.getPosition();
      sprite = sf::Sprite();
      sprite.setScale(scaleX, scaleY);
      sprite.setPosition(pos);
    }

    const int getSingleOption() const {
      if (!isCollapsed())
        throw std::runtime_error("Cell is not collapsed");

      return *options.begin();
    }

    const bool isCollapsed() const {
      if (options.size() == 0)
        zeroOptionsLeft = true;

      return options.size() == 1;
    }

    const bool isDrawn() const {
      return sprite.getTexture();
    }

    const int getEntropy() const {
      return options.size();
    }

    const std::vector<int> peekOptions() const {
      return options;
    }

    const std::map<int, std::vector<int>> peekNeighourOptions() const {
      std::map<int, std::vector<int>> neighboursOpts;

      for (const auto& neighbour : neighbours)
        neighboursOpts[neighbour.first] = neighbour.second->peekOptions();

      return neighboursOpts;
    }
};

