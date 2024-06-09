#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <optional>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>

template <typename Vertex = size_t, typename Edge = std::pair<Vertex, Vertex>>
class Graph {
 public:
  virtual size_t VerticesCount() const = 0;
  virtual size_t EdgesCount() const = 0;
  virtual std::list<Vertex> GetNeighbours(const Vertex&) const = 0;
  virtual std::list<Vertex> GetVertices() const = 0;
  virtual bool HasEdge(const Vertex&, const Vertex&) const = 0;
  virtual std::optional<Edge> GetEdge(const Vertex&, const Vertex&) const = 0;

  using VertexType = Vertex;
  using EdgeType = Edge;
};

template <typename Vertex = size_t, typename Edge = std::pair<size_t, size_t>>
class DirectedGraph : public Graph<Vertex, Edge> {
 public:
  template <typename VContainer, typename EContainer>
  DirectedGraph(const VContainer& vertices, const EContainer& edges) {
    for (auto vertex : vertices) {
      edges_[vertex] = std::list<Edge>();
    }
    for (auto edge : edges) {
      edges_[edge.first].push_back(edge);
    }
  }

  size_t VerticesCount() const { return edges_.size(); }

  size_t EdgesCount() const {
    size_t res = 0;
    for (auto row : edges_) {
      res += row.second.size();
    }
    return res;
  }

  std::list<Vertex> GetNeighbours(const Vertex& vertex) const {
    std::list<Vertex> res;
    for (auto edge : edges_.at(vertex)) {
      res.push_back(edge.second);
    }
    return res;
  }

  std::list<Vertex> GetVertices() const {
    std::list<Vertex> res;
    for (auto row : edges_) {
      res.push_back(row.first);
    }
    return res;
  }

  bool HasEdge(const Vertex& first, const Vertex& second) const {
    for (auto edge : edges_.at(first)) {
      if (edge.second == second) {
        return true;
      }
    }
    return false;
  }

  std::optional<Edge> GetEdge(const Vertex& first, const Vertex& second) const {
    for (auto edge : edges_.at(first)) {
      if (edge.second == second) {
        return edge;
      }
    }
    return std::nullopt;
  }

 private:
  std::unordered_map<Vertex, std::list<Edge>> edges_;
};

template <typename Vertex, typename Edge>
class Visitor {
 public:
  virtual void Initialize(const Vertex& /*unused*/,
                          const Graph<Vertex, Edge>& /*unused*/) {}

  virtual void ExamineVertex(const Vertex& /*unused*/,
                             const Graph<Vertex, Edge>& /*unused*/) {}

  virtual void ExamineEdge(const Edge& /*unused*/,
                           const Graph<Vertex, Edge>& /*unused*/) {}

  virtual void TreeEdge(const Edge& /*unused*/,
                        const Graph<Vertex, Edge>& /*unused*/) {}

  virtual void Discover(const Vertex& /*unused*/,
                        const Graph<Vertex, Edge>& /*unused*/) {}

  virtual void BackEdge(const Edge& /*unused*/,
                        const Graph<Vertex, Edge>& /*unused*/) {}

  virtual void GrayTarget(const Vertex& /*unused*/,
                          const Graph<Vertex, Edge>& /*unused*/) {}

  virtual void BlackTarget(const Vertex& /*unused*/,
                           const Graph<Vertex, Edge>& /*unused*/) {}

  virtual void Finish(const Vertex& /*unused*/,
                      const Graph<Vertex, Edge>& /*unused*/) {}
};

template <typename Vertex = size_t, typename Edge = std::pair<size_t, size_t>>
class TopologicalSorter : public Visitor<Vertex, Edge> {
 public:
  void Finish(const Vertex& vertex, const Graph<Vertex, Edge>& /*unused*/) {
    result_.push_front(vertex);
  }

  void GrayTarget(const Vertex& /*unused*/,
                  const Graph<Vertex, Edge>& /*unused*/) {
    sort_exists_ = false;
  }

  std::optional<std::list<Vertex>> SortResult() const {
    if (!sort_exists_) {
      return std::nullopt;
    }
    return result_;
  }

 private:
  std::list<Vertex> result_;
  bool sort_exists_ = true;
};

template <typename Graph, typename Visitor>
class DFS {
  enum class VertexColor { WHITE, GRAY, BLACK };

 public:
  DFS(const Graph& graph, const Visitor& visitor = Visitor())
      : graph_(graph), visitor_(visitor) {}

  void operator()() {
    for (auto vertex : graph_.GetVertices()) {
      colors_[vertex] = VertexColor::WHITE;
      visitor_.Initialize(vertex, graph_);
    }
    for (auto vertex : graph_.GetVertices()) {
      if (colors_[vertex] == VertexColor::WHITE) {
        Visit(vertex);
      }
    }
  }

  const Visitor& GetVisitor() const { return visitor_; }

 private:
  void Visit(const typename Graph::VertexType& vertex) {
    visitor_.Discover(vertex, graph_);
    colors_[vertex] = VertexColor::GRAY;
    visitor_.ExamineVertex(vertex, graph_);
    for (auto adj : graph_.GetNeighbours(vertex)) {
      visitor_.ExamineEdge(*graph_.GetEdge(vertex, adj), graph_);
      if (colors_[adj] == VertexColor::WHITE) {
        visitor_.TreeEdge(*graph_.GetEdge(vertex, adj), graph_);
        Visit(adj);
      } else {
        visitor_.BackEdge(*graph_.GetEdge(vertex, adj), graph_);
        if (colors_[adj] == VertexColor::GRAY) {
          visitor_.GrayTarget(adj, graph_);
        } else {
          visitor_.BlackTarget(adj, graph_);
        }
      }
    }
    colors_[vertex] = VertexColor::BLACK;
    visitor_.Finish(vertex, graph_);
  }

  Graph graph_;
  Visitor visitor_;
  std::unordered_map<typename Graph::VertexType, VertexColor> colors_;
};

int main() {
  size_t num_vertices, num_edges;
  std::cin >> num_vertices >> num_edges;
  std::vector<size_t> vertices(num_vertices);
  for (size_t i = 0; i < num_vertices; ++i) {
    vertices[i] = i + 1;
  }
  std::vector<std::pair<size_t, size_t>> edges(num_edges);
  for (size_t i = 0; i < num_edges; ++i) {
    std::cin >> edges[i].first >> edges[i].second;
  }
  DirectedGraph<> graph(vertices, edges);
  DFS<decltype(graph), TopologicalSorter<>> topsort(graph);
  topsort();
  auto res = topsort.GetVisitor().SortResult();
  if (!res.has_value()) {
    std::cout << -1 << std::endl;
  } else {
    for (auto vertex : *res) {
      std::cout << vertex << " ";
    }
    std::cout << std::endl;
  }
}
