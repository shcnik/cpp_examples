#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <numeric>
#include <optional>
#include <queue>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

template <typename Vertex, typename Edge, typename BaseIterator>
class GraphIterator {
 public:
  GraphIterator(BaseIterator start, BaseIterator end,
                std::function<bool(Edge)> filter)
      : iter_(start), end_(end), filter_(filter) {
    while ((iter_ != end_) && (!filter_(*iter_))) {
      ++iter_;
    }
  }

  GraphIterator<Vertex, Edge, BaseIterator>& operator++() {
    do {
      ++iter_;
    } while ((iter_ != end_) && !filter_(*iter_));
  }

  bool operator==(
      const GraphIterator<Vertex, Edge, BaseIterator>& other) const {
    return iter_ == other.iter_;
  }

  bool operator!=(
      const GraphIterator<Vertex, Edge, BaseIterator>& other) const {
    return iter_ != other.iter_;
  }

  // NOLINTNEXTLINE
  GraphIterator<Vertex, Edge, BaseIterator> begin() const { return *this; }

  // NOLINTNEXTLINE
  GraphIterator<Vertex, Edge, BaseIterator> end() const {
    return GraphIterator<Vertex, Edge, BaseIterator>(end_, end_, filter_);
  }

  const Vertex& operator*() { return iter_->second; }

 private:
  BaseIterator iter_;
  BaseIterator end_;
  std::function<bool(Edge)> filter_;
};

template <typename Vertex, typename Edge, typename BaseContainer>
class Graph {
 public:
  virtual size_t VerticesCount() const = 0;
  virtual size_t EdgesCount() const = 0;
  virtual std::list<Vertex> GetNeighbours(const Vertex&) const = 0;
  virtual std::list<Edge> GetEdges(const Vertex&) const = 0;
  virtual typename BaseContainer::const_iterator IterateEdges(
      const Vertex&) const = 0;
  virtual std::list<Vertex> GetVertices() const = 0;
  virtual GraphIterator<Vertex, Edge, typename BaseContainer::const_iterator>
  IterateNeighbours(const Vertex&, std::function<bool(Edge)>) = 0;
  virtual bool HasEdge(const Vertex&, const Vertex&) const = 0;
  virtual std::optional<Edge> GetEdge(const Vertex&, const Vertex&) const = 0;

  using VertexType = Vertex;
  using EdgeType = Edge;
};

template <typename Vertex = size_t, typename Edge = std::pair<Vertex, Vertex>>
class ListGraph : public Graph<Vertex, Edge, std::list<Edge>> {
 public:
  template <typename VContainer, typename EContainer>
  ListGraph(const VContainer& vertices, const EContainer& edges) {
    for (Vertex vertex : vertices) {
      adjacent_[vertex] = std::list<Edge>();
    }
    for (Edge edge : edges) {
      adjacent_[edge.first].push_back(edge);
      adjacent_[edge.second].push_back(edge);
      adjacent_[edge.second].back().first = edge.second;
      adjacent_[edge.second].back().second = edge.first;
    }
  }

  size_t VerticesCount() const { return adjacent_.size(); }

  size_t EdgesCount() const {
    size_t res = 0;
    for (auto adj : adjacent_) {
      res += adj.second.size();
    }
    return res / 2;
  }

  std::list<Vertex> GetNeighbours(const Vertex& vertex) const {
    std::list<Vertex> res;
    for (auto edge : adjacent_.at(vertex)) {
      res.push_back(edge.second);
    }
    return res;
  }

  std::list<Edge> GetEdges(const Vertex& vertex) const {
    return adjacent_.at(vertex);
  }

  std::list<Edge> GetEdges() const {
    std::list<Edge> res;
    for (auto edges : adjacent_) {
      for (auto edge : edges.second) {
        res.push_back(edge);
      }
    }
    return res;
  }

  GraphIterator<Vertex, Edge, typename std::list<Edge>::const_iterator>
  IterateNeighbours(const Vertex& vertex, std::function<bool(Edge)> filter) {
    return GraphIterator<Vertex, Edge,
                         typename std::list<Edge>::const_iterator>(
        adjacent_[vertex].begin(), adjacent_[vertex].end(), filter);
  }

  GraphIterator<Vertex, Edge, typename std::list<Edge>::const_iterator>
  IterateNeighbours(const Vertex& vertex) {
    return GraphIterator<Vertex, Edge,
                         typename std::list<Edge>::const_iterator>(
        adjacent_[vertex].begin(), adjacent_[vertex].end(),
        [](Edge edge) { return true; });
  }

  typename std::list<Edge>::const_iterator IterateEdges(
      const Vertex& vertex) const {
    return adjacent_.at(vertex).begin();
  }

  std::list<Vertex> GetVertices() const {
    std::list<Vertex> res;
    for (auto row : adjacent_) {
      res.push_back(row.first);
    }
    return res;
  }

  bool HasEdge(const Vertex& first, const Vertex& second) const {
    if (adjacent_.count(first) == 0) {
      return false;
    }
    return std::any_of(
        adjacent_.at(first).begin(), adjacent_.at(first).end(),
        [&second](const Edge& edge) { return edge.second == second; });
  }

