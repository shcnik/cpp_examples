#include <iostream>
#include <limits>
#include <numeric>
#include <optional>
#include <queue>
#include <unordered_map>
#include <vector>

struct Edge {
  size_t first;
  size_t second;
  int weight;
};

class Graph {
 public:
  Graph(size_t num_vertices, const std::vector<Edge>& edges)
      : adj_(num_vertices + 1) {
    for (auto edge : edges) {
      adj_[edge.first][edge.second] = edge.weight;
    }
  }

  std::optional<int> GetMinimumPath(size_t start, size_t finish,
                                    int order) const {
    std::vector<int> distances(adj_.size(), kInfinity);
    std::vector<bool> visited(adj_.size(), false);
    std::deque<std::queue<size_t>> queue(order + 1);
    size_t total_vertices = 1;
    queue[0].push(start);
    int time = 0;
    while (total_vertices > 0) {
      while (!queue[0].empty()) {
        size_t cur = queue[0].front();
        queue[0].pop();
        --total_vertices;
        if (visited[cur]) {
          continue;
        }
        distances[cur] = time;
        for (auto next : adj_[cur]) {
          if (visited[next.first]) {
            continue;
          }
          queue[next.second].push(next.first);
          ++total_vertices;
        }
        visited[cur] = true;
      }
      queue.pop_front();
      queue.emplace_back();
      ++time;
    }
    if (distances[finish] == kInfinity) {
      return std::nullopt;
    }
    return distances[finish];
  }

 private:
  std::vector<std::unordered_map<size_t, int>> adj_;
  static constexpr int kInfinity = std::numeric_limits<int>::max();
};

constexpr size_t kBufferSize = 4096;

inline char GetChar() {
  static char buffer[4096];
  static size_t len = 0, pos = 0;
  if (pos == len) {
    pos = 0;
    len = fread(buffer, 1, kBufferSize, stdin);
  }
  if (pos == len) {
    return '\0';
  }
  return buffer[pos++];
}

constexpr char kWhitespaceLimit = 32;

inline char ReadChar() {
  char ch = GetChar();
  while (ch <= kWhitespaceLimit) {
    ch = GetChar();
  }
  return ch;
}

template <typename T>
inline T ReadInt() {
  int sign = 1;
  char ch = ReadChar();
  T x = 0;
  if (ch == '-') {
    sign = -1;
    ch = GetChar();
  }
  while (('0' <= ch) && (ch <= '9')) {
    x = x * 10 + (ch - '0');
    ch = GetChar();
  }
  return (sign == 1) ? x : -x;
}

int main() {
  constexpr int kOrder = 30;
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);
  std::cout.tie(0);
  size_t num_vertices = ReadInt<size_t>(), num_edges = ReadInt<size_t>();
  size_t start = ReadInt<size_t>(), finish = ReadInt<size_t>();
  std::vector<Edge> edges(num_edges);
  for (size_t i = 0; i < num_edges; ++i) {
    edges[i].first = ReadInt<size_t>();
    edges[i].second = ReadInt<size_t>();
    edges[i].weight = ReadInt<int>();
  }
  Graph graph(num_vertices, edges);
  auto length = graph.GetMinimumPath(start, finish, kOrder);
  if (!length.has_value()) {
    std::cout << -1 << std::endl;
    return 0;
  }
  std::cout << *length << std::endl;
}  //!
