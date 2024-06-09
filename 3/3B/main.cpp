/*
#include <initializer_list>
#include <iostream>
#include <numeric>
#include <stack>
#include <unordered_map>
#include <vector>

template <typename T>
class DisjointSetUnion {
 public:
  DisjointSetUnion() = default;

  template <typename Container>
  DisjointSetUnion(const Container& items) {
    for (T item : items) {
      MakeSet(item);
    }
  }

  DisjointSetUnion(std::initializer_list<T> items) {
    for (T item : items) {
      MakeSet(item);
    }
  }

  void MakeSet(const T& value) {
    parents_[value] = value;
    rank_[value] = 0;
  }

  T FindSet(const T& value) {
    if (!HasParent(value)) {
      return value;
    }
    return parents_[value] = FindSet(parents_[value]);
  }

  void Union(const T& first, const T& second) {
    T top_first = FindSet(first);
    T top_second = FindSet(second);
    if (top_first != top_second) {
      if (rank_[top_first] < rank_[top_second]) {
        std::swap(top_first, top_second);
      }
      parents_[top_second] = top_first;
      if (rank_[top_second] == rank_[top_first]) {
        ++rank_[top_first];
      }
    }
  }

  bool IsSameSet(const T& first, const T& second) {
    return FindSet(first) == FindSet(second);
  }

 private:
  bool HasParent(T value) { return parents_[value] != value; }

  std::unordered_map<T, T> parents_;
  std::unordered_map<T, size_t> rank_;
};

using Vertex = size_t;

namespace operations {
const std::string kCut = "cut";
const std::string kAsk = "ask";
}  // namespace operations

struct Operation {
  std::string type;
  Vertex first;
  Vertex second;
};

int main() {
  size_t num_vertices;
  size_t num_connections;
  size_t num_operations;
  std::cin >> num_vertices >> num_connections >> num_operations;
  for (size_t i = 0; i < num_connections; ++i) {
    Vertex first;
    Vertex second;
    std::cin >> first >> second;
  }
  std::stack<Operation> ops;
  for (size_t i = 0; i < num_operations; ++i) {
    Operation op;
    std::cin >> op.type >> op.first >> op.second;
    ops.push(op);
  }
  std::vector<Vertex> vertices(num_vertices);
  std::iota(vertices.begin(), vertices.end(), 1);
  DisjointSetUnion<Vertex> network(vertices);
  std::stack<std::string> result;
  while (!ops.empty()) {
    auto next = ops.top();
    if (next.type == operations::kAsk) {
      result.push(network.IsSameSet(next.first, next.second) ? "YES\n" : "NO\n");
    } else if (next.type == operations::kCut) {
      network.Union(next.first, next.second);
    }
    ops.pop();
  }
  while (!result.empty()) {
    std::cout << result.top();
    result.pop();
  }
  return 0;
}
*/
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Функция поиска множества, содержащего элемент x
int find(int parent[], int x) {
    if (parent[x] != x) {
        parent[x] = find(parent, parent[x]);
    }
    return parent[x];
}

// Функция объединения множеств, содержащих элементы x и y
void unionSet(int parent[], int rank[], int x, int y) {
    int xroot = find(parent, x);
    int yroot = find(parent, y);
    if (xroot != yroot) {
        if (rank[xroot] < rank[yroot]) {
            swap(xroot, yroot);
        }
        parent[yroot] = xroot;
        if (rank[xroot] == rank[yroot]) {
            rank[xroot]++;
        }
    }
}

struct Query {
  string type;
  int u, v;
};

int main() {
    int n, m, k;
    cin >> n >> m >> k;

    // Инициализация массива родителей множеств и массива рангов
    int parent[n+1], rank[n+1];
    for (int i = 1; i <= n; i++) {
        parent[i] = i;
        rank[i] = 0;
    }

    // Обработка операций
    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
    }
    Query queries[k];
    for (int i = 0; i < k; ++i) {
      cin >> queries[i].type >> queries[i].u >> queries[i].v;
    }
    vector<string> res;
    for (int i = k - 1; i >= 0; --i) {
        string op = queries[i].type;
        int u = queries[i].u, v = queries[i].v;
        if (op == "ask") {
          res.push_back((find(parent, u) == find(parent, v)) ? "YES\n" : "NO\n");
        } else {
            unionSet(parent, rank, u, v);
        }
    }
  reverse(res.begin(), res.end());
  for (string ans : res) {
    cout << ans;
  }
    return 0;
}