  std::optional<Edge> GetEdge(const Vertex& first, const Vertex& second) const {
    if (adjacent_.count(first) == 0) {
      return std::nullopt;
    }
    for (Edge edge : adjacent_.at(first)) {
      if (edge.second == second) {
        return edge;
      }
    }
    return std::nullopt;
  }

 private:
  std::unordered_map<Vertex, std::list<Edge>> adjacent_;
};

template <typename Vertex = size_t, typename Weight = int>
struct WeightedEdge {
  Vertex first;
  Vertex second;
  Weight weight;

  bool operator==(const WeightedEdge<Vertex, Weight>& other) const {
    return ((first == other.first) && (second == other.second)) ||
           ((first == other.second) && (second == other.first));
  }

  bool operator!=(const WeightedEdge<Vertex, Weight>& other) const {
    return !(*this == other);
  }

  bool operator<(const WeightedEdge<Vertex, Weight>& other) const {
    return weight < other.weight;
  }
};

template <typename Vertex, typename Weight>
struct std::hash<WeightedEdge<Vertex, Weight>> {
  size_t operator()(const WeightedEdge<Vertex, Weight>& edge) const {
    return std::hash<size_t>{}(edge.first) + std::hash<size_t>{}(edge.second);
  }
};

template <typename Vertex = size_t, typename Edge = std::pair<Vertex, Vertex>>
class Tree : public ListGraph<Vertex, Edge> {
 public:
  template <typename VContainer, typename EContainer>
  Tree(const VContainer& vertices, const EContainer& edges)
      : ListGraph<Vertex, Edge>(vertices, edges) {}

  size_t EdgesCount() const override { return this->VerticesCount() - 1; }
};

template <typename Graph, typename Weight>
Weight GetTotalWeight(const Graph& graph) {
  Weight res = 0;
  for (auto edge : graph.GetEdges()) {
    res += edge.weight;
  }
  return res / 2;
}

template <typename Graph, typename Tree = Graph, typename Weight = int>
class BuildMST {
 public:
  virtual Tree operator()(const Graph& graph) const = 0;
};

template <typename Graph, typename Tree = Graph, typename Weight = int>
class PrimMST : public BuildMST<Graph, Tree, Weight> {
 public:
  Tree operator()(const Graph& graph) const {
    std::priority_queue<
        std::pair<Weight, typename Graph::EdgeType>,
        std::vector<std::pair<Weight, typename Graph::EdgeType>>,
        std::greater<std::pair<Weight, typename Graph::EdgeType>>>
        queue;
    std::unordered_set<typename Graph::EdgeType> used;
    std::vector<typename Graph::EdgeType> edges;
    for (auto edge : graph.GetEdges(graph.GetVertices().front())) {
      queue.push({edge.weight, edge});
    }
    while (edges.size() < graph.VerticesCount() - 1) {
      auto cur = queue.top().second;
      queue.pop();
      if (used.count(cur) != 0) {
        continue;
      }
      used.insert(cur);
      edges.push_back(cur);
      for (auto next : graph.GetEdges(cur.second)) {
        if (used.count(next) == 0) {
          queue.push({next.weight, next});
        }
      }
    }
    return Tree{graph.GetVertices(), edges};
  }
};

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

template <typename Graph, typename Tree = Graph, typename Weight = int>
class KruskalMST : public BuildMST<Graph, Tree, Weight> {
 public:
  Tree operator()(const Graph& graph) const {
    std::priority_queue<
        std::pair<Weight, typename Graph::EdgeType>,
        std::vector<std::pair<Weight, typename Graph::EdgeType>>,
        std::greater<std::pair<Weight, typename Graph::EdgeType>>>
        queue;
    std::vector<typename Graph::EdgeType> edges;
    std::unordered_set<typename Graph::EdgeType> used;
    DisjointSetUnion<typename Graph::VertexType> connect{graph.GetVertices()};
    for (auto edge : graph.GetEdges()) {
      queue.push({edge.weight, edge});
    }
    while (edges.size() < graph.VerticesCount() - 1) {
      auto cur = queue.top().second;
      queue.pop();
      if (used.count(cur) != 0) {
        continue;
      }
      used.insert(cur);
      if (connect.IsSameSet(cur.first, cur.second)) {
        continue;
      }
      connect.Unite(cur.first, cur.second);
      edges.push_back(cur);
    }
    return Tree{graph.GetVertices(), edges};
  }
};

int main() {
  size_t num_vertices;
  size_t num_edges;
  std::cin >> num_vertices >> num_edges;
  std::vector<size_t> vertices(num_vertices);
  std::iota(vertices.begin(), vertices.end(), 1);
  std::vector<WeightedEdge<>> edges(num_edges);
  for (size_t i = 0; i < num_edges; ++i) {
    std::cin >> edges[i].first >> edges[i].second >> edges[i].weight;
  }
  ListGraph<size_t, WeightedEdge<>> graph{vertices, edges};
  auto mst = PrimMST<decltype(graph), Tree<size_t, WeightedEdge<>>>{}(graph);
  std::cout << GetTotalWeight<decltype(mst), int>(mst) << std::endl;
}
