#include <iostream>
#include <numeric>
#include <vector>

using Vertex = size_t;

struct Edge {
  Vertex first;
  Vertex second;
  int weight;
};

struct Graph {
  std::vector<Vertex> vertices;
  std::vector<Edge> edges;
};

std::vector<int> FindLengths(const Graph& graph, Vertex start) {
  constexpr int kNoPath = 30000;
  std::vector<int> lengths(graph.vertices.size(), kNoPath);
  lengths[start - 1] = 0;
  bool change = false;
  do {
    change = false;
    for (Edge edge : graph.edges) {
      if (lengths[edge.first - 1] < kNoPath) {
        if (lengths[edge.second - 1] > lengths[edge.first - 1] + edge.weight) {
          lengths[edge.second - 1] = lengths[edge.first - 1] + edge.weight;
          change = true;
        }
      }
    }
  } while (change);
  return lengths;
}

int main() {
  size_t num_vertices, num_edges;
  std::cin >> num_vertices >> num_edges;
  std::vector<Vertex> vertices(num_vertices);
  std::iota(vertices.begin(), vertices.end(), 1);
  std::vector<Edge> edges(num_edges);
  for (size_t i = 0; i < num_edges; ++i) {
    std::cin >> edges[i].first >> edges[i].second >> edges[i].weight;
  }
  Graph graph{vertices, edges};
  auto res = FindLengths(graph, 1);
  for (auto item : res) {
    std::cout << item << " ";
  }
  std::cout << std::endl;
}
