#include <cassert>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <numeric>
#include <queue>
#include <unordered_map>
#include <unordered_set>
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

struct Edge {
  Vertex first;
  Vertex second;

  bool operator==(const Edge& other) const {
    return ((first == other.first) && (second == other.second)) ||
           ((first == other.second) && (second == other.first));
  }

  bool operator!=(const Edge& other) const { return !(*this == other); }

  Edge Reverse() const { return Edge{second, first}; }
};

template <>
struct std::hash<Edge> {
  size_t operator()(const Edge& edge) const {
    return std::hash<size_t>{}(edge.first) + std::hash<size_t>{}(edge.second);
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

  std::unordered_set<Edge> operator()(Vertex start = 1) {
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

class CondenseGraph {
 public:
  std::pair<std::vector<Vertex>, std::unordered_set<Edge>> operator()(const Graph& graph) {
    used_ = std::vector<bool>(graph.size(), false);
    res_.first = std::vector<Vertex>(graph.size());
    res_.second = FindBridges{graph}();
    max_color_ = 1;
    DFS(graph, 1, 1);
    return res_;
  }

 private:
  void DFS(const Graph& graph, Vertex cur, size_t color) {
    res_.first[cur] = color;
    used_[cur] = true;
    for (auto edge : graph[cur]) {
      if (used_[edge.second]) {
        continue;
      }
      if (res_.second.count(edge) != 0) {
        ++max_color_;
        DFS(graph, edge.second, max_color_);
      } else {
        DFS(graph, edge.second, color);
      }
    }
  }

  std::vector<bool> used_;
  std::pair<std::vector<Vertex>, std::unordered_set<Edge>> res_;
  size_t max_color_;
};

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
    for (auto next : graph_[cur]) {
      if (used_[next.second]) {
        continue;
      }
      DFS(next.second, depth + 1, res);
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
    std::vector<std::vector<Vertex>> query_graph(graph_.size());
    for (auto query : queries) {
      query_graph[query.first].push_back(query.second);
      query_graph[query.second].push_back(query.first);
    }
    DFS(root, query_graph, res);
    return res;
  }

 private:
  void DFS(Vertex cur, const std::vector<std::vector<Vertex>>& queries, std::unordered_map<Query, Vertex>& res) {
    ancestors_[cur] = cur;
    used_[cur] = true;
    for (Edge next : graph_[cur]) {
      if (used_[next.second]) {
        continue;
      }
      DFS(next.second, queries, res);
      set_.Unite(cur, next.second);
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
  size_t num_vertices, num_edges;
  std::cin >> num_vertices >> num_edges;
  Vertex end;
  std::cin >> end;
  Graph graph(num_vertices + 1);
  for (size_t i = 0; i < num_edges; ++i) {
    Edge edge;
    std::cin >> edge.first >> edge.second;
    graph[edge.first].push_back(edge);
    graph[edge.second].push_back(edge.Reverse());
  }
  auto [translate, bridges] = CondenseGraph{}(graph);
  Graph condense(num_vertices + 1);
  for (auto bridge : bridges) {
    bridge.first = translate[bridge.first];
    bridge.second = translate[bridge.second];
    condense[bridge.first].push_back(bridge);
    condense[bridge.second].push_back(bridge.Reverse());
  }
  auto depths = GetDepths{condense}(translate[end]);
  size_t num_queries;
  std::cin >> num_queries;
  std::unordered_map<GetQueryResults::Query, std::vector<size_t>> ids;
  std::vector<GetQueryResults::Query> queries(num_queries);
  for (size_t i = 0; i < num_queries; ++i) {
    std::cin >> queries[i].first >> queries[i].second;
    queries[i].first = translate[queries[i].first];
    queries[i].second = translate[queries[i].second];
    if (queries[i].first > queries[i].second) {
      std::swap(queries[i].first, queries[i].second);
    }
    if (ids.count(queries[i]) == 0) {
      ids[queries[i]] = {i};
    } else {
      ids[queries[i]].push_back(i);
    }
  }
  auto lca = GetQueryResults{condense}(translate[end], queries);
  std::vector<size_t> res(num_queries, std::numeric_limits<size_t>::max());
  for (auto [query, vertex] : lca) {
    for (auto id : ids[query]) {
      res[id] = depths[vertex];
    }
  }
  for (auto ans : res) {
    std::cout << ans << std::endl;
  }
}
