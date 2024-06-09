#include <iostream>
#include <limits>
#include <utility>
#include <vector>

using Vertex = size_t;

struct BipartiteGraph {
 public:
  BipartiteGraph(const std::vector<std::vector<Vertex>>& connect,
                 size_t num_first, size_t num_second)
      : pairs_(connect), num_first_(num_first), num_second_(num_second) {}

  std::pair<size_t, size_t> NumVertices() const {
    return {num_first_, num_second_};
  }

  std::vector<Vertex> PairedVertices(Vertex vertex) const {
    return pairs_[vertex];
  }

 private:
  std::vector<std::vector<Vertex>> pairs_;
  size_t num_first_;
  size_t num_second_;
};

class FindPairs {
 public:
  static constexpr size_t kInfinity = std::numeric_limits<size_t>::max();

  FindPairs(const BipartiteGraph& graph)
      : graph_(graph),
        min_t_(graph.NumVertices().second + 1, kInfinity),
        used_(graph.NumVertices().first + 1, false) {}

  std::pair<size_t, std::vector<size_t>> operator()() {
    size_t pair_size = 0;
    for (Vertex i = 1; i <= graph_.NumVertices().first; ++i) {
      used_.assign(graph_.NumVertices().first, false);
      bool res = TryConnect(i);
      if (res) {
        ++pair_size;
      }
    }
    return {pair_size, min_t_};
  }

 private:
  bool TryConnect(Vertex vertex) {
    if (used_[vertex]) {
      return false;
    }
    used_[vertex] = true;
    bool res = false;
    for (Vertex next : graph_.PairedVertices(vertex)) {
      if (min_t_[next] == kInfinity) {
        min_t_[next] = vertex;
        res = true;
        break;
      }
      bool att_res = TryConnect(min_t_[next]);
      if (att_res) {
        min_t_[next] = vertex;
        res = true;
        break;
      }
    }
    return res;
  }

  const BipartiteGraph& graph_;
  std::vector<size_t> min_t_;
  std::vector<bool> used_;
};

BipartiteGraph ReadGraph() {
  size_t num_eng;
  size_t num_ch;
  std::cin >> num_eng >> num_ch;
  std::vector<std::vector<Vertex>> connect(num_eng + 1);
  for (size_t i = 1; i <= num_eng; ++i) {
    while (true) {
      Vertex next;
      std::cin >> next;
      if (next == 0) {
        break;
      }
      connect[i].push_back(next);
    }
  }
  BipartiteGraph graph(connect, num_eng, num_ch);
  return graph;
}

int main() {
  auto graph = ReadGraph();
  auto [size, part] = FindPairs{graph}();
  std::cout << size << std::endl;
  for (size_t i = 1; i <= graph.NumVertices().second; ++i) {
    if (part[i] == FindPairs::kInfinity) {
      continue;
    }
    std::cout << part[i] << " " << i << std::endl;
  }
  return 0;
}
