#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <numeric>
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

size_t GetMinimumCost(const Graph& graph) {
  size_t cost = 0;
  std::vector<Edge> edges;
  for (Vertex u = 0; u < graph.size(); ++u) {
    for (Vertex v = 0; v < graph.size(); ++v) {
      if (u == v) {
        continue;
      }
      edges.push_back(Edge{u, v, graph[u][v]});
    }
  }
  DisjointSetUnion<Vertex> connect(GenerateSequence<Vertex>(0, graph.size()));
  std::sort(edges.begin(), edges.end(), [](Edge first, Edge second) { return first.weight < second.weight; });
  for (Edge edge : edges) {
    if (!connect.IsSameSet(edge.first, edge.second)) {
      cost += edge.weight;
      connect.Unite(edge.first, edge.second);
    }
  }
  return cost;
}

int main() {
  size_t num_candidates;
  std::cin >> num_candidates;
  Graph costs(num_candidates + 1, std::vector<size_t>(num_candidates + 1, 0));
  for (Vertex u = 1; u <= num_candidates; ++u) {
    for (Vertex v = 1; v <= num_candidates; ++v) {
      std::cin >> costs[u][v];
    }
  }
  for (Vertex c = 1; c <= num_candidates; ++c) {
    size_t cost;
    std::cin >> cost;
    costs[0][c] = costs[c][0] = cost;
  }
  std::cout << GetMinimumCost(costs) << std::endl;
}
