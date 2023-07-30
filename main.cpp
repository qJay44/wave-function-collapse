#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "Cell.hpp"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <fstream>
#include <memory>
#include <stdlib.h>
#include <cstdlib>
#include <string>


int main() {
  srand((unsigned)time(NULL));

  const std::string pathPrefix = "../../tiles/"; // Looking from "Build/[TYPE]/" directory
  const std::vector<std::string> tilesFolders {
    "demo/", "demo-tracks/", "mountains/", "pipes/", "polka/", "roads/", "train-tracks/"
  };
  const std::vector<std::string> directions {
    "blank.png", "up.png", "right.png", "down.png", "left.png"
  };
  std::string folder = pathPrefix + tilesFolders[0];

  const int width = 900;
  const int height = 900;

  bool allowSteps = false;
  bool nextStep = true;
  bool changeTilesFolderOnRestart = false;

  const int DIM = 80;
  const float w = (float) width / DIM;
  const float h = (float) height / DIM;

  // create the window
  sf::RenderWindow window(sf::VideoMode(width, height), "Wave function collapse", sf::Style::Close);
  window.setFramerateLimit(75);

  sf::RenderTexture renderTexture;
  renderTexture.create(width, height);

  sf::Font font;
  font.loadFromFile("Minecraft rus.ttf");

  // Get the render texture and make sprite of it
  const sf::Texture &canvasTexture = renderTexture.getTexture();
  sf::Sprite canvasSprite(canvasTexture);

  // Load images and set rules (all going clockwise) //
  std::vector<Tile> tiles {
    Tile(folder + directions[0], { 0, 0, 0, 0 }),
    Tile(folder + directions[1], { 1, 1, 0, 1 }),
    Tile(folder + directions[2], { 1, 1, 1, 0 }),
    Tile(folder + directions[3], { 0, 1, 1, 1 }),
    Tile(folder + directions[4], { 1, 0, 1, 1 }),
  };

  // Use any texture since all cells will have the same size
  sf::Vector2u textureSize = tiles.at(0).texture.getSize();
  float scaleX = w / textureSize.x;
  float scaleY = h / textureSize.y;

  // Generate the adjancency rules based on edges
  for (Tile& tile : tiles)
    tile.setRules(tiles);

  // Initialize cells
  std::vector<Cell> grid;
  grid.reserve(DIM * DIM);
  std::generate_n(std::back_inserter(grid), DIM * DIM, [&] {
    return Cell(scaleX, scaleY, tiles.size());
  });

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

        if (event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::N)) {
          if (allowSteps)
            nextStep = true;
        }

        if (event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::S))
          allowSteps = !allowSteps;

        if (event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
          // Change texture set
          if (changeTilesFolderOnRestart) {
            folder = pathPrefix + tilesFolders[random(0, tilesFolders.size() - 1)];
            for (int i = 0; i < tiles.size(); i++)
              tiles[i].changeTexture(folder + directions[i]);
          }

          // Change texture scales (size)
          textureSize = tiles.at(0).texture.getSize();
          scaleX = w / textureSize.x;
          scaleY = h / textureSize.y;

          // Clear grid
          grid = std::vector<Cell>(DIM * DIM, Cell(scaleX, scaleY, tiles.size()));
          grid.reserve(DIM * DIM);
          std::generate_n(std::back_inserter(grid), DIM * DIM, [&] {
            return Cell(scaleX, scaleY, tiles.size());
          });

          // Clear window
          renderTexture.clear(sf::Color(31, 30, 31));
        }

        if (event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::I))
          changeTilesFolderOnRestart = !changeTilesFolderOnRestart;
    }

    // draw everything here...
    if (nextStep) {
      if (allowSteps)
        nextStep = false;

      std::vector<Cell*> leastEntropyCells;

      int lastSize = tiles.size();
      for (Cell& cell : grid) {
        const int currentSize = cell.options.size();

        // Add cells with least entropy
        if (!cell.isCollapsed()) {
          if (leastEntropyCells.size() == 0 || currentSize < lastSize) {
            lastSize = currentSize;
            leastEntropyCells.clear();
            leastEntropyCells.push_back(&cell);
          }
          else if (currentSize == lastSize)
            leastEntropyCells.push_back(&cell);
        }
      }

      if (leastEntropyCells.size() > 0)
        leastEntropyCells[random(0, leastEntropyCells.size() - 1)]->setRandomOption();

      for (int j = 0; j < DIM; j++) {
        for (int i = 0; i < DIM; i++) {
          const int index = i + j * DIM;
          Cell& cell = grid[index];

          // Draw collapsed cells and reduce options from cells around it //
          if (cell.isCollapsed() && !cell.isDrawn) {
            const sf::Texture& tileTexture = tiles.at(cell.getSingleOption()).texture;
            cell.sprite.setTexture(tileTexture);
            cell.sprite.setPosition(i * w, j * h);
            cell.isDrawn = true;

            renderTexture.draw(cell.sprite);

            // Look above
            if (j > 0) {
              const int iAbove = i;
              const int jAbove = j - 1;
              Cell& above = grid[iAbove + jAbove * DIM];

              above.validateOptions(cell, tiles, "above");
            }

            // Look right
            if (i < DIM - 1) {
              const int iRight = i + 1;
              const int jRight = j;
              Cell& right = grid[iRight + jRight * DIM];

              right.validateOptions(cell, tiles, "right");
            }

            // Look under
            if (j < DIM - 1) {
              const int iUnder = i;
              const int jUnder = j + 1;
              Cell& under = grid[iUnder + jUnder * DIM];

              under.validateOptions(cell, tiles, "under");
            }

            // Look left
            if (i > 0) {
              const int iLeft = i - 1;
              const int jLeft = j;
              Cell& left = grid[iLeft + jLeft * DIM];

              left.validateOptions(cell, tiles, "left");
            }
          }
        }
      }

    ////////////////////////////////////////////////////////
    }

    renderTexture.display();

    window.clear(sf::Color(31, 30, 31));
    window.draw(canvasSprite);
    window.display();
  }

  return 0;
}

