#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "Cell.hpp"
#include <algorithm>
#include <iterator>
#include <map>
#include <fstream>
#include <stdlib.h>
#include <cstdlib>
#include <string>


int main() {
  srand((unsigned)time(NULL));

  const int width = 900;
  const int height = 900;
  const std::string tilesFolder = "../../tiles/demo/"; // Looking from Build/[TYPE]/ directory

  bool allowSteps = false;
  bool nextStep = true;
  int stepCount = 0;

  const int DIM = 50;
  const float w = (float) width / DIM;
  const float h = (float) height / DIM;

  // create the window
  sf::RenderWindow window(sf::VideoMode(width, height), "Wave function collapse", sf::Style::Close);
  window.setFramerateLimit(75);

  // Load images and set rules (all going clockwise) //
  std::vector<Tile> tileMap {
    Tile(tilesFolder + "blank.png", { 0, 0, 0, 0 }),
    Tile(tilesFolder + "up.png",    { 1, 1, 0, 1 }),
    Tile(tilesFolder + "right.png", { 1, 1, 1, 0 }),
    Tile(tilesFolder + "down.png",  { 0, 1, 1, 1 }),
    Tile(tilesFolder + "left.png",  { 1, 0, 1, 1 }),
  };

  // Use any texture since all cells will have the same size
  const sf::Vector2u textureSize = tileMap.at(0).texture.getSize();
  const float scaleX = w / textureSize.x;
  const float scaleY = h / textureSize.y;

  // Generate the adjancency rules based on edges
  for (Tile& tile : tileMap)
    tile.setRules(tileMap);

  // Initialize cells
  std::vector<Cell> grid(DIM * DIM, Cell(scaleX, scaleY, tileMap.size()));

  // run the program as long as the window is open
  while (window.isOpen())
  {
    // check all the window's events that were triggered since the last iteration of the loop
    sf::Event event;
    while (window.pollEvent(event))
    {
        // "close requested" event: we close the window
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
            window.close();

        if (event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::J)) {
          if (allowSteps)
            nextStep = true;
        }

        if (event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::S))
          allowSteps = !allowSteps;

        if (event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
          grid = std::vector<Cell>(DIM * DIM, Cell(scaleX, scaleY, tileMap.size()));
          window.clear(sf::Color(31, 30, 31));
          stepCount = 0;
        }
    }

    // clear the window with black color
    window.clear(sf::Color(31, 30, 31));

    // draw everything here...
    if (nextStep) {
      if (allowSteps)
        nextStep = false;
      stepCount++;

      std::vector<Cell*> gridCopy;
      for (Cell& cell : grid) {
        if (!cell.isCollapsed())
          gridCopy.push_back(&cell);
      }

      if (gridCopy.size() > 1) {
        // Sort copy of the grid with least entrory
        std::sort(gridCopy.begin(), gridCopy.end(), [&](Cell* obj1, Cell* obj2) {
          return obj1->options.size() < obj2->options.size();
        });

        // Leave elements with the same (minimal) entropy //

        const int len = gridCopy[0]->options.size();
        int stopIndex = 0;
        for (int i = 1; i < gridCopy.size(); i++) {
          if (gridCopy[i]->options.size() > len) {
            stopIndex = i;
            break;
          }
        }

        if (stopIndex > 0)
          gridCopy.erase(gridCopy.begin() + stopIndex, gridCopy.end());

        ////////////////////////////////////////////////////

        // Pick random cell with least entropy and randomly set option to it
        gridCopy[random(0, gridCopy.size() - 1)]->setRandomOption();
      }

      std::vector<Cell> nextGrid(DIM * DIM);

      for (int j = 0; j < DIM; j++) {
        for (int i = 0; i < DIM; i++) {
          int index = i + j * DIM;
          Cell& cell = grid[index];

          if (cell.isCollapsed()) {
            nextGrid[index] = cell;

            if (!cell.isDrawn) {
              const sf::Texture& tileTexture = tileMap.at(cell.getLastOption()).texture;
              cell.sprite.setTexture(tileTexture);
              cell.sprite.setPosition(i * w, j * h);
              cell.isDrawn = true;

              window.draw(cell.sprite);
            }
          } else {
            std::vector<int> currOptions(tileMap.size());
            std::iota(currOptions.begin(), currOptions.end(), 0);

            // Look above
            if (j > 0) {
              Cell above = grid[i + (j - 1) * DIM];
              std::vector<int> validOptions;

              for (int option : above.options) {
                std::vector<int> valid = tileMap.at(option).under;
                std::copy(valid.begin(), valid.end(), std::inserter(validOptions, validOptions.end()));
              }
              Cell::validateOptions(currOptions, validOptions);
            }

            // Look right
            if (i < DIM - 1) {
              Cell right = grid[i + 1 + j * DIM];
              std::vector<int> validOptions;

              for (int option : right.options) {
                std::vector<int> valid = tileMap[option].left;
                std::copy(valid.begin(), valid.end(), std::inserter(validOptions, validOptions.end()));
              }
              Cell::validateOptions(currOptions, validOptions);
            }

            // Look under
            if (j < DIM - 1) {
              Cell under = grid[i + (j + 1) * DIM];
              std::vector<int> validOptions;

              for (int option : under.options) {
                std::vector<int> valid = tileMap[option].above;
                std::copy(valid.begin(), valid.end(), std::inserter(validOptions, validOptions.end()));
              }
              Cell::validateOptions(currOptions, validOptions);
            }

            // Look left
            if (i > 0) {
              Cell left = grid[i - 1 + j * DIM];
              std::vector<int> validOptions;

              for (int option : left.options) {
                std::vector<int> valid = tileMap[option].right;
                std::copy(valid.begin(), valid.end(), std::inserter(validOptions, validOptions.end()));
              }
              Cell::validateOptions(currOptions, validOptions);
            }

            nextGrid[index] = Cell(scaleX, scaleY, currOptions);
          }
        }
      }

      grid = nextGrid;
    }

    // end the current frame
    window.display();
  }

  return 0;
}

