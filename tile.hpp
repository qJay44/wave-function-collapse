#include "SFML/Graphics.hpp"
#include "utils.hpp"
#include <algorithm>
#include <iterator>

class Tile {
  public:
    sf::Texture texture;
    const std::vector<int> edges;
    std::map<std::string, std::vector<int>> sides;

    Tile() {}

    Tile(const std::string &path, const std::vector<int>& edges)
      : edges(edges) {
      this->texture.loadFromFile(path);
      this->texture.setSmooth(true);
    }

    void setRules(const std::vector<Tile>& tiles) {
      for (int i = 0; i < tiles.size(); i++) {
        // connection for up
        if (tiles[i].edges[2] == edges[0])
          sides["above"].push_back(i);

        // connection for right
        if (tiles[i].edges[3] == edges[1])
          sides["right"].push_back(i);

        // connection for down
        if (tiles[i].edges[0] == edges[2])
          sides["under"].push_back(i);

        // connection for left
        if (tiles[i].edges[1] == edges[3])
          sides["left"].push_back(i);
      }
    }

    void changeTexture(const std::string &path) {
      this->texture.loadFromFile(path);
    }
};

