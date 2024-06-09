#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <numeric>
#include <optional>
#include <queue>
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

template <typename Graph>
class Visitor {
 public:
  virtual void Initialize(const Graph& /*unused*/) {}
  virtual void Start(const Graph& /*unused*/,
                     const typename Graph::VertexType& /*unused*/) {}
  virtual void Discover(const Graph& /*unused*/,
                        const typename Graph::VertexType& /*unused*/) {}
  virtual void ExamineVertex(const Graph& /*unused*/,
                             const typename Graph::VertexType& /*unused*/) {}
  virtual void ExamineEdge(const Graph& /*unused*/,
                           const typename Graph::EdgeType& /*unused*/) {}
  virtual void Optimize(const Graph& /*unused*/,
                        const typename Graph::EdgeType& /*unused*/) {}
};

template <typename Graph>
class DistanceVisitor : public Visitor<Graph> {
 public:
  void Start(const Graph& /*unused*/,
             const typename Graph::VertexType& start) override {
    distances_[start] = 0;
  }

  void Optimize(const Graph& /*unused*/,
                const typename Graph::EdgeType& edge) override {
    parents_[edge.second] = edge.first;
    distances_[edge.second] = edge.weight;
  }

  std::optional<typename Graph::VertexType> GetParent(
      const typename Graph::VertexType& vertex) const {
    if (parents_.count(vertex) == 0) {
      return std::nullopt;
    }
    return parents_.at(vertex);
  }

  typename Graph::EdgeType::WeightType GetDistance(
      const typename Graph::VertexType& vertex) const {
    if (distances_.count(vertex) == 0) {
      return std::numeric_limits<typename Graph::EdgeType::WeightType>::max();
    }
    return distances_.at(vertex);
  }

 private:
  std::unordered_map<typename Graph::VertexType,
                     typename Graph::EdgeType::WeightType>
      distances_;
  std::unordered_map<typename Graph::VertexType, typename Graph::VertexType>
      parents_;
};

template <typename Vertex = size_t, typename Weight = int>
struct WeightedEdge {
  Vertex first;
  Vertex second;
  Weight weight;

  using WeightType = Weight;
};

template <typename Graph, typename Visitor>
class DijkstraSearch {
 public:
  using VertexType = typename Graph::VertexType;
  using EdgeType = typename Graph::EdgeType;
  using WeightType = typename EdgeType::WeightType;

  std::unordered_map<VertexType, WeightType> operator()(
      const Graph& graph, const VertexType& start) {
    std::unordered_map<VertexType, WeightType> dist;
    dist[start] = 0;
    std::unordered_set<VertexType> used;
    std::priority_queue<std::pair<WeightType, VertexType>,
                        std::vector<std::pair<WeightType, VertexType>>,
                        std::greater<std::pair<WeightType, VertexType>>>
        queue;
    queue.push({0, start});
    visitor_.Start(graph, start);
    while (!queue.empty()) {
      VertexType cur = queue.top().second;
      queue.pop();
      if (used.count(cur) != 0) {
        continue;
      }
      used.insert(cur);
      visitor_.ExamineVertex(graph, cur);
      for (auto edge : graph.GetEdges(cur)) {
        visitor_.ExamineEdge(graph, edge);
        if ((dist.count(edge.second) == 0) ||
            (dist[edge.second] > dist[edge.first] + edge.weight)) {
          dist[edge.second] = dist[edge.first] + edge.weight;
          queue.push({dist[edge.second], edge.second});
          visitor_.Optimize(graph, edge);
        }
      }
    }
    return dist;
  }

  const Visitor& GetVisitor() const { return visitor_; }

 private:
  Visitor visitor_;
};

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);
  size_t num_graphs;
  std::cin >> num_graphs;
  for (size_t t = 0; t < num_graphs; ++t) {
    size_t num_vertices;
    size_t num_edges;
    std::cin >> num_vertices >> num_edges;
    std::vector<size_t> vertices(num_vertices);
    std::iota(vertices.begin(), vertices.end(), 0);
    std::vector<WeightedEdge<>> edges(num_edges);
    for (size_t i = 0; i < edges.size(); ++i) {
      std::cin >> edges[i].first >> edges[i].second >> edges[i].weight;
    }
    ListGraph<size_t, WeightedEdge<>> graph(vertices, edges);
    size_t start;
    std::cin >> start;
    auto res =
        DijkstraSearch<decltype(graph), DistanceVisitor<decltype(graph)>>{}(
            graph, start);
    constexpr int kInfinity = 2009000999;
    for (size_t i = 0; i < num_vertices; ++i) {
      if (res.count(i) == 0) {
        std::cout << kInfinity << " ";
      } else {
        std::cout << res[i] << " ";
      }
    }
    std::cout << std::endl;
  }
}
