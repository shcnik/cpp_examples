#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <numeric>
#include <queue>
#include <unordered_map>
#include <vector>

template <typename T>
class DisjointSetUnion {
 public:
  DisjointSetUnion() = default;

  template <typename Container>
  DisjointSetUnion(const Container& items) {
    for (T item : items) {
      MakeSet(item);
    }
  }

  DisjointSetUnion(std::initializer_list<T> items) {
    for (T item : items) {
      MakeSet(item);
    }
  }

  void MakeSet(const T& value) {
    parents_[value] = value;
    rank_[value] = 0;
  }

  T FindSet(const T& value) {
    if (!HasParent(value)) {
      return value;
    }
    return parents_[value] = FindSet(parents_[value]);
  }

  void Unite(const T& first, const T& second) {
    T top_first = FindSet(first);
    T top_second = FindSet(second);
    if (top_first != top_second) {
      if (rank_[top_first] < rank_[top_second]) {
        std::swap(top_first, top_second);
      }
      parents_[top_second] = top_first;
      if (rank_[top_second] == rank_[top_first]) {
        ++rank_[top_first];
      }
    }
  }

  bool IsSameSet(const T& first, const T& second) {
    return FindSet(first) == FindSet(second);
  }

 private:
  bool HasParent(T value) { return parents_[value] != value; }

  std::unordered_map<T, T> parents_;
  std::unordered_map<T, size_t> rank_;
};

using Vertex = size_t;
using Graph = std::vector<std::vector<size_t>>;

void InitializeGraph(Graph& graph, size_t num_vertices) {
  graph.clear();
  graph.resize(num_vertices + 1);
  for (size_t v = 1; v <= num_vertices; ++v) {
    graph[v].resize(num_vertices + 1, 0);
  }
}

struct Edge {
  Vertex first;
  Vertex second;
  size_t weight;
};

template <typename Integer>
std::vector<Integer> GenerateSequence(Integer start, Integer end) {
  std::vector<Integer> res(end - start);
  std::iota(res.begin(), res.end(), start);
  return res;
}

Graph GetMaximumTree(const Graph& graph) {
  size_t cost = 0;
  std::vector<Edge> edges;
  for (Vertex u = 1; u < graph.size(); ++u) {
    for (Vertex v = u + 1; v < graph.size(); ++v) {
      if (graph[u][v] == 0) {
        continue;
      }
      edges.push_back(Edge{u, v, graph[u][v]});
    }
  }
  DisjointSetUnion<Vertex> connect(GenerateSequence<Vertex>(1, graph.size()));
  std::sort(edges.begin(), edges.end(), [](Edge first, Edge second) {
    return first.weight > second.weight;
  });
  Graph tree;
  InitializeGraph(tree, graph.size() - 1);
  for (Edge edge : edges) {
    if (!connect.IsSameSet(edge.first, edge.second)) {
      tree[edge.first][edge.second] = tree[edge.second][edge.first] =
          edge.weight;
      connect.Unite(edge.first, edge.second);
    }
  }
  return tree;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);
  size_t num_vertices, num_edges, num_queries;
  std::cin >> num_vertices >> num_edges >> num_queries;
  Graph graph;
  InitializeGraph(graph, num_vertices);
  for (size_t i = 0; i < num_edges; ++i) {
    Edge edge;
    std::cin >> edge.first >> edge.second >> edge.weight;
    graph[edge.first][edge.second] = graph[edge.second][edge.first] =
        edge.weight;
  }
  auto tree = GetMaximumTree(graph);
  std::vector<std::vector<size_t>> dist(num_vertices + 1);
  for (size_t v = 1; v <= num_vertices; ++v) {
    dist[v].resize(num_vertices + 1, std::numeric_limits<size_t>::max());
  }
  for (Vertex v = 1; v <= num_vertices; ++v) {
  }
  for (size_t q = 0; q < num_queries; ++q) {
    Vertex first, second;
    std::cin >> first >> second;
    if ((dist[first][second] == std::numeric_limits<size_t>::max()) &&
        (dist[second][first] == std::numeric_limits<size_t>::max())) {
      std::queue<Vertex> queue;
      std::vector<bool> used(num_vertices + 1, false);
      queue.push(first);
      used[first] = true;
      while (!queue.empty()) {
        Vertex cur = queue.front();
        queue.pop();
        for (Vertex next = 1; next <= num_vertices; ++next) {
          if (tree[cur][next] == 0) {
            continue;
          }
          if (used[next]) {
            continue;
          }
          dist[next][first] = dist[first][next] = std::min(dist[first][cur], tree[cur][next]);
          used[next] = true;
          queue.push(next);
        }
      }
    }
    std::cout << dist[first][second] << '\n';
  }
}
