#include <algorithm>
#include <iostream>
#include <optional>
#include <set>
#include <vector>

using Edge = std::pair<size_t, size_t>;

class Graph {
 public:
  Graph(size_t num_vertices, const std::vector<Edge>& edges)
      : matrix_(num_vertices + 1, std::set<size_t>()) {
    for (auto edge : edges) {
      matrix_[edge.first].insert(edge.second);
      matrix_[edge.second].insert(edge.first);
    }
  }

  size_t NumVertices() const { return matrix_.size() - 1; }

  const std::set<size_t>& GetNeighbours(size_t vertex) const {
    return matrix_[vertex];
  }

 private:
  std::vector<std::set<size_t>> matrix_;
};

class DFS {
 public:
  DFS(const Graph& graph)
      : graph_(graph),
        used_(graph.NumVertices() + 1, false),
        t_in_(graph.NumVertices() + 1, 0),
        f_up_(graph.NumVertices() + 1, 0) {}

  std::set<size_t> operator()() {
    std::set<size_t> res;
    for (size_t i = 1; i <= graph_.NumVertices(); ++i) {
      if (!used_[i]) {
        Visit(i, res);
      }
    }
    return res;
  }

 private:
  void Visit(size_t vertex, std::set<size_t>& res, size_t parent = kNoParent) {
    used_[vertex] = true;
    t_in_[vertex] = f_up_[vertex] = ++time_;
    size_t child = 0;
    for (auto next : graph_.GetNeighbours(vertex)) {
      if ((next == parent) || (next == vertex)) {
        continue;
      }
      if (used_[next]) {
        f_up_[vertex] = std::min(f_up_[vertex], t_in_[next]);
      } else {
        Visit(next, res, vertex);
        f_up_[vertex] = std::min(f_up_[vertex], f_up_[next]);
        if ((f_up_[next] >= t_in_[vertex]) && (parent != kNoParent)) {
          res.insert(vertex);
        }
        ++child;
      }
    }
    if ((parent == kNoParent) && (child > 1)) {
      res.insert(vertex);
    }
  }

  const Graph& graph_;
  std::vector<bool> used_;
  std::vector<size_t> t_in_;
  std::vector<size_t> f_up_;
  size_t time_ = 0;

  static constexpr size_t kNoParent = 0;
};

int main() {
  size_t num_vertices, num_edges;
  std::cin >> num_vertices >> num_edges;
  std::vector<Edge> edges(num_edges);
  for (size_t i = 0; i < num_edges; ++i) {
    std::cin >> edges[i].first >> edges[i].second;
  }
  Graph graph(num_vertices, edges);
  DFS dfs(graph);
  auto res = dfs();
  std::cout << res.size() << std::endl;
  for (auto x : res) {
    std::cout << x << std::endl;
  }
}