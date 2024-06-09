#include <iostream>
#include <limits>
#include <queue>
#include <vector>

using Vertex = size_t;
using Weight = uint64_t;

struct Edge {
  Vertex first;
  Vertex second;
  uint64_t weight;
};

using Graph = std::vector<std::vector<Edge>>;

std::vector<Weight> FindLengths(const Graph& graph,
                                const std::vector<Vertex>& starts) {
  constexpr Weight kInfinity = std::numeric_limits<Weight>::max();
  std::vector<Weight> dist(graph.size(), kInfinity);
  std::priority_queue<std::pair<Weight, Vertex>,
                      std::vector<std::pair<Weight, Vertex>>,
                      std::greater<std::pair<Weight, Vertex>>>
      queue;
  for (Vertex start : starts) {
    dist[start] = 0;
    queue.push({0, start});
  }
  std::vector<bool> used(graph.size(), false);
  while (!queue.empty()) {
    Vertex cur = queue.top().second;
    queue.pop();
    if (used[cur]) {
      continue;
    }
    used[cur] = true;
    for (Edge edge : graph[cur]) {
      dist[edge.second] = std::min(dist[edge.second], dist[cur] + edge.weight);
      queue.push({dist[edge.second], edge.second});
    }
  }
  return dist;
}

std::vector<Weight> FindPath(const Graph& graph, Vertex start,
                             const std::vector<Weight>& infect) {
  constexpr Weight kInfinity = std::numeric_limits<Weight>::max();
  std::vector<Weight> dist(graph.size(), kInfinity);
  std::priority_queue<std::pair<Weight, Vertex>,
                      std::vector<std::pair<Weight, Vertex>>,
                      std::greater<std::pair<Weight, Vertex>>>
      queue;
  dist[start] = 0;
  queue.push({0, start});
  std::vector<bool> used(graph.size(), false);
  while (!queue.empty()) {
    Vertex cur = queue.top().second;
    queue.pop();
    if (used[cur]) {
      continue;
    }
    if (dist[cur] >= infect[cur]) {
      continue;
    }
    used[cur] = true;
    for (Edge edge : graph[cur]) {
      if ((dist[edge.second] > dist[cur] + edge.weight) &&
          (dist[cur] + edge.weight < infect[edge.second])) {
        dist[edge.second] = dist[cur] + edge.weight;
        queue.push({dist[edge.second], edge.second});
      }
    }
  }
  return dist;
}

int main() {
  size_t num_vertices, num_edges, num_sources;
  std::cin >> num_vertices >> num_edges >> num_sources;
  Graph graph(num_vertices, std::vector<Edge>());
  std::vector<Vertex> infected(num_sources);
  for (size_t i = 0; i < num_sources; ++i) {
    std::cin >> infected[i];
    --infected[i];
  }
  for (size_t i = 0; i < num_edges; ++i) {
    Vertex first, second;
    Weight weight;
    std::cin >> first >> second >> weight;
    graph[first - 1].push_back({first - 1, second - 1, weight});
    graph[second - 1].push_back({second - 1, first - 1, weight});
  }
  auto infect_time = FindLengths(graph, infected);
  Vertex start, target;
  std::cin >> start >> target;
  Weight res = FindPath(graph, start - 1, infect_time)[target - 1];
  if (res == std::numeric_limits<Weight>::max()) {
    std::cout << -1 << std::endl;
  } else {
    std::cout << res << std::endl;
  }
}