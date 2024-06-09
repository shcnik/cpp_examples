#include <algorithm>
#include <iostream>
#include <limits>
#include <optional>
#include <queue>
#include <vector>

using Vertex = size_t;

struct Edge {
  Vertex first;
  Vertex second;
  size_t time;
  size_t price;

  Edge Reverse() { return {second, first, time, price}; }
};

using Graph = std::vector<std::vector<Edge>>;

struct State {
  static constexpr size_t kInfinity = std::numeric_limits<size_t>::max();

  Vertex vertex;
  size_t price;
  size_t time;
  std::vector<Vertex> path;

  State(Vertex vertex, size_t price = kInfinity, size_t time = kInfinity,
        const std::vector<Vertex>& path = {})
      : vertex(vertex), price(price), time(time), path(path) {}

  State() = default;

  bool Empty() { return path.empty(); }
};

State FindPath(const Graph& graph, Vertex start, Vertex finish,
               size_t max_time) {
  std::vector<State> opt(graph.size());
  std::generate(opt.begin(),
                opt.end(), [vertex = 0]() mutable { return State(vertex++); });
  opt[start] = State(start, 0, 0, {start});
  auto cmp = [](State a, State b) { return a.time > b.time; };
  std::priority_queue<State, std::vector<State>, decltype(cmp)> queue(cmp);
  queue.push(opt[start]);
  while (!queue.empty()) {
    State cur = queue.top();
    queue.pop();
    for (Edge next : graph[cur.vertex]) {
      if ((opt[next.second].price > opt[next.first].price + next.price) &&
          (opt[next.first].time + next.time <= max_time)) {
        opt[next.second].price = opt[next.first].price + next.price;
        opt[next.second].path = opt[next.first].path;
        opt[next.second].path.push_back(next.second);
        opt[next.second].time = opt[next.first].time + next.time;
        queue.push(opt[next.second]);
      }
    }
  }
  return opt[finish];
}

int main() {
  size_t num_vertices, num_edges, max_time;
  std::cin >> num_vertices >> num_edges >> max_time;
  Graph graph(num_vertices + 1);
  for (size_t i = 0; i < num_edges; ++i) {
    Edge edge;
    std::cin >> edge.first >> edge.second >> edge.price >> edge.time;
    graph[edge.first].push_back(edge);
    graph[edge.second].push_back(edge.Reverse());
  }
  auto res = FindPath(graph, 1, num_vertices, max_time);
  if (res.Empty()) {
    std::cout << -1 << std::endl;
  } else {
    std::cout << res.price << std::endl;
    std::cout << res.path.size() << std::endl;
    for (Vertex vertex : res.path) {
      std::cout << vertex << " ";
    }
    std::cout << std::endl;
  }
}