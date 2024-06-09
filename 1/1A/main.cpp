// 82825957
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
  IterateNeighbours(const Vertex&, std::function<bool(Edge)>) = 0;
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
      matrix_[edge.second][edge.first] = edge;
      matrix_[edge.first][edge.second] = edge;
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
    for (auto edge : matrix_[vertex]) {
      res.push_back(edge.second);
    }
    return res;
  }

  GraphIterator<Vertex, Edge,
                typename std::unordered_map<Vertex, Edge>::const_iterator>
  IterateNeighbours(const Vertex& vertex) {
    return GraphIterator<
        Vertex, Edge,
        typename std::unordered_map<Vertex, Edge>::const_iterator>(
        matrix_[vertex].begin(), matrix_[vertex].end(),
        [](Edge /*unused*/) { return true; });
  }

  GraphIterator<Vertex, Edge,
                typename std::unordered_map<Vertex, Edge>::const_iterator>
  IterateNeighbours(const Vertex& vertex, std::function<bool(Edge)> filter) {
    return GraphIterator<
        Vertex, Edge,
        typename std::unordered_map<Vertex, Edge>::const_iterator>(
        matrix_[vertex].begin(), matrix_[vertex].end(), filter);
  }

  std::list<Vertex> GetVertices() const {
    std::list<Vertex> res;
    for (auto row : matrix_) {
      res.push_back(row.first);
    }
    return res;
  }

  bool HasEdge(const Vertex& first, const Vertex& second) const {
    return (matrix_.count(first) != 0) && (matrix_[first].count(second) != 0);
  }

  std::optional<Edge> GetEdge(const Vertex& first, const Vertex& second) const {
    if (!HasEdge(first, second)) {
      return std::nullopt;
    }
    return matrix_[first][second];
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
      //  разворот ребра, чтобы в second была всегда вершина, куда мы переходим
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
        [](Edge /*unused*/) { return true; });
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

template <typename Vertex, typename Edge>
class Visitor {
 public:
  virtual void Initialize(const Vertex& /*unused*/) {}

  virtual void ExamineVertex(const Vertex& /*unused*/) {}

  virtual void ExamineEdge(const Edge& /*unused*/) {}

  virtual void TreeEdge(const Edge& /*unused*/) {}

  virtual void Discover(const Vertex& /*unused*/) {}

  virtual void NonTreeEdge(const Edge& /*unused*/) {}

  virtual void GrayTarget(const Vertex& /*unused*/) {}

  virtual void BlackTarget(const Vertex& /*unused*/) {}

  virtual void Finish(const Vertex& /*unused*/) {}
};

template <typename Vertex = size_t, typename Edge = std::pair<size_t, size_t>>
class ParentVisitor : public Visitor<Vertex, Edge> {
 public:
  void TreeEdge(const Edge& edge) override {
    parents_[edge.second] = edge.first;
  }

  std::optional<Vertex> GetParent(const Vertex& vertex) const {
    if (parents_.count(vertex) == 0) {
      return std::nullopt;
    }
    return parents_[vertex];
  }

  std::vector<Vertex> GetPath(const Vertex& first, const Vertex& second) const {
    if (second == first) {
      return {first};
    }
    if (parents_.count(second) == 0) {
      return std::vector<Vertex>();
    }
    std::vector<Vertex> res;
    res.push_back(second);
    while (res.back() != first) {
      res.push_back(parents_.at(res.back()));
    }
    std::reverse(res.begin(), res.end());
    return res;
  }

  template <typename Graph>
  std::vector<Edge> GetEdges(const Graph& graph, const Vertex& second) const {
    std::vector<Edge> res;
    Vertex cur = second;
    while (parents_.count(cur) != 0) {
      res.push_back(graph.GetEdge(parents_.at(cur), cur));
      cur = parents_[cur];
    }
    std::reverse(res.begin(), res.end());
    return res;
  }

 private:
  std::unordered_map<Vertex, Vertex> parents_;
};

template <typename Graph, typename Visitor>
class BFS {
  enum class VertexColor { WHITE, GRAY, BLACK };

 public:
  BFS(const Graph& graph, const Visitor& visitor = Visitor())
      : graph_(graph), visitor_(visitor) {}

  void operator()(typename Graph::VertexType const& start) {
    std::unordered_map<typename Graph::VertexType, VertexColor> colors;
    std::queue<typename Graph::VertexType> vertices;
    for (auto vertex : graph_.GetVertices()) {
      visitor_.Initialize(vertex);
      colors[vertex] = VertexColor::WHITE;
    }
    if (colors.count(start) == 0) {
      return;
    }
    colors[start] = VertexColor::GRAY;
    visitor_.Discover(start);
    vertices.push(start);
    while (!vertices.empty()) {
      auto cur = vertices.front();
      visitor_.ExamineVertex(cur);
      vertices.pop();
      for (auto next : graph_.GetNeighbours(cur)) {
        auto edge = *(graph_.GetEdge(cur, next));
        visitor_.ExamineEdge(edge);
        if (colors[next] == VertexColor::WHITE) {
          visitor_.TreeEdge(edge);
          colors[next] = VertexColor::GRAY;
          vertices.push(next);
        } else {
          visitor_.NonTreeEdge(edge);
          if (colors[next] == VertexColor::GRAY) {
            visitor_.GrayTarget(next);
          } else {
            visitor_.BlackTarget(next);
          }
        }
      }
      colors[cur] = VertexColor::BLACK;
      visitor_.Finish(cur);
    }
  }

  const Visitor& GetVisitor() const { return visitor_; }

 private:
  Graph graph_;
  Visitor visitor_;
};

int main() {
  size_t num_vertices;
  size_t num_edges;
  std::cin >> num_vertices >> num_edges;
  size_t start;
  size_t finish;
  std::cin >> start >> finish;
  std::vector<std::pair<size_t, size_t>> edges(num_edges);
  for (size_t i = 0; i < num_edges; ++i) {
    std::cin >> edges[i].first >> edges[i].second;
  }
  std::vector<size_t> vertices(num_vertices);
  for (size_t i = 0; i < num_vertices; ++i) {
    vertices[i] = i;
  }
  ListGraph<> graph(vertices, edges);
  BFS<ListGraph<>, ParentVisitor<>> finder(graph);
  finder(start);
  std::vector<size_t> path = finder.GetVisitor().GetPath(start, finish);
  if (path.empty()) {
    std::cout << "-1" << std::endl;
    return 0;
  }
  std::cout << (path.size() - 1) << std::endl;
  for (size_t planet : path) {
    std::cout << planet << " ";
  }
  std::cout << std::endl;
  return 0;
}
