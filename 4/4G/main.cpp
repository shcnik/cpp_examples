#include <algorithm>
#include <iostream>
#include <iterator>
#include <limits>
#include <queue>
#include <vector>

static constexpr int64_t kInfinity = std::numeric_limits<int64_t>::max();

using Vertex = size_t;

struct Edge {
  Vertex from;
  Vertex to;
  int64_t flow;
  int64_t capacity;
  bool valid = false;

  Edge(Vertex from, Vertex to, int64_t capacity)
      : from(from), to(to), flow(0), capacity(capacity) {}

  Edge(Vertex from, Vertex to, int64_t flow, int64_t capacity)
      : from(from), to(to), flow(flow), capacity(capacity) {}
};

class Graph {
  enum class EdgeColors { kWhite, kGrey, kBlack };

 public:
  Graph(size_t num_vertices)
      : graph_(num_vertices + 1, std::vector<size_t>()) {}

  void AddEdge(size_t from, size_t to, int64_t capacity) {
    edges_.emplace_back(from, to, capacity);
    size_t edge_id = edges_.size() - 1;
    edges_.emplace_back(to, from, 0);
    size_t back_edge_id = edges_.size() - 1;
    graph_[from].push_back(edge_id);
    graph_[to].push_back(back_edge_id);
  }

  std::pair<int64_t, std::vector<Edge>> FindMaxFlow(Vertex start, Vertex end) {
    int64_t max_flow = 0;
    while (true) {
      FilterEdges(start, end);
      if (!BFS(start, end)) {
        break;
      }
      int64_t delta = 0;
      do {
        std::vector<EdgeColors> colors(graph_.size(), EdgeColors::kWhite);
        delta = DFS(colors, start, end, kInfinity);
        max_flow += delta;
      } while (delta > 0);
    }
    return std::make_pair(max_flow, edges_);
  }

 private:
  void FilterEdges(Vertex start, Vertex end) {
    for (size_t i = 0; i < edges_.size(); ++i) {
      edges_[i].valid = false;
    }
    std::vector<bool> used(graph_.size(), false);
    std::queue<Vertex> queue;
    queue.push(end);
    while (!queue.empty()) {
      Vertex vertex = queue.front();
      queue.pop();
      if (used[vertex]) {
        continue;
      }
      used[vertex] = true;
      if (vertex == start) {
        continue;
      }
      for (size_t id : graph_[vertex]) {
        if (id % 2 == 0) {
          continue;
        }
        if (edges_[id ^ 1].capacity - edges_[id ^ 1].flow == 0) {
          continue;
        }
        edges_[id ^ 1].valid = true;
        edges_[id].valid = true;
        queue.push(edges_[id].to);
      }
    }
    for (Vertex i = 1; i < graph_.size(); ++i) {
      graph_[i].resize(std::distance(
          graph_[i].begin(),
          std::remove_if(
              graph_[i].begin(), graph_[i].end(),
              [&](size_t edge_id) { return !(this->edges_[edge_id].valid); })));
    }
  }

  bool BFS(Vertex start, Vertex end) {
    dist.assign(graph_.size(), kInfinity);
    std::vector<bool> used(graph_.size(), false);
    std::queue<Vertex> queue;
    dist[start] = 0;
    queue.push(start);
    while (!queue.empty() && dist[end] == kInfinity) {
      Vertex vertex = queue.front();
      queue.pop();
      if (used[vertex]) {
        continue;
      }
      used[vertex] = true;
      for (size_t i = 0; i < graph_[vertex].size(); ++i) {
        size_t id = graph_[vertex][i];
        if (!edges_[id].valid) {
          continue;
        }
        if (dist[edges_[id].to] == kInfinity &&
            edges_[id].flow < edges_[id].capacity) {
          queue.push(edges_[id].to);
          dist[edges_[id].to] = dist[edges_[id].from] + 1;
        }
      }
    }
    return dist[end] != kInfinity;
  }

  int DFS(std::vector<EdgeColors>& colors, Vertex vertex, Vertex end,
          int64_t current_flow) {
    if (current_flow <= 0) {
      return 0;
    }
    if (vertex == end) {
      return current_flow;
    }
    colors[vertex] = EdgeColors::kGrey;
    for (size_t to_id : graph_[vertex]) {
      if (!edges_[to_id].valid) {
        continue;
      }
      size_t to = edges_[to_id].to;
      if (colors[to] == EdgeColors::kWhite && dist[to] == dist[vertex] + 1) {
        int flow = DFS(colors, to, end,
                       std::min(current_flow,
                                edges_[to_id].capacity - edges_[to_id].flow));
        if (flow > 0) {
          edges_[to_id].flow += flow;
          edges_[to_id ^ 1].flow -= flow;
          colors[vertex] = EdgeColors::kBlack;
          return flow;
        }
      }
    }
    colors[vertex] = EdgeColors::kBlack;
    return 0;
  }

  std::vector<Edge> edges_;
  std::vector<std::vector<size_t>> graph_;
  std::vector<int64_t> dist;
};

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);
  size_t num_vertices, num_edges;
  std::cin >> num_vertices >> num_edges;
  Graph gr(num_vertices);
  for (size_t i = 0; i < num_edges; ++i) {
    Vertex from, to;
    int64_t capacity;
    std::cin >> from >> to >> capacity;
    gr.AddEdge(from, to, capacity);
  }
  auto [max_flow, edges_flows] = gr.FindMaxFlow(1, num_vertices);
  std::cout << max_flow << '\n';
  for (auto edge : edges_flows) {
    if (edge.capacity == 0) {
      continue;
    }
    std::cout << edge.flow << '\n';
  }
}
