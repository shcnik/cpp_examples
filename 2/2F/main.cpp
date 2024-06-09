#include <algorithm>
#include <iostream>
#include <limits>
#include <list>
#include <optional>
#include <vector>

using Vertex = size_t;
using Weight = int;

struct Edge {
  size_t id;
  Vertex first;
  Vertex second;
  Weight weight;
};

struct Graph {
  size_t vertices;
  std::vector<Edge> edges;
};

constexpr Weight kInfinity = std::numeric_limits<Weight>::min();
constexpr Vertex kNoVertex = std::numeric_limits<Vertex>::max();
constexpr size_t kNoEdge = std::numeric_limits<size_t>::max();

std::list<size_t> RestorePath(const std::vector<std::vector<Vertex>>& parents,
                              const std::vector<std::vector<size_t>>& eids,
                              Vertex start, Vertex end) {
  if (start == end) {
    return {};
  }
  if (parents[start][end] == kNoVertex) {
    return {eids[start][end]};
  }
  std::list<size_t> res =
      RestorePath(parents, eids, start, parents[start][end]);
  res.splice(res.end(), RestorePath(parents, eids, parents[start][end], end));
  return res;
}

std::optional<std::list<size_t>> FindPath(const Graph& graph,
                                          const std::vector<Vertex>& targets) {
  std::vector<std::vector<Weight>> dist(
      graph.vertices + 1, std::vector<Weight>(graph.vertices + 1, kInfinity));
  std::vector<std::vector<Vertex>> parents(
      graph.vertices + 1, std::vector<Vertex>(graph.vertices + 1, kNoVertex));
  std::vector<std::vector<size_t>> eids(
      graph.vertices + 1, std::vector<Vertex>(graph.vertices + 1, kNoEdge));
  for (size_t i = 1; i <= graph.vertices; ++i) {
    dist[i][i] = 0;
  }
  for (Edge edge : graph.edges) {
    dist[edge.first][edge.second] = edge.weight;
    eids[edge.first][edge.second] = edge.id;
  }
  for (size_t k = 1; k <= graph.vertices; ++k) {
    for (size_t i = 1; i <= graph.vertices; ++i) {
      for (size_t j = 1; j <= graph.vertices; ++j) {
        if ((dist[i][k] > kInfinity) && (dist[k][j] > kInfinity)) {
          if (dist[i][k] + dist[k][j] > dist[i][j]) {
            dist[i][j] = dist[i][k] + dist[k][j];
            parents[i][j] = k;
          }
        }
      }
    }
  }
  std::list<size_t> path;
  for (size_t i = 1; i < targets.size(); ++i) {
    for (size_t t = 1; t <= graph.vertices; ++t) {
      if ((dist[targets[i - 1]][t] > kInfinity) && (dist[t][t] > 0) &&
          (dist[t][targets[i]] > kInfinity)) {
        return std::nullopt;
      }
    }
    path.splice(path.end(),
                RestorePath(parents, eids, targets[i - 1], targets[i]));
  }
  return path;
}

int main() {
  size_t num_vertices, num_edges, num_targets;
  std::cin >> num_vertices >> num_edges >> num_targets;
  Graph graph{num_vertices, std::vector<Edge>(num_edges)};
  for (size_t i = 0; i < num_edges; ++i) {
    graph.edges[i].id = i + 1;
    std::cin >> graph.edges[i].first >> graph.edges[i].second >>
        graph.edges[i].weight;
  }
  std::vector<Vertex> targets(num_targets);
  for (size_t i = 0; i < num_targets; ++i) {
    std::cin >> targets[i];
  }
  std::vector<Vertex> path;
  auto next = FindPath(graph, targets);
  if (!next.has_value()) {
    std::cout << "infinitely kind" << std::endl;
    return 0;
  }
  for (Vertex item : *next) {
    path.push_back(item);
  }
  std::cout << path.size() << std::endl;
  for (size_t edge_id : path) {
    std::cout << edge_id << " ";
  }
  std::cout << std::endl;
  return 0;
}
