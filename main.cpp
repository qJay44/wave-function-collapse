#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "Cell.hpp"
#include <memory>


float Cell::scaleX;
float Cell::scaleY;
int Cell::optionsSize;
sf::Font Cell::font;


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

  const int DIM = 60;
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
  sf::Vector2u textureSize = tiles.at(0).getTexture().getSize();

  // Generate the adjancency rules based on edges
  for (Tile& tile : tiles)
    tile.setRules(tiles);

  sf::Font font;
  font.loadFromFile("Minecraft rus.ttf");

  Cell::setCoreValues(
    w / textureSize.x,
    h / textureSize.y,
    tiles.size(),
    font
  );

  // Initialize cells
  std::vector<Cell> grid(DIM * DIM, Cell());
  grid.reserve(DIM * DIM);

  for (int j = 0; j < DIM; j++) {
    for (int i = 0; i < DIM; i++) {
      Cell& cell = grid[i + j * DIM];
      std::map<int, Cell*> neighbours;

      // Look up
      if (j > 0)
        neighbours[NORTH] = &grid[i + (j - 1) * DIM];

      // Look right
      if (i < DIM - 1)
        neighbours[EAST] = &grid[i + 1 + j * DIM];

      // Look under
      if (j < DIM - 1)
        neighbours[SOUTH] = &grid[i + (j + 1) * DIM];

      // Look left
      if (i > 0)
        neighbours[WEST] = &grid[i - 1 + j * DIM];

      cell.setNeighbours(neighbours);
      cell.setPosition(i, j, w, h, DIM);
    }
  }

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
            textureSize = tiles.at(0).getTexture().getSize();

            Cell::setCoreValues(
              w / textureSize.x,
              h / textureSize.y,
              tiles.size(),
              font
            );

            // Clear grid
            for (Cell& cell : grid)
              cell.reset();

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
            if (event.mouseButton.button == sf::Mouse::Right) {
              int i = event.mouseButton.x / w;
              int j = event.mouseButton.y / h;

              Cell& cell = grid[i + j * DIM];

              print("/////////////////////");

              printVector(cell.peekOptions(), "My options");
              print("My neighbours");
              for (const auto& n : cell.peekNeighourOptions())
                printVector(n.second, std::to_string(n.first));

              print("/////////////////////");
            }
        }
    }

    if (nextStep) {
      if (allowSteps)
        nextStep = false;

      renderTextureSecondary.clear(sf::Color(31, 30, 31, 0));

      int lastEntropy = tiles.size();
      std::vector<int> leastEntropyIndeces;
      std::deque<int> collapsedIndeces;

      for (int j = 0; j < DIM; j++) {
        for (int i = 0; i < DIM; i++) {
          int index = i + j * DIM;
          Cell& cell = grid[index];

          if (cell.isCollapsed()) continue;

          int currentEntropy = cell.getEntropy();

          // Add cells with least entropy
          if (leastEntropyIndeces.size() == 0 || currentEntropy < lastEntropy) {
            lastEntropy = currentEntropy;
            leastEntropyIndeces.clear();
            leastEntropyIndeces.push_back(index);

          } else if(currentEntropy == lastEntropy)
            leastEntropyIndeces.push_back(index);

          if (showOptionsText && cell.getEntropy() < 5)
            renderTextureSecondary.draw(*cell.prepareText());
        }
      }

      if (leastEntropyIndeces.size() > 0) {
          int index = leastEntropyIndeces[random(0, leastEntropyIndeces.size() - 1)];
          grid[index].setRandomOption();
          collapsedIndeces.push_back(index);
      }

      while (!collapsedIndeces.empty()) {
        Cell& cell = grid[collapsedIndeces.front()];
        collapsedIndeces.pop_front();

        cell.validateOptions(tiles, collapsedIndeces);

        if (!cell.isDrawn()) {
          const sf::Texture& tileTexture = tiles[cell.getSingleOption()].getTexture();
          const sf::Sprite* sprite = cell.prepareSprite(tileTexture);

          renderTexture.draw(*sprite);
        }
      }
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


