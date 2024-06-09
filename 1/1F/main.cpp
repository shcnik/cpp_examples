#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <optional>
#include <queue>
#include <set>
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

  DirectedGraph<Vertex, Edge> Transpose() const {
    DirectedGraph<Vertex, Edge> res(GetVertices(), std::vector<Edge>());
    for (auto list : edges_) {
      for (auto edge : list.second) {
        Edge new_edge = edge;
        new_edge.first = edge.second;
        new_edge.second = edge.first;
        res.edges_[edge.second].push_back(new_edge);
      }
    }
    return res;
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

  virtual void Clear() = 0;
};

template <typename Vertex = size_t, typename Edge = std::pair<size_t, size_t>>
class TopologicalSorter : public Visitor<Vertex, Edge> {
 public:
  void Finish(const Vertex& vertex,
              const Graph<Vertex, Edge>& /*unused*/) override {
    result_.push_front(vertex);
  }

  std::list<Vertex> SortResult() const { return result_; }

  void Clear() override { result_.clear(); }

 private:
  std::list<Vertex> result_;
};

template <typename Vertex = size_t, typename Edge = std::pair<size_t, size_t>>
class Enumerator : public Visitor<Vertex, Edge> {
 public:
  void Discover(const Vertex& vertex,
                const Graph<Vertex, Edge>& /*unused*/) override {
    vertices_.push_back(vertex);
  }

  const std::list<Vertex>& GetList() const { return vertices_; }

  void Clear() override { vertices_.clear(); }

 private:
  std::list<Vertex> vertices_;
};

template <typename Graph, typename Visitor>
class DFS {
  enum class VertexColor { WHITE, GRAY, BLACK };

 public:
  DFS(const Graph& graph, const Visitor& visitor = Visitor())
      : graph_(graph), visitor_(visitor) {}

  DFS<Graph, Visitor>& operator()() {
    for (auto vertex : graph_.GetVertices()) {
      colors_[vertex] = VertexColor::WHITE;
      visitor_.Initialize(vertex, graph_);
    }
    for (auto vertex : graph_.GetVertices()) {
      if (colors_[vertex] == VertexColor::WHITE) {
        Visit(vertex);
      }
    }
    return *this;
  }

  DFS<Graph, Visitor>& operator()(const typename Graph::VertexType& start) {
    if (colors_[start] != VertexColor::WHITE) {
      return *this;
    }
    /*
    for (auto vertex : graph_.GetVertices()) {
      colors_[vertex] = VertexColor::WHITE;
      visitor_.Initialize(vertex, graph_);
    }
    */
    Visit(start);
    return *this;
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

  const Graph& graph_;
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
  auto order = topsort.GetVisitor().SortResult();
  Enumerator<> enumer;
  std::vector<size_t> res(num_vertices, 0);
  size_t comp = 1;
  auto transp = graph.Transpose();
  for (auto vertex : order) {
    enumer.Clear();
    if (res[vertex - 1] != 0) {
      continue;
    }
    auto found = DFS<decltype(graph), decltype(enumer)>{transp, enumer}(vertex)
                     .GetVisitor()
                     .GetList();
    if (found.empty()) {
      continue;
    }
    for (auto cur : found) {
      if (res[cur - 1] != 0) {
        continue;
      }
      res[cur - 1] = comp;
    }
    ++comp;
  }
  std::cout << comp - 1 << std::endl;
  for (auto item : res) {
    std::cout << item << " ";
  }
  std::cout << std::endl;
}
