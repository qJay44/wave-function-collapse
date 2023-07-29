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

  sf::RenderTexture renderTexture;
  renderTexture.create(width, height);

  // Get the render texture and make sprite of it
  const sf::Texture &canvasTexture = renderTexture.getTexture();
  sf::Sprite canvasSprite(canvasTexture);

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
  std::vector<Cell> grid;
  grid.reserve(DIM * DIM);
  std::generate_n(std::back_inserter(grid), DIM * DIM, [&] {
    return Cell(scaleX, scaleY, tileMap.size());
  });

  sf::Font font;
  font.loadFromFile("Minecraft rus.ttf");

  sf::Text iterTitle;
  iterTitle.setFont(font);
  iterTitle.setCharacterSize(20);
  iterTitle.setFillColor(sf::Color::White);
  iterTitle.setOutlineColor(sf::Color(31, 30, 31));
  iterTitle.setOutlineThickness(4.f);
  iterTitle.setPosition(sf::Vector2f(20.f, 20.f));

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
          renderTexture.clear(sf::Color(31, 30, 31));
          stepCount = 0;
        }
    }

    // draw everything here...
    if (nextStep) {
      if (allowSteps)
        nextStep = false;
      stepCount++;

      // Get cells with least entropy and randomly collapse one; //

      std::vector<Cell*> leastEntropyCells;
      int lastSize = tileMap.size();

      for (Cell& cell : grid) {
        int currentSize = cell.options.size();

        if (!cell.isCollapsed()) {
          if (leastEntropyCells.size() == 0 || currentSize < lastSize) {
            lastSize = currentSize;
            leastEntropyCells.clear();
            leastEntropyCells.push_back(&cell);
          } else if (currentSize == lastSize)
            leastEntropyCells.push_back(&cell);
        }
      }
      if (leastEntropyCells.size() > 0)
        leastEntropyCells[random(0, leastEntropyCells.size() - 1)]->setRandomOption();

      /////////////////////////////////////////////////////////////

      // Draw collapsed cells and reduce options around it //

      for (int j = 0; j < DIM; j++) {
        for (int i = 0; i < DIM; i++) {
          const int index = i + j * DIM;
          Cell& cell = grid[index];

          if (cell.isCollapsed() && !cell.isDrawn) {
            const sf::Texture& tileTexture = tileMap.at(cell.getSingleOption()).texture;
            cell.sprite.setTexture(tileTexture);
            cell.sprite.setPosition(i * w, j * h);
            cell.isDrawn = true;

            renderTexture.draw(cell.sprite);

            // Look above
            if (j > 0) {
              const int iAbove = i;
              const int jAbove = j - 1;
              Cell& above = grid[iAbove + jAbove * DIM];

              above.checkNeighbours(iAbove, jAbove, DIM, grid, tileMap);
            }

            // Look right
            if (i < DIM - 1) {
              const int iRight = i + 1;
              const int jRight = j;
              Cell& right = grid[iRight + jRight * DIM];

              right.checkNeighbours(iRight, jRight, DIM, grid, tileMap);
            }

            // Look under
            if (j < DIM - 1) {
              const int iUnder = i;
              const int jUnder = j + 1;
              Cell& under = grid[iUnder + jUnder * DIM];

              under.checkNeighbours(iUnder, jUnder, DIM, grid, tileMap);
            }

            // Look left
            if (i > 0) {
              const int iLeft = i - 1;
              const int jLeft = j;
              Cell& left = grid[iLeft + jLeft * DIM];

              left.checkNeighbours(iLeft, jLeft, DIM, grid, tileMap);
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

