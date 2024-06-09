#include <iostream>
#include <limits>
#include <unordered_map>
#include <utility>
#include <vector>

static constexpr int64_t kInfinity = std::numeric_limits<int64_t>::max();

using Graph = std::vector<std::vector<int64_t>>;

struct Edge {
  size_t to_vertex;
  int64_t capacity;
  int64_t flow;

  int64_t CurrentCapacity() const { return capacity - flow; }

  Edge(int64_t to_vertex, int64_t capacity)
      : to_vertex(to_vertex), capacity(capacity), flow(0) {}
};

void AddEdge(Graph& graph, std::vector<Edge>& edges, int64_t from, int64_t to,
             int64_t capacity) {
  edges.emplace_back(to, capacity);
  graph[from].emplace_back(edges.size() - 1);
  edges.emplace_back(from, 0);
  graph[to].emplace_back(edges.size() - 1);
}

struct State {
  size_t phase;
  size_t current;
  size_t end;
  int64_t min;
};

int64_t FindIncreasingPath(Graph& graph, std::vector<Edge>& edges,
                           std::vector<size_t>& used, State state) {
  if (state.end == state.current) {
    return state.min;
  }
  used[state.current] = state.phase;
  for (int64_t edge_id : graph[state.current]) {
    if (edges[edge_id].CurrentCapacity() > 0 &&
        used[edges[edge_id].to_vertex] != state.phase) {
      int64_t delta = FindIncreasingPath(
          graph, edges, used,
          {state.phase, edges[edge_id].to_vertex, state.end,
           std::min(state.min, edges[edge_id].CurrentCapacity())});
      if (delta > 0) {
        edges[edge_id].flow += delta;
        edges[edge_id ^ 1].flow -= delta;
        return delta;
      }
    }
  }
  return 0;
}

int64_t MaxFlow(Graph& graph, std::vector<Edge>& edges, size_t start,
                size_t end) {
  std::vector<size_t> used(graph.size(), 0);
  size_t phase = 1;
  while (FindIncreasingPath(graph, edges, used,
                            {phase, start, end, kInfinity}) != 0) {
    ++phase;
  }

  int64_t result = 0;
  for (int64_t edge_id : graph[start]) {
    result += edges[edge_id].flow;
  }
  return result;
}

using Field = std::vector<std::vector<uint8_t>>;

void Input(size_t height, size_t width, Field& field, int64_t& sum_black,
           int64_t& sum_white) {
  const std::unordered_map<char, uint8_t> kValents = {
      {'H', 1}, {'O', 2}, {'N', 3}, {'C', 4}};
  for (size_t i = 0; i < height; i++) {
    for (size_t j = 0; j < width; j++) {
      char field_piece;
      std::cin >> field_piece;
      if (field_piece != '.') {
        field[i][j] = kValents.at(field_piece);
      }
      if ((i + j) % 2 != 0) {
        sum_white += field[i][j];
      } else {
        sum_black += field[i][j];
      }
    }
  }
}

struct Params {
  size_t height;
  size_t width;
  size_t x_coord;
  size_t y_coord;
};

void AddVertexToGraph(std::vector<Edge>& edges, Graph& graph, Field& field,
                      Params params) {
  size_t x_coord = params.x_coord;
  size_t y_coord = params.y_coord;
  size_t height = params.height;
  size_t width = params.width;
  if (field[x_coord][y_coord] != 0) {
    if ((x_coord + y_coord) % 2 != 0) {
      AddEdge(graph, edges, height * width, x_coord * width + y_coord,
              field[x_coord][y_coord]);
      if (x_coord < height - 1 && field[x_coord + 1][y_coord] != 0) {
        AddEdge(graph, edges, x_coord * width + y_coord,
                (x_coord + 1) * width + y_coord, 1);
      }
      if (y_coord < width - 1 && field[x_coord][y_coord + 1] != 0) {
        AddEdge(graph, edges, x_coord * width + y_coord,
                x_coord * width + y_coord + 1, 1);
      }
    } else {
      AddEdge(graph, edges, x_coord * width + y_coord, height * width + 1,
              field[x_coord][y_coord]);
      if (x_coord < height - 1 && field[x_coord + 1][y_coord] != 0) {
        AddEdge(graph, edges, (x_coord + 1) * width + y_coord,
                x_coord * width + y_coord, 1);
      }
      if (y_coord < width - 1 && field[x_coord][y_coord + 1] != 0) {
        AddEdge(graph, edges, x_coord * width + y_coord + 1,
                x_coord * width + y_coord, 1);
      }
    }
  }
}

std::pair<Graph, std::vector<Edge>> ReadGraph(size_t height, size_t width, Field& field) {
  std::vector<Edge> edges;
  Graph graph(height * width + 2, std::vector<int64_t>());
  for (size_t i = 0; i < height; i++) {
    for (size_t j = 0; j < width; j++) {
      AddVertexToGraph(edges, graph, field, {height, width, i, j});
    }
  }
  return {graph, edges};
}

int main() {
  size_t height;
  size_t width;
  std::cin >> height >> width;
  Field field(height, std::vector<uint8_t>(width, 0));
  int64_t sum_white = 0;
  int64_t sum_black = 0;
  Input(height, width, field, sum_black, sum_white);
  auto [graph, edges] = ReadGraph(height, width, field);
  int64_t max_flow = MaxFlow(graph, edges, height * width, height * width + 1);
  if (max_flow == sum_black && max_flow == sum_white && max_flow != 0) {
    std::cout << "Valid\n";
  } else {
    std::cout << "Invalid\n";
  }
}
