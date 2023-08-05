#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "Cell.hpp"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <fstream>
#include <deque>
#include <stdlib.h>
#include <cstdlib>


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
  bool showOptionsText = false;

  const int DIM = 50;
  const float w = (float) width / DIM;
  const float h = (float) height / DIM;

  // create the window
  sf::RenderWindow window(sf::VideoMode(width, height), "Wave function collapse", sf::Style::Close);
  window.setFramerateLimit(75);

  sf::RenderTexture renderTexture;
  renderTexture.create(width, height);

  // Get the render texture and make sprite of it
  const sf::Texture &canvasTexture = renderTexture.getTexture();
  sf::Sprite canvasSprite(canvasTexture);

  sf::RenderTexture renderTextureSecondary;
  renderTextureSecondary.create(width, height);

  // Get the render texture and make sprite of it
  const sf::Texture &canvasTextureSecondary = renderTextureSecondary.getTexture();
  sf::Sprite canvasSpriteSecondary(canvasTextureSecondary);

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

  sf::Font font;
  font.loadFromFile("Minecraft rus.ttf");

  // Initialize cells
  std::vector<Cell> grid;
  grid.reserve(DIM * DIM);
  std::generate_n(std::back_inserter(grid), DIM * DIM, [&] {
    return Cell(scaleX, scaleY, tiles.size(), font);
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

        else if (event.type == sf::Event::KeyPressed) {
          if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
              window.close();

          if (sf::Keyboard::isKeyPressed(sf::Keyboard::N)) {
            if (allowSteps)
              nextStep = true;
          }

          if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            allowSteps = !allowSteps;
            nextStep = true;
          }

          if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
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
            grid = std::vector<Cell>(DIM * DIM, Cell(scaleX, scaleY, tiles.size(), font));
            grid.reserve(DIM * DIM);
            std::generate_n(std::back_inserter(grid), DIM * DIM, [&] {
              return Cell(scaleX, scaleY, tiles.size(), font);
            });

            // Clear window
            renderTexture.clear(sf::Color(31, 30, 31));
          }

          if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
            changeTilesFolderOnRestart = !changeTilesFolderOnRestart;

          if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
            showOptionsText = !showOptionsText;

        } else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
              int i = event.mouseButton.x / w;
              int j = event.mouseButton.y / h;

              grid[i + j * DIM].setRandomOption(true);
              nextStep = true;
            }
        }
    }

    // draw everything here...
    if (nextStep) {
      if (allowSteps)
        nextStep = false;

      renderTextureSecondary.clear(sf::Color(31, 30, 31, 0));

      std::deque<std::vector<int>> collapsedCells;
      std::vector<Cell*> leastEntropyCells;
      std::vector<std::vector<int>> leastEntropyCellsIndex;

      int lastSize = tiles.size();
      for (int j = 0; j < DIM; j++) {
        for (int i = 0; i < DIM; i++) {
          Cell& cell = grid[i + j * DIM];

          // Add cells with least entropy
          if (!cell.isCollapsed()) {
            int currentSize = cell.getOptionsSize();

            if (leastEntropyCells.size() == 0 || currentSize < lastSize) {
              lastSize = currentSize;
              leastEntropyCells.clear();
              leastEntropyCellsIndex.clear();
              leastEntropyCells.push_back(&cell);
              leastEntropyCellsIndex.push_back({ i, j });
            }
            else if(currentSize == lastSize) {
              leastEntropyCells.push_back(&cell);
              leastEntropyCellsIndex.push_back({ i, j });
            }

            if (showOptionsText && cell.getOptionsSize() < 5)
              renderTextureSecondary.draw(*cell.updateText(w * (i + 0.5f), h * (j + 0.5f)));
          }
        }
      }

      if (leastEntropyCells.size() > 0) {
        int pickIndex = random(0, leastEntropyCells.size() - 1);
        leastEntropyCells[pickIndex]->setRandomOption();
        collapsedCells.push_back(leastEntropyCellsIndex[pickIndex]);
      }

      auto validator = [&] (Cell& cell, int i, int j) {
        // Look above
        if (j > 0) {
          Cell& above = grid[i + (j - 1) * DIM];

          if (above.validateOptions(cell, tiles, "above"))
            collapsedCells.push_back({ i, j - 1 });
        }

        // Look right
        if (i < DIM - 1) {
          Cell& right = grid[i + 1 + j * DIM];

          if (right.validateOptions(cell, tiles, "right"))
            collapsedCells.push_back({ i + 1, j });
        }

        // Look under
        if (j < DIM - 1) {
          Cell& under = grid[i + (j + 1) * DIM];

          if (under.validateOptions(cell, tiles, "under"))
            collapsedCells.push_back({ i, j + 1 });
        }

        // Look left
        if (i > 0) {
          Cell& left = grid[i - 1 + j * DIM];

          if (left.validateOptions(cell, tiles, "left"))
            collapsedCells.push_back({ i - 1, j });
        }
      };

      while (!collapsedCells.empty()) {
        std::vector<int> collapsedCellIndex = collapsedCells.back();
        collapsedCells.pop_back();

        int i = collapsedCellIndex[0];
        int j = collapsedCellIndex[1];
        Cell& cell = grid[i + j * DIM];

        validator(cell, i, j);

        // Draw collapsed cells
        if (!cell.isDrawn()) {
          const sf::Texture& tileTexture = tiles.at(cell.getSingleOption()).texture;
          const sf::Sprite* sprite = cell.prepareSprite(tileTexture, i * w, j * h);

          renderTexture.draw(*sprite);
        }
      }

    ////////////////////////////////////////////////////////
    }
    renderTexture.display();
    renderTextureSecondary.display();

    window.clear(sf::Color(31, 30, 31));
    window.draw(canvasSprite);
    window.draw(canvasSpriteSecondary);
    window.display();
  }

  return 0;
}

