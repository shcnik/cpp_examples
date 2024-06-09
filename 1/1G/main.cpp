#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <optional>
#include <queue>
#include <unordered_map>
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

template <typename Vertex, typename Edge>
class GraphIterator<Vertex, Edge,
                    typename std::unordered_map<Vertex, Edge>::const_iterator> {
  using BaseIterator =
      typename std::unordered_map<Vertex, Edge>::const_iterator;

 public:
  GraphIterator(BaseIterator start, BaseIterator end,
                std::function<bool(Edge)> filter)
      : iter_(start), end_(end), filter_(filter) {
    while ((iter_ != end_) && (!filter_(iter_->second))) {
      ++iter_;
    }
  }

  GraphIterator<Vertex, Edge, BaseIterator>& operator++() {
    do {
      ++iter_;
    } while ((iter_ != end_) && !filter_(iter_->second));
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

  const Vertex& operator*() { return iter_->first; }

 private:
  BaseIterator iter_;
  BaseIterator end_;
  std::function<bool(Edge)> filter_;
};

template <typename Vertex, typename Edge>
class GraphIterator<Vertex, Edge,
                    typename std::map<Vertex, Edge>::const_iterator> {
  using BaseIterator = typename std::map<Vertex, Edge>::const_iterator;

 public:
  GraphIterator(BaseIterator start, BaseIterator end,
                std::function<bool(Edge)> filter)
      : iter_(start), end_(end), filter_(filter) {
    while ((iter_ != end_) && (!filter_(iter_->second))) {
      ++iter_;
    }
  }

  GraphIterator& operator++() {
    do {
      ++iter_;
    } while ((iter_ != end_) && !filter_(iter_->second));
  }

  bool operator==(const GraphIterator& other) const {
    return iter_ == other.iter_;
  }

  bool operator!=(const GraphIterator& other) const {
    return iter_ != other.iter_;
  }

  // NOLINTNEXTLINE
  GraphIterator begin() const { return *this; }

  // NOLINTNEXTLINE
  GraphIterator end() const { return Iterator(end_, end_, filter_); }

  const Vertex& operator*() { return iter_->first; }

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
  virtual std::list<Vertex> GetVertices() const = 0;
  virtual GraphIterator<Vertex, Edge, typename BaseContainer::const_iterator>
  IterateNeighbours(const Vertex&, std::function<bool(Edge)>) const = 0;
  virtual bool HasEdge(const Vertex&, const Vertex&) const = 0;
  virtual std::optional<Edge> GetEdge(const Vertex&, const Vertex&) const = 0;

  using VertexType = Vertex;
  using EdgeType = Edge;
};

template <typename Vertex = size_t, typename Edge = std::pair<Vertex, Vertex>>
class MatrixGraph
    : public Graph<Vertex, Edge, std::unordered_map<Vertex, Edge>> {
 public:
  template <typename VContainer, typename EContainer>
  MatrixGraph(const VContainer& vertices, const EContainer& edges) {
    for (Vertex vertex : vertices) {
      matrix_[vertex] = std::unordered_map<Vertex, Edge>();
    }
    for (Edge edge : edges) {
      matrix_[edge.second][edge.first] = matrix_[edge.first][edge.second] =
          edge;
      matrix_[edge.second][edge.first].first = edge.second;
      matrix_[edge.second][edge.first].second = edge.first;
    }
  }

  size_t VerticesCount() const { return matrix_.size(); }
  size_t EdgesCount() const {
    size_t res = 0;
    for (auto row : matrix_) {
      res += row.second.size();
    }
    return res / 2;
  }

  std::list<Vertex> GetNeighbours(const Vertex& vertex) const {
    std::list<Vertex> res;
    for (auto edge : matrix_.at(vertex)) {
      res.push_back(edge.first);
    }
    return res;
  }

  GraphIterator<Vertex, Edge,
                typename std::unordered_map<Vertex, Edge>::const_iterator>
  IterateNeighbours(const Vertex& vertex) const {
    return GraphIterator<
        Vertex, Edge,
        typename std::unordered_map<Vertex, Edge>::const_iterator>(
        matrix_.at(vertex).begin(), matrix_.at(vertex).end(),
        [](Edge edge) { return true; });
  }

  GraphIterator<Vertex, Edge,
                typename std::unordered_map<Vertex, Edge>::const_iterator>
  IterateNeighbours(const Vertex& vertex,
                    std::function<bool(Edge)> filter) const {
    return GraphIterator<
        Vertex, Edge,
        typename std::unordered_map<Vertex, Edge>::const_iterator>(
        matrix_.at(vertex).begin(), matrix_.at(vertex).end(), filter);
  }

  std::list<Vertex> GetVertices() const {
    std::list<Vertex> res;
    for (auto row : matrix_) {
      res.push_back(row.first);
    }
    return res;
  }

  bool HasEdge(const Vertex& first, const Vertex& second) const {
    return (matrix_.count(first) != 0) &&
           (matrix_.at(first).count(second) != 0);
  }

  std::optional<Edge> GetEdge(const Vertex& first, const Vertex& second) const {
    if (!HasEdge(first, second)) {
      return std::nullopt;
    }
    return matrix_.at(first).at(second);
  }

 private:
  std::unordered_map<Vertex, std::unordered_map<Vertex, Edge>> matrix_;
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

  GraphIterator<Vertex, Edge, typename std::list<Edge>::const_iterator>
  IterateNeighbours(const Vertex& vertex,
                    std::function<bool(Edge)> filter) const {
    return GraphIterator<Vertex, Edge,
                         typename std::list<Edge>::const_iterator>(
        adjacent_.at(vertex).begin(), adjacent_.at(vertex).end(), filter);
  }

  GraphIterator<Vertex, Edge, typename std::list<Edge>::const_iterator>
  IterateNeighbours(const Vertex& vertex) const {
    return GraphIterator<Vertex, Edge,
                         typename std::list<Edge>::const_iterator>(
        adjacent_.at(vertex).begin(), adjacent_.at(vertex).end(),
        [](Edge edge) { return true; });
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
  virtual void Initialize(const typename Graph::VertexType& /*unused*/,
                          const Graph& /*unused*/) {}

  virtual void ExamineVertex(const typename Graph::VertexType& /*unused*/,
                             const Graph& /*unused*/) {}

  virtual void ExamineEdge(const typename Graph::EdgeType& /*unused*/,
                           const Graph& /*unused*/) {}

  virtual void TreeEdge(const typename Graph::EdgeType& /*unused*/,
                        const Graph& /*unused*/) {}

  virtual void Discover(const typename Graph::VertexType& /*unused*/,
                        const Graph& /*unused*/) {}

  virtual void BackEdge(const typename Graph::EdgeType& /*unused*/,
                        const Graph& /*unused*/) {}

  virtual void GrayTarget(const typename Graph::VertexType& /*unused*/,
                          const Graph& /*unused*/) {}

  virtual void BlackTarget(const typename Graph::VertexType& /*unused*/,
                           const Graph& /*unused*/) {}

  virtual void FinishVertex(const typename Graph::VertexType& /*unused*/,
                            const Graph& /*unused*/) {}

  virtual void FinishEdge(const typename Graph::EdgeType& /*unused*/,
                          const Graph& /*unused*/) {}
};

template <typename Graph = ListGraph<>>
class BridgesFinder : public Visitor<Graph> {
 public:
  void Initialize(const typename Graph::VertexType& vertex,
                  const Graph& /*unused*/) override {
    t_in_[vertex] = std::numeric_limits<size_t>::max();
    f_up_[vertex] = std::numeric_limits<size_t>::max();
  }

  void Discover(const typename Graph::VertexType& vertex,
                const Graph& /*unused*/) override {
    f_up_[vertex] = t_in_[vertex] = time_++;
  }

  void BackEdge(const typename Graph::EdgeType& edge,
                const Graph& /*unused*/) override {
    f_up_[edge.first] = std::min(f_up_[edge.first], t_in_[edge.second]);
  }

  void FinishEdge(const typename Graph::EdgeType& edge,
                  const Graph& /*unused*/) override {
    f_up_[edge.first] = std::min(f_up_[edge.first], f_up_[edge.second]);
    if (f_up_[edge.second] > t_in_[edge.first]) {
      if (edge.repeat > 1) {
        return;
      }
      bridges_.insert(std::lower_bound(bridges_.begin(), bridges_.end(), edge),
                      edge);
    }
  }

  const std::vector<typename Graph::EdgeType>& GetBridges() const {
    return bridges_;
  }

 private:
  std::unordered_map<typename Graph::VertexType, size_t> t_in_;
  std::unordered_map<typename Graph::VertexType, size_t> f_up_;
  std::vector<typename Graph::EdgeType> bridges_;
  size_t time_ = 0;
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
        Visit(adj, vertex);
        visitor_.FinishEdge(*graph_.GetEdge(vertex, adj), graph_);
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
    visitor_.FinishVertex(vertex, graph_);
  }

  void Visit(const typename Graph::VertexType& vertex,
             const typename Graph::VertexType& parent) {
    visitor_.Discover(vertex, graph_);
    colors_[vertex] = VertexColor::GRAY;
    visitor_.ExamineVertex(vertex, graph_);
    for (auto adj : graph_.GetNeighbours(vertex)) {
      if (adj == parent) {
        continue;
      }
      visitor_.ExamineEdge(*graph_.GetEdge(vertex, adj), graph_);
      if (colors_[adj] == VertexColor::WHITE) {
        visitor_.TreeEdge(*graph_.GetEdge(vertex, adj), graph_);
        Visit(adj, vertex);
        visitor_.FinishEdge(*graph_.GetEdge(vertex, adj), graph_);
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
    visitor_.FinishVertex(vertex, graph_);
  }

  const Graph& graph_;
  Visitor visitor_;
  std::unordered_map<typename Graph::VertexType, VertexColor> colors_;
};

template <typename Vertex = size_t>
struct Edge {
  uint32_t number;
  Vertex first;
  Vertex second;
  uint32_t repeat = 1;

  bool operator==(const Edge& other) const { return number == other.number; }

  bool operator<(const Edge& other) const { return number < other.number; }
};

int main() {
  size_t num_vertices, num_edges;
  std::cin >> num_vertices >> num_edges;
  std::vector<uint16_t> vertices(num_vertices);
  for (size_t i = 0; i < num_vertices; ++i) {
    vertices[i] = i + 1;
  }
  std::vector<Edge<uint16_t>> edges(num_edges);
  for (size_t i = 0; i < num_edges; ++i) {
    edges[i].number = i + 1;
    std::cin >> edges[i].first >> edges[i].second;
  }
  MatrixGraph<uint16_t, Edge<uint16_t>> graph(vertices, edges);
  DFS<decltype(graph), BridgesFinder<decltype(graph)>> finder(graph);
  finder();
  const std::vector<Edge<uint16_t>>& res = finder.GetVisitor().GetBridges();
  std::cout << res.size() << std::endl;
  for (auto bridge : res) {
    std::cout << bridge.number << std::endl;
  }
}
