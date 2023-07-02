#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "Cell.hpp"
#include <map>
#include <iostream>

sf::Sprite Cell::originalSprite = sf::Sprite();

int main() {
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

  Cell::setSize(w, h, tileMap.at(TileType::BLANK).texturePtr->getSize());

  std::vector<Cell> grid(DIM * DIM);

  for (int i = 0; i < grid.size(); i++) {
    grid[i] = Cell();
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

    for (int j = 0; j < DIM; j++) {
      for (int i = 0; i < DIM; i++) {
        Cell cell = grid[i + j * DIM];

        if (cell.isCollapsed()) {
          const sf::Texture tileTexture = *tileMap.at(cell.getLastOption()).texturePtr;

          renderTexture.draw(cell.setTexture(tileTexture));
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

