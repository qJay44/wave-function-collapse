#include "SFML/Graphics.hpp"
#include "utils.hpp"
#include "Directions.hpp"
#include <set>

class Tile {
  const std::vector<int> edges;
  sf::Texture texture;
  std::map<int, std::vector<int>> sides;

  public:

    Tile(const std::string &path, const std::vector<int>& edges)
      : edges(edges) {
      this->texture.loadFromFile(path);
      this->texture.setSmooth(true);
    }

    void setRules(const std::vector<Tile>& tiles) {
      for (int i = 0; i < tiles.size(); i++) {
        // connection for up
        if (tiles[i].edges[2] == edges[0])
          sides[NORTH].push_back(i);

        // connection for right
        if (tiles[i].edges[3] == edges[1])
          sides[EAST].push_back(i);

        // connection for down
        if (tiles[i].edges[0] == edges[2])
          sides[SOUTH].push_back(i);

        // connection for left
        if (tiles[i].edges[1] == edges[3])
          sides[WEST].push_back(i);
      }
    }

    void changeTexture(const std::string &path) {
      this->texture.loadFromFile(path);
    }

    const std::set<int> getSideOptions(int side) const {
      const std::vector<int>& sideOptions = sides.at(side);
      return std::set<int>(sideOptions.begin(), sideOptions.end());
    }

    const sf::Texture getTexture() const {
      return texture;
    }
};

