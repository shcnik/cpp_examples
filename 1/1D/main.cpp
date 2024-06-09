#include <iostream>
#include <queue>
#include <vector>

std::vector<size_t> BFS(const std::vector<std::vector<size_t>>& graph,
                        size_t start, std::vector<bool>& used) {
  std::queue<size_t> queue;
  std::vector<size_t> res;
  queue.push(start);
  used[start] = true;
  while (!queue.empty()) {
    size_t cur = queue.front();
    queue.pop();
    res.push_back(cur);
    for (size_t next : graph[cur]) {
      if (used[next]) {
        continue;
      }
      used[next] = true;
      queue.push(next);
    }
  }
  return res;
}

int main() {
  size_t num_vertices, num_edges;
  std::cin >> num_vertices >> num_edges;
  std::vector<std::vector<size_t>> graph(num_vertices + 1);
  for (size_t i = 0; i < num_edges; ++i) {
    size_t first, second;
    std::cin >> first >> second;
    graph[first].push_back(second);
    graph[second].push_back(first);
  }
  std::vector<bool> used(num_vertices + 1, false);
  std::vector<std::vector<size_t>> res;
  for (size_t vertex = 1; vertex <= num_vertices; ++vertex) {
    if (used[vertex]) {
      continue;
    }
    res.push_back(BFS(graph, vertex, used));
  }
  std::cout << res.size() << std::endl;
  for (auto list : res) {
    std::cout << list.size() << std::endl;
    for (auto item : list) {
      std::cout << item << " ";
    }
    std::cout << std::endl;
  }
}
