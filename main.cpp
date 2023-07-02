#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "tile.hpp"

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

  Tile blankTile("tiles/demo/blank.png");
  Tile downTile("tiles/demo/down.png");
  Tile leftTile("tiles/demo/left.png");
  Tile rightTile("tiles/demo/right.png");
  Tile upTile("tiles/demo/up.png");

  blankTile.setPosition(0.f, 0.f);
  downTile.setPosition(0.f, 200.f);
  leftTile.setPosition(0.f, 400.f);
  rightTile.setPosition(0.f, 600.f);
  upTile.setPosition(rightTile.width(), 0.f);

  /////////////////

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

    renderTexture.draw(blankTile.get());
    renderTexture.draw(downTile.get());
    renderTexture.draw(leftTile.get());
    renderTexture.draw(rightTile.get());
    renderTexture.draw(upTile.get());

    window.draw(canvasSprite);

    // end the current frame
    window.display();
  }

  return 0;
}

