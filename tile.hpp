#include "SFML/Graphics.hpp"
#include "utils.hpp"
#include "Directions.hpp"
#include <map>
#include <set>

class Tile {
  sf::Texture texture;
  std::vector<std::string> edges;
  std::map<int, std::vector<int>> sides;
  float rotation = 0.f;

  Tile(
    const std::vector<std::string> edges,
    sf::Texture& texture,
    std::map<int, std::vector<int>> sides,
    float rotation
  ) : edges(edges), texture(texture), sides(sides), rotation(rotation) {}

  bool compareEdges(const std::string& lhs, std::string rhs) const {
    return lhs == std::string(rhs.rbegin(), rhs.rend());
  }

  public:

    Tile(const std::string &path, const std::vector<std::string> edges)
      : edges(edges) {
      this->texture.loadFromFile(path);
      this->texture.setSmooth(true);
    }

    static Tile createRotatedVersion(Tile& tile, int rotateNum) {
      const int edgesCount = tile.edges.size();
      std::vector<std::string> newEdges(edgesCount);
      newEdges.reserve(edgesCount);

      /* Move edges postions forward by rotateNum
       * Ex. if rotateNum = 1:
       * [ AAA, ..., AAB, CCC ]
       * [ CCC, AAA, ..., AAB ]
      */
      for (int i = 0; i < edgesCount; i++)
        newEdges[i] = (tile.edges[(i - rotateNum + edgesCount) % edgesCount]);

      return Tile(
        newEdges,
        tile.texture,
        tile.sides,
        90.f * rotateNum
      );
    }

    static void removeDuplicates(
      std::set<std::vector<std::string>>& uniqueEdgesSet,
      std::vector<Tile>& rotatedTiles
    ) {
      for (int i = 0; i < rotatedTiles.size(); i++) {
        int before = uniqueEdgesSet.size();
        uniqueEdgesSet.insert(rotatedTiles[i].edges);
        int after = uniqueEdgesSet.size();

        // Size doesn't change
        // if previous vector is the same as current
        if (before == after)
          rotatedTiles.erase(rotatedTiles.begin() + i--);
      }
    }

    void setRules(const std::vector<Tile>& tiles) {
      for (int i = 0; i < tiles.size(); i++) {
        const Tile& tile = tiles[i];

        // connection for up
        if (compareEdges(tile.edges[2], edges[0]))
          sides[NORTH].push_back(i);

        // connection for right
        if (compareEdges(tile.edges[3], edges[1]))
          sides[EAST].push_back(i);

        // connection for down
        if (compareEdges(tile.edges[0], edges[2]))
          sides[SOUTH].push_back(i);

        // connection for left
        if (compareEdges(tile.edges[1], edges[3]))
          sides[WEST].push_back(i);
      }
    }

    void changeTexture(const std::string &path) {
      this->texture.loadFromFile(path);
    }

    const std::vector<std::string>& getEdges() const {
      return edges;
    }

    const std::set<int> getSideOptions(int side) const {
      std::vector<int> sideOptions = sides.at(side);
      return std::set<int>(sideOptions.begin(), sideOptions.end());
    }

    const sf::Texture& getTexture() const {
      return texture;
    }

    const float getRotation() const {
      return rotation;
    }
};

