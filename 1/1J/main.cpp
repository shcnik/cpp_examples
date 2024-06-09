#include <functional>
#include <iostream>
#include <limits>
#include <unordered_set>
#include <vector>

using Vertex = size_t;

struct Edge {
  Vertex first;
  Vertex second;
  size_t number;

  bool operator==(const Edge& other) const { return number == other.number; }

  bool operator!=(const Edge& other) const { return !(*this == other); }

  Edge Reverse() const { return Edge{second, first, number}; }
};

template <>
struct std::hash<Edge> {
  size_t operator()(const Edge& edge) const {
    return std::hash<size_t>{}(edge.number);
  }
};

using Graph = std::vector<std::vector<Edge>>;

class FindBridges {
 public:
  FindBridges(const Graph& graph)
      : graph_(graph),
        used_(graph.size(), false),
        t_in_(graph.size(), std::numeric_limits<size_t>::max()),
        f_up_(graph.size(), std::numeric_limits<size_t>::max()) {}

  std::unordered_set<Edge> operator()(Vertex start) {
    used_[start] = true;
    t_in_[start] = f_up_[start] = time_++;
    std::unordered_set<Edge> res;
    for (auto edge : graph_[start]) {
      Visit(edge.second, edge, res);
      f_up_[start] = std::min(f_up_[start], f_up_[edge.second]);
      if (f_up_[edge.second] > t_in_[start]) {
        res.insert(edge);
      }
    }
    for (auto bridge : res) {
      std::cout << bridge.first << "-" << bridge.second << std::endl;
    }
    return res;
  }

 private:
  void Visit(Vertex vertex, Edge edge, std::unordered_set<Edge>& res) {
    used_[vertex] = true;
    t_in_[vertex] = f_up_[vertex] = time_++;
    for (auto next : graph_[vertex]) {
      if (next == edge) {
        continue;
      }
      if (used_[next.second]) {
        f_up_[vertex] = std::min(f_up_[vertex], t_in_[next.second]);
      } else {
        Visit(next.second, next, res);
        f_up_[vertex] = std::min(f_up_[vertex], f_up_[next.second]);
        if (f_up_[next.second] > t_in_[vertex]) {
          res.insert(next);
        }
      }
    }
  }

  const Graph& graph_;
  std::vector<bool> used_;
  std::vector<size_t> t_in_;
  std::vector<size_t> f_up_;
  size_t time_ = 0;
};

class CountRequiredEdges {
 public:
  CountRequiredEdges(const Graph& graph)
      : graph_(graph), used_(graph.size(), false) {
    bridges_ = FindBridges{graph}(1);
  }

  size_t operator()() {
    size_t res = 0;
    for (size_t start = 1; start < graph_.size(); ++start) {
      if (!used_[start]) {
        used_[start] = true;
        size_t num_bridges = Visit(start);
        if (num_bridges == 1) {
          ++res;
        }
      }
    }
    return (res == 0) ? 0 : ((res - 1) / 2 + 1);
  }

 private:
  size_t Visit(Vertex vertex) {
    size_t num_bridges = 0;
    used_[vertex] = true;
    for (auto next : graph_[vertex]) {
      if (bridges_.count(next) != 0) {
        ++num_bridges;
        continue;
      }
      if (used_[next.second]) {
        continue;
      }
      num_bridges += Visit(next.second, next);
    }
    return num_bridges;
  }

  size_t Visit(Vertex vertex, Edge parent) {
    size_t num_bridges = 0;
    used_[vertex] = true;
    for (auto next : graph_[vertex]) {
      if (next == parent) {
        continue;
      }
      if (bridges_.count(next) != 0) {
        ++num_bridges;
        continue;
      }
      if (used_[next.second]) {
        continue;
      }
      num_bridges += Visit(next.second, next);
    }
    return num_bridges;
  }

  const Graph& graph_;
  std::vector<bool> used_;
  std::unordered_set<Edge> bridges_;
};

int main() {
  size_t num_planets, num_routes;
  std::cin >> num_planets >> num_routes;
  Graph map(num_planets + 1);
  for (size_t i = 0; i < num_routes; ++i) {
    Edge edge;
    edge.number = i;
    std::cin >> edge.first >> edge.second;
    map[edge.first].push_back(edge);
    map[edge.second].push_back(edge.Reverse());
  }
  std::cout << CountRequiredEdges{map}() << std::endl;
}