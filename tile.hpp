#include "SFML/Graphics.hpp"
#include "utils.hpp"
#include <algorithm>
#include <iterator>

class Tile {
  public:
    sf::Texture texture;
    const std::vector<int> edges;

    std::vector<int> above;
    std::vector<int> right;
    std::vector<int> under;
    std::vector<int> left;

    Tile() {}

    Tile(const std::string &path, const std::vector<int>& edges)
      : edges(edges) {
      this->texture.loadFromFile(path);
    }

    void setRules(std::vector<Tile>& tiles) {
      for (size_t i = 0; i < tiles.size(); i++) {
        // connection for up
        if (tiles[i].edges[2] == edges[0])
          above.push_back(i);

        // connection for right
        if (tiles[i].edges[3] == edges[1])
          right.push_back(i);

        // connection for under
        if (tiles[i].edges[0] == edges[2])
          under.push_back(i);

        // connection for left
        if (tiles[i].edges[1] == edges[3])
          left.push_back(i);
      }
    }
};

