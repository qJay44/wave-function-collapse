#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "Cell.hpp"
#include <algorithm>
#include <map>
#include <iterator>
#include <string>
#include <fstream>

int main() {
  srand((unsigned)time(NULL));

  const int width = 900;
  const int height = 900;

  // create the window
  sf::RenderWindow window(sf::VideoMode(width, height), "Wave function collapse");
  window.setFramerateLimit(75);

  sf::RenderTexture renderTexture;
  renderTexture.create(width, height);

  // Get the render texture and make sprite of it
  const sf::Texture &canvasTexture = renderTexture.getTexture();
  sf::Sprite canvasSprite(canvasTexture);

  // Load images and set rules (all going clockwise) //

  /*
   * ABOVE
   * RIGHT
   * BELOW
   * LEFT
   */

  std::map<TileType, Tile> tileMap {
    { TileType::BLANK, Tile("tiles/demo/blank.png",
        std::vector<std::vector<TileType>> {
          { TileType::BLANK, TileType::UP    }, // ABOVE
          { TileType::BLANK, TileType::RIGHT }, // RIGHT
          { TileType::BLANK, TileType::DOWN  }, // BELOW
          { TileType::BLANK, TileType::LEFT  }, // LEFT
        }
      )
    },
    { TileType::UP, Tile("tiles/demo/up.png",
        std::vector<std::vector<TileType>> {
          { TileType::RIGHT, TileType::LEFT, TileType::DOWN    },
          { TileType::LEFT,  TileType::UP,   TileType::DOWN    },
          { TileType::BLANK, TileType::DOWN                    },
          { TileType::RIGHT, TileType::UP,   TileType::DOWN    }
        }
      )
    },
    { TileType::RIGHT, Tile("tiles/demo/right.png",
        std::vector<std::vector<TileType>> {
          { TileType::RIGHT, TileType::LEFT, TileType::DOWN    },
          { TileType::LEFT,  TileType::UP,   TileType::DOWN    },
          { TileType::RIGHT, TileType::LEFT, TileType::UP      },
          { TileType::BLANK, TileType::LEFT                    },
        }
      )
    },
    { TileType::DOWN,  Tile("tiles/demo/down.png",
        std::vector<std::vector<TileType>> {
          { TileType::BLANK, TileType::UP                      },
          { TileType::LEFT,  TileType::UP,   TileType::DOWN    },
          { TileType::RIGHT, TileType::LEFT, TileType::UP      },
          { TileType::RIGHT, TileType::UP,   TileType::DOWN    }
        }
      )
    },
    { TileType::LEFT,  Tile("tiles/demo/left.png",
        std::vector<std::vector<TileType>> {
          { TileType::RIGHT, TileType::LEFT,   TileType::DOWN  },
          { TileType::BLANK, TileType::RIGHT                   },
          { TileType::RIGHT, TileType::LEFT,   TileType::UP    },
          { TileType::UP,    TileType::DOWN,   TileType::RIGHT }
        }
      )
    },
  };

  /////////////////

  // Initialize cells //

  const int DIM = 20;
  const float w = (float) width / DIM;
  const float h = (float) height / DIM;

  const sf::Vector2u textureSize = tileMap.at(TileType::BLANK).texture.getSize();
  const float scaleX = w / textureSize.x;
  const float scaleY = h / textureSize.y;

  std::vector<Cell> grid(DIM * DIM, Cell(scaleX, scaleY));

  //////////////////////

  bool allowSteps = false;
  bool nextStep = true;
  int stepCount = 0;

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

        if (event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::J)) {
          if (allowSteps) {
            nextStep = true;
          }
        }

        if (event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
          allowSteps = !allowSteps;
        }

        if (event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
          grid = std::vector<Cell>(DIM * DIM, Cell(scaleX, scaleY));
          renderTexture.clear(sf::Color(31, 30, 31));
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
        gridCopy.push_back(&cell);
      }

      gridCopy.erase(std::remove_if(gridCopy.begin(), gridCopy.end(), [&](Cell* obj) {
          return obj->isCollapsed();
      }), gridCopy.end());

      if (gridCopy.size() != 0) {
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

        ///////////////////////////////////////////////////////

        // Pick random cell with least entropy and randomly set option to it
        gridCopy[random(0, gridCopy.size() - 1)]->setRandomOption();
      }

      std::vector<Cell> nextGrid(DIM * DIM);

      for (int j = 0; j < DIM; j++) {
        for (int i = 0; i < DIM; i++) {
          int index = i + j * DIM;
          if (grid[index].isCollapsed())
            nextGrid[index] = grid[index];
          else {
            std::set<TileType> currOptions {
              TileType::BLANK,
              TileType::UP,
              TileType::RIGHT,
              TileType::DOWN,
              TileType::LEFT
            };

            // Look under
            if (j > 0) {
              Cell above = grid[i + (j - 1) * DIM];
              std::set<TileType> validOptions;

              for (TileType option : above.options) {
                std::vector<TileType> valid = tileMap[option].rules[2];
                std::copy(valid.begin(), valid.end(), std::inserter(validOptions, validOptions.end()));
              }
              Cell::validateOptions(currOptions, validOptions);
            }

            // Look right
            if (i < DIM - 1) {
              Cell right = grid[i + 1 + j * DIM];
              std::set<TileType> validOptions;

              for (TileType option : right.options) {
                std::vector<TileType> valid = tileMap[option].rules[3];
                std::copy(valid.begin(), valid.end(), std::inserter(validOptions, validOptions.end()));
              }
              Cell::validateOptions(currOptions, validOptions);
            }

            // Look above
            if (j < DIM - 1) {
              Cell under = grid[i + (j + 1) * DIM];
              std::set<TileType> validOptions;

              for (TileType option : under.options) {
                std::vector<TileType> valid = tileMap[option].rules[0];
                std::copy(valid.begin(), valid.end(), std::inserter(validOptions, validOptions.end()));
              }
              Cell::validateOptions(currOptions, validOptions);
            }

            // Look left
            if (i > 0) {
              Cell left = grid[i - 1 + j * DIM];
              std::set<TileType> validOptions;

              for (TileType option : left.options) {
                std::vector<TileType> valid = tileMap[option].rules[1];
                std::copy(valid.begin(), valid.end(), std::inserter(validOptions, validOptions.end()));
              }
              Cell::validateOptions(currOptions, validOptions);
            }

            nextGrid[index] = Cell(scaleX, scaleY, currOptions);
          }
        }
      }

      grid = nextGrid;

      for (int j = 0; j < DIM; j++) {
        for (int i = 0; i < DIM; i++) {
          Cell cell = grid[i + j * DIM];

          if (cell.isCollapsed() && !cell.isDrawn) {
            const sf::Texture tileTexture = tileMap.at(cell.getLastOption()).texture;
            cell.sprite.setTexture(tileTexture);
            cell.sprite.setPosition(i * w, j * h);
            cell.isDrawn = true;

            renderTexture.draw(cell.sprite);
          } else {
            /* // A rectangle with color to draw */
            /* sf::RectangleShape rect; */
            /* rect.setSize(sf::Vector2f(w, h)); */
            /* rect.setPosition(i * w, j * h); */
            /* rect.setFillColor(sf::Color(31, 30, 31)); */
            /* rect.setOutlineThickness(1.0f); */

            /* // Draw the rectangle on the render texture */
            /* renderTexture.draw(rect); */
          }
        }
      }
    }

    renderTexture.display();
    window.draw(canvasSprite);

    // end the current frame
    window.display();
  }

  return 0;
}

int random(int min, int max) {
  return min + rand() % ((max + 1) - min);
}

template<typename T>
void print(T msg) {
  std::cout << msg << "\n";
}

void print(std::vector<Cell> cells) {
  for (const Cell& cell : cells)
    std::cout << cell.options.size() << "\n";
}

std::string getTileTypeString(TileType tileType) {
  switch(tileType) {
    case TileType::BLANK:
      return "Blank";
    case TileType::UP:
      return "Up";
    case TileType::RIGHT:
      return "Right";
    case TileType::DOWN:
      return "Down";
    case TileType::LEFT:
      return "Left";
  }
}

