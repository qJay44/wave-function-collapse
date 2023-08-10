#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "Cell.hpp"
#include <cmath>
#include <filesystem>

#define WIDTH 900
#define HEIGHT 900
#define DIM 20

float Cell::scaleX;
float Cell::scaleY;
int Cell::optionsSize;
sf::Font Cell::font;
bool Cell::zeroOptionsLeft;


int main() {
  srand((unsigned)time(NULL));

  const std::string pathPrefix = "../../tiles/"; // Looking from "Build/[TYPE]/" directory
  const std::vector<std::string> tilesFolders {
    "circuit/", "circuit-coding-train/"
  };
  std::vector<std::string> directions;
  std::string folder = pathPrefix + random(tilesFolders);

  for (const auto& entry : std::filesystem::directory_iterator(folder))
    directions.push_back(entry.path().string());

  // 1.png, 10.png, 11.png, 2.png -> 1.png, 2.png, ..., 10.png, 11.png
  std::sort(directions.begin(), directions.end(),
    [] (const std::string& lhs, const std::string& rhs) {
      if (lhs.size() < rhs.size())
        return true;

      if (lhs.size() == rhs.size())
        switch (lhs.size()) {
          // One digit + '.png' (4)
          case 1 + 4:
            return lhs[0] < rhs[0];

          // Two digits + '.png' (4)
          case 2 + 4:
            std::string first;
            first = lhs[0] + lhs[1];

            std::string second;
            second = rhs[0] + rhs[1];

            return std::stoi(first) < std::stoi(second);
        }

      return false;
    });

  bool allowSteps = false;
  bool nextStep = true;
  bool showOptionsText = false;
  bool drawGrid = false;

  const float w = (float) WIDTH / DIM;
  const float h = (float) HEIGHT / DIM;

  // create the window
  sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Wave function collapse", sf::Style::Close);
  window.setFramerateLimit(75);

  sf::RenderTexture renderTexture;
  renderTexture.create(WIDTH, HEIGHT);

  // Get the render texture and make sprite of it
  const sf::Texture &canvasTexture = renderTexture.getTexture();
  sf::Sprite canvasSprite(canvasTexture);

  // Secondary layer for some tests drawables
  sf::RenderTexture renderTextureSecondary;
  renderTextureSecondary.create(WIDTH, HEIGHT);

  // Get the render texture and make sprite of it
  const sf::Texture &canvasTextureSecondary = renderTextureSecondary.getTexture();
  sf::Sprite canvasSpriteSecondary(canvasTextureSecondary);

  // Load images and set rules (all going clockwise) //
  std::vector<Tile> tiles {
    Tile(directions[0],   {"AAA", "AAA", "AAA", "AAA"}),
    Tile(directions[1],   {"BBB", "BBB", "BBB", "BBB"}),
    Tile(directions[2],   {"BBB", "BCB", "BBB", "BBB"}),
    Tile(directions[3],   {"BBB", "BDB", "BBB", "BDB"}),
    Tile(directions[4],   {"ABB", "BCB", "BBA", "AAA"}),
    Tile(directions[5],   {"ABB", "BBB", "BBB", "BBA"}),
    Tile(directions[6],   {"BBB", "BCB", "BBB", "BCB"}),
    Tile(directions[7],   {"BDB", "BCB", "BDB", "BCB"}),
    Tile(directions[8],   {"BDB", "BBB", "BCB", "BBB"}),
    Tile(directions[9],   {"BCB", "BCB", "BBB", "BCB"}),
    Tile(directions[10],  {"BCB", "BCB", "BCB", "BCB"}),
    Tile(directions[11],  {"BCB", "BCB", "BBB", "BBB"}),
    Tile(directions[12],  {"BBB", "BCB", "BBB", "BCB"})
  };

  // Add unique rotated versions of tiles
  const int initialTileCount = tiles.size();
  for (int i = 0; i < initialTileCount; i++) {
    std::set<std::vector<std::string>> uniqueEdgesSet;
    std::vector<Tile> rotatedTiles;

    uniqueEdgesSet.insert(tiles[i].getEdges());

    for (int j = 0; j < 4; j++)
      rotatedTiles.push_back(Tile::createRotatedVersion(tiles[i], j));

    Tile::removeDuplicates(uniqueEdgesSet, rotatedTiles);
    tiles.insert(tiles.end(), rotatedTiles.begin(), rotatedTiles.end());
  }

  // Generate the adjancency rules based on edges
  for (Tile& tile : tiles)
    tile.setRules(tiles);

  // Use any texture since all cells will have the same size
  sf::Vector2u textureSize = tiles.at(0).getTexture().getSize();

  // Font for some test text
  sf::Font font;
  font.loadFromFile("Minecraft rus.ttf");

  // Set generic fields
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

  auto reset = [&] () {
    // Clear grid
    for (Cell& cell : grid)
      cell.reset();

    // Clear window
    renderTexture.clear(sf::Color(30, 30, 30));

    Cell::zeroOptionsLeft = false;
  };

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

          if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
            reset();

          if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
            showOptionsText = !showOptionsText;

          if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
            drawGrid = !drawGrid;

        } else if (event.type == sf::Event::MouseButtonPressed) {
            int i = std::floor(event.mouseButton.x / w);
            int j = std::floor(event.mouseButton.y / h);

            if (event.mouseButton.button == sf::Mouse::Left) {
              grid[i + j * DIM].setRandomOption(true);
              nextStep = true;
            }

            if (event.mouseButton.button == sf::Mouse::Right) {
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

      renderTextureSecondary.clear(sf::Color(30, 30, 30, 0));

      if (Cell::zeroOptionsLeft)
        reset();

      std::vector<int> leastEntropyIndeces;

      for (int j = 0; j < DIM; j++) {
        for (int i = 0; i < DIM; i++) {
          int index = i + j * DIM;
          Cell& cell = grid[index];
          cell.validateOptions(tiles);

          if (!cell.isCollapsed()) {
            static int lastEntropy = tiles.size();
            int currentEntropy = cell.getEntropy();

            // Add cells with least entropy
            if (leastEntropyIndeces.size() == 0 || currentEntropy < lastEntropy) {
              lastEntropy = currentEntropy;
              leastEntropyIndeces.clear();
              leastEntropyIndeces.push_back(index);

            } else if(currentEntropy == lastEntropy)
              leastEntropyIndeces.push_back(index);

            if (showOptionsText && cell.getEntropy() < tiles.size())
              renderTextureSecondary.draw(cell.prepareText());
          } else {
            const Tile& tile = tiles[cell.getSingleOption()];
            const sf::Sprite& sprite = cell.prepareSprite(tile);
            renderTexture.draw(sprite);
          }

          if (drawGrid) {
            sf::RectangleShape cellOutline;
            cellOutline.setPosition({ i * w, j * h });
            cellOutline.setSize({ w, h });
            cellOutline.setOutlineThickness(1.f);
            cellOutline.setOutlineColor(sf::Color(90, 90, 90));
            cellOutline.setFillColor(sf::Color::Transparent);

            renderTextureSecondary.draw(cellOutline);
          }
        }
      }

      if (leastEntropyIndeces.size() > 0) {
          int index = random(leastEntropyIndeces);
          grid[index].setRandomOption();
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


