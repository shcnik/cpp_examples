#include <initializer_list>
#include <iostream>
#include <unordered_map>
#include <utility>
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
using Graph = std::vector<std::vector<Vertex>>;

class GetDepths {
 public:
  GetDepths(const Graph& graph) : used_(graph.size(), false), graph_(graph) {}

  std::vector<size_t> operator()(Vertex root) {
    used_.assign(graph_.size(), false);
    std::vector<size_t> res(graph_.size());
    DFS(root, 0, res);
    return res;
  }

 private:
  void DFS(Vertex cur, size_t depth, std::vector<size_t>& res) {
    res[cur] = depth;
    used_[cur] = true;
    for (size_t next : graph_[cur]) {
      if (used_[next]) {
        continue;
      }
      DFS(next, depth + 1, res);
    }
  }

  std::vector<bool> used_;
  Graph graph_;
};

template <>
struct std::hash<std::pair<Vertex, Vertex>> {
  size_t operator()(std::pair<Vertex, Vertex> value) const {
    return std::hash<size_t>{}(value.first) + std::hash<size_t>{}(value.second);
  }
};

class GetQueryResults {
 public:
  using Query = std::pair<Vertex, Vertex>;

  GetQueryResults(const Graph& graph)
      : used_(graph.size(), false),
        graph_(graph),
        ancestors_(graph.size()),
        set_() {
    for (size_t i = 1; i < graph_.size(); ++i) {
      set_.MakeSet(i);
    }
  }

  std::unordered_map<Query, Vertex> operator()(Vertex root, const std::vector<Query>& queries) {
    std::unordered_map<std::pair<Vertex, Vertex>, Vertex> res;
    Graph query_graph(graph_.size());
    for (auto query : queries) {
      query_graph[query.first].push_back(query.second);
      query_graph[query.second].push_back(query.first);
    }
    DFS(root, query_graph, res);
    return res;
  }

 private:
  void DFS(Vertex cur, const Graph& queries, std::unordered_map<Query, Vertex>& res) {
    ancestors_[cur] = cur;
    used_[cur] = true;
    for (Vertex next : graph_[cur]) {
      if (used_[next]) {
        continue;
      }
      DFS(next, queries, res);
      set_.Unite(cur, next);
      ancestors_[set_.FindSet(cur)] = cur;
    }
    for (Vertex other : queries[cur]) {
      if (used_[other]) {
        res[{std::min(cur, other), std::max(cur, other)}] = ancestors_[set_.FindSet(other)];
      }
    }
  }

  std::vector<bool> used_;
  Graph graph_;
  std::vector<Vertex> ancestors_;
  DisjointSetUnion<Vertex> set_;
};

int main() {
  size_t num_vertices;
  std::cin >> num_vertices;
  Graph tree(num_vertices + 1);
  for (size_t i = 0; i < num_vertices - 1; ++i) {
    Vertex first, second;
    std::cin >> first >> second;
    tree[first].push_back(second);
    tree[second].push_back(first);
  }
  auto depths = GetDepths{tree}(1);
  size_t num_queries;
  std::cin >> num_queries;
  std::unordered_map<GetQueryResults::Query, std::vector<size_t>> ids;
  std::vector<GetQueryResults::Query> queries(num_queries);
  for (size_t i = 0; i < num_queries; ++i) {
    std::cin >> queries[i].first >> queries[i].second;
    if (queries[i].first > queries[i].second) {
      std::swap(queries[i].first, queries[i].second);
    }
    if (ids.count(queries[i]) == 0) {
      ids[queries[i]] = {i};
    } else {
      ids[queries[i]].push_back(i);
    }
  }
  auto res = GetQueryResults{tree}(1, queries);
  std::vector<size_t> lengths(num_queries);
  for (auto [query, vertex] : res) {
    for (auto id : ids[query]) {
      lengths[id] = depths[query.first] + depths[query.second] - 2 * depths[vertex];
    }
  }
  for (auto length : lengths) {
    std::cout << length << "\n";
  }
  return 0;
}
