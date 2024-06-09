#include <iostream>
#include <limits>
#include <vector>

template <typename T>
using SquareTable = std::vector<std::vector<T>>;

using Vertex = size_t;

using Graph = SquareTable<bool>;

SquareTable<bool> FindAccess(const Graph& graph) {
  SquareTable<bool> res = graph;
  for (Vertex mid = 0; mid < graph.size(); ++mid) {
    for (Vertex i = 0; i < graph.size(); ++i) {
      for (Vertex j = 0; j < graph.size(); ++j) {
        if (res[i][mid] && res[mid][j]) {
          res[i][j] = true;
        }
      }
    }
  }
  return res;
}

int main() {
  size_t num_rooms;
  std::cin >> num_rooms;
  Graph graph(num_rooms, std::vector<bool>(num_rooms));
  for (Vertex i = 0; i < num_rooms; ++i) {
    for (Vertex j = 0; j < num_rooms; ++j) {
      bool next;
      std::cin >> next;
      graph[i][j] = next;
    }
  }
  auto res = FindAccess(graph);
  for (Vertex i = 0; i < num_rooms; ++i) {
    for (Vertex j = 0; j < num_rooms; ++j) {
      std::cout << res[i][j] << " ";
    }
    std::cout << std::endl;
  }
}
