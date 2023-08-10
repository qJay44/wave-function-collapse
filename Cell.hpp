#include "Tile.hpp"
#include <numeric>
#include <deque>

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

    Cell() {
      sprite.setScale(scaleX, scaleY);
      options = std::vector<int>(optionsSize);
      std::iota(options.begin(), options.end(), 0);

      optionsText.setCharacterSize(30);
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

    void validateOptions(
      const std::vector<Tile>& tiles,
      std::deque<int>& collapsedIndeces
    ) {
      for (const auto& [direction, neighbour] : neighbours) {
        if (neighbour->isCollapsed()) continue;

        std::vector<int>& neighbourOptions = neighbour->options;
        std::set<int> validOptions = tiles[getSingleOption()].getSideOptions(direction);

        // Leave only valid opitons
        neighbourOptions.erase(std::remove_if(neighbourOptions.begin(), neighbourOptions.end(),
          [&validOptions] (int opt) {
            return std::find(validOptions.begin(), validOptions.end(), opt) == validOptions.end();
          }
        ), neighbourOptions.end());

        if (neighbour->isCollapsed())
          collapsedIndeces.push_back(neighbour->index);
      }
    }

    void setPosition(int i, int j, float w, float h, int DIM) {
      index = i + j * DIM;
      float x = (i + 0.5f) * w;
      float y = (j + 0.5f) * h;

      sprite.setPosition({ x, y });
      optionsText.setPosition({ x, y });
    }

    void setNeighbours(std::map<int, Cell*> neighbours) {
      if (this->neighbours.size() != 0)
        throw std::runtime_error("Already has neighbours");

      this->neighbours = neighbours;
    }

    void setRandomOption(bool forced = false) {
      if (isCollapsed() && !forced)
        throw std::runtime_error("Cell already collapsed");

      int option = options[random(0, options.size() - 1)];
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
      optionsText.setScale(scaleX / 2.f, scaleY / 2.f);
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
        throw std::runtime_error("0 options left");

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

