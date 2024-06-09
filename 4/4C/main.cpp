#include <algorithm>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <vector>

static constexpr int64_t kInfinity = std::numeric_limits<int64_t>::max();

struct Edge {
  size_t to_vertex;
  int64_t flow;
  int64_t capacity;

  int64_t CurrentCapacity() const { return capacity - flow; }

  Edge(size_t to_vertex, int64_t capacity)
      : to_vertex(to_vertex), flow(0), capacity(capacity) {}
};

class Graph {
 public:
  Graph(size_t num_vertices, size_t num_edges,
        std::unordered_map<size_t, size_t>& map)
      : graph_(2 * num_vertices + 1, std::vector<int64_t>()) {
    const size_t kMagicConst = 1500;
    for (size_t i = 1; i <= num_edges; ++i) {
      size_t from;
      size_t to;
      int64_t capacity;
      std::cin >> from >> to >> capacity;
      AddEdge(from, to, capacity);
      AddEdge(to, from + num_vertices, capacity);
      AddEdge(from + num_vertices, from, capacity);
      map[from * kMagicConst + to] = i;
      map[to * kMagicConst + from] = i;
    }
  }

  void AddEdge(size_t from, size_t to, int64_t capacity) {
    edges_.emplace_back(to, capacity);
    graph_[from].emplace_back(edges_.size() - 1);
    edges_.emplace_back(from, 0);
    graph_[to].emplace_back(edges_.size() - 1);
  }

  std::vector<std::pair<size_t, size_t>> MaxFlow(size_t s_in, size_t t_in,
                                                   size_t num_vertices) {
    std::vector<std::pair<size_t, size_t>> cut;
    std::vector<size_t> used(graph_.size(), 0);
    size_t phase = 1;
    while (FindIncreasingPath(used, phase, s_in, t_in, kInfinity) != 0) {
      ++phase;
    }
    int64_t result = 0;
    for (size_t i = 0; i < graph_[s_in].size(); ++i) {
      result += edges_[graph_[s_in][i]].flow;
    }
    std::vector<bool> visited(graph_.size(), false);
    visited[s_in] = true;
    DFS(visited, s_in);
    for (int i = 0; i <= num_vertices; ++i) {
      if (visited[i]) {
        for (auto edge_id : graph_[i]) {
          auto to = edges_[edge_id].to_vertex;
          if ((edges_[edge_id].CurrentCapacity() == 0) && !visited[to] &&
              (to <= num_vertices)) {
            cut.push_back({i, to});
          }
        }
      }
    }
    std::cout << cut.size() << " " << result << '\n';
    return cut;
  }

 private:
  int64_t FindIncreasingPath(std::vector<size_t>& used,
                             size_t phase, int64_t v_in, int64_t t_in,
                             int64_t min) {
    if (t_in == v_in) {
      return min;
    }
    used[v_in] = phase;
    for (int64_t edge_id : graph_[v_in]) {
      if (edges_[edge_id].CurrentCapacity() == 0) {
        continue;
      }
      if (used[edges_[edge_id].to_vertex] == phase) {
        continue;
      }
      int64_t delta = FindIncreasingPath(used, phase, edges_[edge_id].to_vertex, t_in,
          std::min(min, edges_[edge_id].CurrentCapacity()));
      if (delta > 0) {
        edges_[edge_id].flow += delta;
        edges_[edge_id ^ 1].flow -= delta;  // back edge
        return delta;
      }
    }
    return 0;
  }

  void DFS(std::vector<bool>& used, size_t start) {
    for (auto edge_id : graph_[start]) {
      auto to = edges_[edge_id].to_vertex;
      if (!used[to] and edges_[edge_id].CurrentCapacity() != 0) {
        used[to] = true;
        DFS(used, to);
      }
    }
  }

  std::vector<std::vector<int64_t>> graph_;
  std::vector<Edge> edges_;
};

void PrintAnswer(Graph& graph, size_t num_vertices,
                 std::unordered_map<size_t, size_t>& map) {
  std::vector<size_t> answer;
  const size_t kMagicConst = 1500;
  auto pairs = graph.MaxFlow(1, num_vertices, num_vertices);
  for (auto [from, to] : pairs) {
    answer.push_back(map[from * kMagicConst + to]);
  }
  std::sort(answer.begin(), answer.end());
  for (auto x : answer) {
    std::cout << x << " ";
  }
}

int main() {
  size_t num_vertices;
  size_t num_edges;
  std::cin >> num_vertices >> num_edges;
  std::unordered_map<size_t, size_t> map;
  Graph graph(num_vertices, num_edges, map);
  PrintAnswer(graph, num_vertices, map);
}
