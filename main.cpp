#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "Cell.hpp"
#include <map>
#include <iostream>
#include <stdlib.h>
#include <iterator>

int random(int min, int max) {
  return min + rand() % ((max + 1) - min);
}

int main() {
  srand((unsigned)time(NULL));

  const int width = 1200;
  const int height = 900;

  // create the window
  sf::RenderWindow window(sf::VideoMode(width, height), "Wave function collapse");
  window.setFramerateLimit(75);

  sf::RenderTexture renderTexture;
  renderTexture.create(width, height);

  // Get the render texture and make sprite of it
  const sf::Texture &canvasTexture = renderTexture.getTexture();
  sf::Sprite canvasSprite(canvasTexture);

  // Load images //

  std::map<TileType, Tile> tileMap {
    { TileType::BLANK, Tile("tiles/demo/blank.png") },
    { TileType::DOWN,  Tile("tiles/demo/down.png")  },
    { TileType::LEFT,  Tile("tiles/demo/left.png")  },
    { TileType::RIGHT, Tile("tiles/demo/right.png") },
    { TileType::UP,    Tile("tiles/demo/up.png")    }
  };

  /////////////////

  // Initialize cells //

  const int DIM = 2;
  const float w = (float) width / DIM;
  const float h = (float) height / DIM;

  const sf::Vector2u textureSize = tileMap.at(TileType::BLANK).texture.getSize();
  const float scaleX = w / textureSize.x;
  const float scaleY = h / textureSize.y;

  std::vector<Cell> grid(DIM * DIM);

  for (int i = 0; i < grid.size(); i++) {
    grid[i] = Cell(scaleX, scaleY);
  }

  for (int i = 0; i < 4; i++) {
    grid[0].temp();
  }

  //////////////////////

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
    }

    // clear the window with black color
    window.clear(sf::Color(31, 30, 31));

    // draw everything here...

    // Sort grid with least entrory
    std::sort(grid.begin(), grid.end(), [&](Cell obj1, Cell obj2) {
      return obj1.getOptionsSize() < obj2.getOptionsSize();
    });

    // Create collection with the same (minimal) entropy //

    std::vector<Cell*> gridMinEntropy;
    const int len = grid[0].getOptionsSize();

    for (int i = 0; i < grid.size(); i++) {
      if (grid[i].getOptionsSize() > len)
        break;
      else
        gridMinEntropy.push_back(&grid[i]);
    }

    ///////////////////////////////////////////////////////

    ////////////////////////////////////////////
    // Pick random cell with least entropy    //
    // and randomly set a single option to it //
    //                                        //

    auto tileMapIter = tileMap.begin();
    std::advance(tileMapIter, random(0, tileMap.size() - 1));
    gridMinEntropy[random(0, gridMinEntropy.size() - 1)]->setSingleOption(tileMapIter->first);

    ////////////////////////////////////////////


    for (int j = 0; j < DIM; j++) {
      for (int i = 0; i < DIM; i++) {
        Cell cell = grid[i + j * DIM];

        if (cell.isCollapsed()) {
          const sf::Texture tileTexture = tileMap.at(cell.getLastOption()).texture;
          cell.sprite.setTexture(tileTexture);
          cell.sprite.setPosition(i * w, j * h);

          renderTexture.draw(cell.sprite);
        } else {
          // A rectangle with color to draw
          sf::RectangleShape rect;
          rect.setSize(sf::Vector2f(w, h));
          rect.setPosition(i * w, j * h);
          rect.setFillColor(sf::Color(31, 30, 31));
          rect.setOutlineColor(sf::Color::White);
          rect.setOutlineThickness(1.0f);

          // Draw the rectangle on the render texture
          renderTexture.draw(rect);
        }
      }
    }

    window.draw(canvasSprite);

    // end the current frame
    window.display();
  }

  return 0;
}

