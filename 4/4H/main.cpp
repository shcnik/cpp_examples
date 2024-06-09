#include <iostream>
#include <limits>
#include <unordered_map>
#include <vector>

static constexpr int64_t kInfinity = std::numeric_limits<int64_t>::max();

struct Edge {
  size_t to_vertex;
  int64_t flow;
  int64_t capacity;

  int64_t CurrentCapacity() { return capacity - flow; }

  Edge(size_t to_vertex, int64_t capacity)
      : to_vertex(to_vertex), flow(0), capacity(capacity) {}
};

struct Graph {
 public:
  void AddEdge(size_t from, size_t to, int64_t capacity) {
    if ((invalid[from] or invalid[to]) == 0) {
      edges_.emplace_back(to, capacity);
      graph_[from].emplace_back(edges_.size() - 1);
      edges_.emplace_back(from, 0);
      graph_[to].emplace_back(edges_.size() - 1);
    }
  }

  int64_t capacity(size_t from) {
    size_t row = from / width_;
    size_t col = from % (width_);
    return valence[field_[row][col]];
  }

  size_t P(size_t row, size_t col) { return row * width_ + col; }

  int64_t flow(size_t vertex) {
    int64_t result = 0;
    for (auto id_edge : graph_[vertex]) {
      result += std::abs(edges_[id_edge].flow);
    }
    return result / 2;
  }

  void ScanPoint(size_t col, size_t row) {
    if (col >= 1) {
      AddEdge(P(col, row), P(col - 1, row), 1);
    }
    if (col <= height_ - 2) {
      AddEdge(P(col, row), P(col + 1, row), 1);
    }
    if (row >= 1) {
      AddEdge(P(col, row), P(col, row - 1), 1);
    }
    if (row <= width_ - 2) {
      AddEdge(P(col, row), P(col, row + 1), 1);
    }
  }

  void ScanField() {
    for (size_t i = 0; i < height_; ++i) {
      for (size_t j = 0; j < width_; ++j) {
        std::cin >> field_[i][j];
        if (field_[i][j] == '.') {
          invalid[P(i, j)] = 1;
          ++invalid_count;
          continue;
        }
      }
    }
    for (int64_t i = 0; i < height_; ++i) {
      for (int64_t j = 0; j < width_; ++j) {
        if ((i + j) % 2 == 0 and !invalid[P(i, j)]) {
          ScanPoint(i, j);
        }
      }
    }
  }

  Graph(int64_t height, int64_t width)
      : invalid_count(0),
        invalid(height * width + 2, 0),
        field_(height + 1, std::vector<char>(width + 1, '.')),
        graph_(height * width + 2, std::vector<int64_t>()),
        height_(height),
        width_(width) {}

  int64_t FindIncreasingPath(
      std::vector<size_t>& used, size_t phase, size_t cur, size_t end,
      int64_t min) {
    if (end == cur) {
      return min;
    }
    used[cur] = phase;
    for (size_t edge_id : graph_[cur]) {
      if (edges_[edge_id].CurrentCapacity() == 0) {
        continue;
      }
      if (used[edges_[edge_id].to_vertex] == phase) {
        continue;
      }
      int64_t delta =
          FindIncreasingPath(used, phase, edges_[edge_id].to_vertex, end,
                             std::min(min, edges_[edge_id].CurrentCapacity()));
      if (delta > 0) {
        edges_[edge_id].flow += delta;
        edges_[edge_id ^ 1].flow -= delta;
        return delta;
      }
    }
    return 0;
  }

  bool MaxFlow() {
    size_t s = width_ * height_;
    size_t t = width_ * height_ + 1;
    for (int64_t i = 0; i < height_; ++i) {
      for (int64_t j = 0; j < width_; ++j) {
        if ((i + j) % 2 == 0) {
          AddEdge(s, P(i, j), capacity(P(i, j)));
        } else {
          AddEdge(P(i, j), t, capacity(P(i, j)));
        }
      }
    }

    std::vector<size_t> used(graph_.size(), 0);
    size_t phase = 1;
    while (FindIncreasingPath(used, phase, s, t, kInfinity)) {
      ++phase;
    }

    bool flag = true;
    for (int64_t i = 0; i < height_; ++i) {
      for (int64_t j = 0; j < width_; ++j) {
        if (valence[field_[i][j]] > flow(P(i, j))) {
          flag = false;
        }
      }
    }
    return flag;
  }

  size_t invalid_count;
  std::vector<int64_t> invalid;
  std::unordered_map<char, int64_t> valence = {
      {'H', 1}, {'O', 2}, {'N', 3}, {'C', 4}};

 private:
  std::vector<std::vector<char>> field_;
  std::vector<std::vector<int64_t>> graph_;
  std::vector<Edge> edges_;
  size_t height_;
  size_t width_;
};

int main() {
  size_t height;
  size_t width;
  std::cin >> height >> width;
  Graph graph(height, width);
  graph.ScanField();
  auto result = graph.MaxFlow();
  if (result && (graph.invalid_count != height * width)) {
    std::cout << "Valid\n";
  } else {
    std::cout << "Invalid\n";
  }
}
