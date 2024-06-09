#include <iostream>
#include <list>
#include <queue>
#include <unordered_map>
#include <vector>

namespace Visitor {

template <class Vertex, class Edge>
class Visitor {
  virtual void Visit(const Edge& /*edge*/) = 0;
};

template <class Vertex, class Edge>
class AncestorVisitor : public Visitor<Vertex, Edge> {
 public:
  AncestorVisitor() = default;
  void Visit(const Edge& edge) override {
    ancestors_[edge.to] = {edge.from, edge.capacity - edge.flow};
  }

  std::unordered_map < Vertex, std::pair < Vertex, int64_t>>& Path() {
    return ancestors_;
  }

  int64_t GetMin(Vertex start, Vertex end) {
    if (start == end) {
      return 0;
    }
    int64_t min = std::numeric_limits<int64_t>::max();
    auto vertex = end;
    while (vertex != start) {
      auto pair = ancestors_[vertex];
      auto next = pair.first;
      auto cur = pair.second;
      min = std::min(min, cur);
      vertex = next;
    }

    return min;
  }

  AncestorVisitor(Vertex start) {
    ancestors_[start] = {start, std::numeric_limits<int64_t>::max()};
  }
  ~AncestorVisitor() = default;

 private:
  std::unordered_map<Vertex, std::pair<Vertex, int64_t>> ancestors_;
};

}  // namespace Visitor

template <class Vertex>
struct Edge {
  Vertex from;
  Vertex to;
  int64_t capacity;
  int64_t flow;
  Edge(Vertex from, Vertex to, int64_t capacity, int64_t flow)
      : from(from), to(to), capacity(capacity), flow(flow) {}
  Edge() = default;
  int64_t CurrentCapacity() { return capacity - flow; }
};

template <class Vertex, class Edge>
class EdgeGraph {
 private:
  std::unordered_map<uint64_t, Edge> edge_from_id_;
  std::vector<std::vector<Vertex>> incident_vertices_;
  std::vector<Vertex> vertices_;

 public:
  using VertexType = Vertex;
  using EdgeType = Edge;

  EdgeGraph(int num_vertices) : incident_vertices_(num_vertices + 1, std::vector<Vertex>()) {
    vertices_.push_back(-1);
    for (int i = 1; i <= num_vertices; ++i) {
      vertices_.push_back(i);
    }
  }

  size_t GetVertecesCount() { return vertices_.size() - 1; }

  std::vector<Vertex>& GetIncidents(Vertex v) { return incident_vertices_[v]; }
  Edge& GetEdge(uint64_t edge_id) { return edge_from_id_[edge_id]; }

  Edge& GetEdge(Vertex from, Vertex to) {
    return edge_from_id_[from * kShift + to];
  }

  void AddEdge(Vertex from, Vertex to, int64_t capacity,
               int64_t flow) {  // {from, to} -> edge[from « 32 | to]
    if (edge_from_id_.find(from * kShift + to) == edge_from_id_.end()) {
      edge_from_id_[from * kShift + to] = Edge(from, to, capacity, flow);
      incident_vertices_[from].push_back(to);
    } else {
      edge_from_id_[from * kShift + to].capacity += capacity;
    }
  }

  static constexpr Vertex kShift = 1337;
};

template <typename Graph>
void ReadEdges(Graph& graph, size_t num_edges) {
  for (size_t i = 0; i < num_edges; ++i) {
    typename Graph::VertexType from;
    typename Graph::VertexType to;
    int64_t capacity;
    std::cin >> from >> to >> capacity;
    graph.AddEdge(from, to, capacity, 0);
    graph.AddEdge(to, from, 0, 0);
  }
}

template <class Vertex, class Edge, class Visitor>
int64_t FindIncreasingPath(EdgeGraph<Vertex, Edge>& graph, Vertex start,
                           Vertex end) {
  std::queue<Vertex> bfs_queue;
  bfs_queue.push(start);
  std::vector<int64_t> dist(graph.GetVertecesCount() + 1,
                            std::numeric_limits<int64_t>::max());
  dist[start] = 0;
  Visitor visitor;
  while (!bfs_queue.empty()) {
    Vertex vertex = bfs_queue.front();
    bfs_queue.pop();
    for (auto next : graph.GetIncidents(vertex)) {
      auto edge = graph.GetEdge(vertex, next);
      if (edge.CurrentCapacity() > 0 and
          dist[edge.to] == std::numeric_limits<int64_t>::max()) {
        bfs_queue.push(edge.to);
        dist[edge.to] = dist[edge.from] + 1;
        visitor.Visit(edge);
      }
      if (next == end) {
        break;
      }
    }
  }
  Vertex next = end;
  int64_t delta =
      dist[end] != std::numeric_limits<int64_t>::max() ? visitor.GetMin(start, end) : 0;
  if (delta > 0) {
    while (next != start) {
      auto pair = visitor.Path()[next];
      auto vertex = pair.first;
      graph.GetEdge(vertex, next).flow += delta;
      graph.GetEdge(next, vertex).flow -= delta;
      next = vertex;
    }
  }
  return delta;
}

template <class Vertex, class Edge>
int64_t FindMaxFlow(EdgeGraph<Vertex, Edge>& graph, Vertex start, Vertex end) {
  int64_t sum = 0;
  auto tmp =
      FindIncreasingPath<Vertex, Edge, Visitor::AncestorVisitor<Vertex, Edge>>(
          graph, start, end);
  while (tmp) {
    sum += tmp;
    tmp =
        FindIncreasingPath<Vertex, Edge,
                           Visitor::AncestorVisitor<Vertex, Edge>>(graph, start, end);
  }
  return sum;
}

int main() {
  size_t num_vertices;
  size_t num_edges;
  std::cin >> num_vertices >> num_edges;
  EdgeGraph<int64_t, Edge<int64_t>> graph(num_vertices);
  ReadEdges(graph, num_edges);
  std::cout << FindMaxFlow<int64_t>(graph, 1, num_vertices);
}
