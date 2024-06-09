#include <algorithm>
#include <iostream>
#include <limits>
#include <optional>
#include <vector>

using Vertex = size_t;
using Weight = int;

struct Edge {
  Vertex first;
  Vertex second;
  Weight weight;
};

struct Graph {
  size_t vertices;
  std::vector<Edge> edges;
};

std::optional<std::vector<Vertex>> FindNegativeCycle(const Graph& graph) {
  constexpr Weight kInfinity = std::numeric_limits<Weight>::max();
  constexpr Vertex kNoVertex = std::numeric_limits<Vertex>::max();
  std::vector<Weight> dist(graph.vertices, kInfinity);
  std::vector<Vertex> parents(graph.vertices, kNoVertex);
  for (Vertex vertex = 0; vertex < graph.vertices; ++vertex) {
    if (dist[vertex] < kInfinity) {
      continue;
    }
    dist[vertex] = 0;
    Vertex start = kNoVertex;
    for (size_t i = 0; i < graph.vertices; ++i) {
      start = kNoVertex;
      for (Edge edge : graph.edges) {
        if (dist[edge.first] < kInfinity) {
          if (dist[edge.second] > dist[edge.first] + edge.weight) {
            dist[edge.second] = dist[edge.first] + edge.weight;
            parents[edge.second] = edge.first;
            start = edge.second;
          }
        }
      }
    }
    if (start != kNoVertex) {
      for (size_t i = 0; i < graph.vertices; ++i) {
        start = parents[start];
      }
      std::vector<Vertex> cycle;
      for (Vertex cur = start; (cur != start) || cycle.empty();
           cur = parents[cur]) {
        cycle.push_back(cur);
      }
      cycle.push_back(start);
      std::reverse(cycle.begin(), cycle.end());
      return cycle;
    }
  }
  return std::nullopt;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);
  size_t num_vertices;
  std::cin >> num_vertices;
  Graph graph{num_vertices, std::vector<Edge>()};
  constexpr Weight kInfinity = 100000;
  for (size_t i = 0; i < num_vertices; ++i) {
    for (size_t j = 0; j < num_vertices; ++j) {
      Weight weight;
      std::cin >> weight;
      if (weight == kInfinity) {
        continue;
      }
      Edge edge{i, j, weight};
      graph.edges.push_back(edge);
    }
  }
  auto res = FindNegativeCycle(graph);
  if (!res.has_value()) {
    std::cout << "NO" << std::endl;
  } else {
    std::cout << "YES" << std::endl;
    std::cout << res->size() << std::endl;
    for (auto item : *res) {
      std::cout << (item + 1) << " ";
    }
    std::cout << std::endl;
  }
  return 0;
}
