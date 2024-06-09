#include <algorithm>
#include <deque>
#include <functional>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

class Tree {
 public:
  using Vertex = size_t;
  using Edge = std::pair<Vertex, Vertex>;

  Tree(size_t num_vertices, const std::vector<Vertex>& parents)
      : children_(num_vertices), parent_(num_vertices, kNoVertex) {
    std::copy(parents.begin(), parents.end(), parent_.begin() + 1);
    for (size_t i = 1; i <= parents.size(); ++i) {
      children_[parents[i - 1]].push_back(i);
    }
  }

  size_t VerticesCount() const { return children_.size(); }

  Vertex GetParent(Vertex vertex) const { return parent_[vertex]; }

  std::vector<Vertex> GetChildren(Vertex vertex) const {
    return children_[vertex];
  }

  size_t NumChildren(Vertex vertex) const { return children_[vertex].size(); }

 private:
  static constexpr Vertex kNoVertex = std::numeric_limits<Vertex>::max();

  std::vector<std::vector<Vertex>> children_;
  std::vector<Vertex> parent_;
};

struct TourPoint {
  Tree::Vertex vertex;
  size_t depth;
};

using EulerTour = std::vector<TourPoint>;

EulerTour MakeEulerTour(const Tree& tree) {
  std::vector<size_t> cur_children(tree.VerticesCount(), 0);
  Tree::Vertex cur_vertex = 0;
  EulerTour res;
  size_t cur_depth = 0;
  while(true) {
    res.push_back({cur_vertex, cur_depth});
    if (cur_children[cur_vertex] == tree.NumChildren(cur_vertex)) {
      if (cur_vertex == 0) {
        break;
      }
      --cur_depth;
      cur_vertex = tree.GetParent(cur_vertex);
    } else {
      ++cur_depth;
      cur_vertex = tree.GetChildren(cur_vertex)[cur_children[cur_vertex]++];
    }
  }
  return res;
}

std::vector<size_t> CountTimeIn(const EulerTour& tour) {
  std::vector<size_t> res;
  constexpr size_t kNoTime = std::numeric_limits<size_t>::max();
  for (size_t t = 0; t < tour.size(); ++t) {
    if (tour[t].vertex >= res.size()) {
      res.resize(tour[t].vertex + 1, kNoTime);
    }
    res[tour[t].vertex] = std::min(res[tour[t].vertex], t);
  }
  return res;
}

template <typename T>
class SparseTable {
 public:
  SparseTable(const std::vector<T>& arr, std::function<T(T, T)> aggr_fn)
      : rows_(SparseTable::GetRows(arr.size())),
        table_(rows_, std::vector<T>(arr.size())),
        aggr_fn_(aggr_fn) {
    for (size_t i = 0; i < arr.size(); ++i) {
      table_[0][i] = arr[i];
    }
    for (size_t j = 1; j < rows_; ++j) {
      for (size_t i = 0; i < arr.size(); ++i) {
        if (i + (1 << (j - 1)) >= arr.size()) {
          table_[j][i] = table_[j - 1][i];
        } else {
          table_[j][i] =
              aggr_fn_(table_[j - 1][i], table_[j - 1][i + (1 << (j - 1))]);
        }
      }
    }
  }

  T Query(size_t left, size_t right) {
    size_t segment = GetSegment(right - left);
    return aggr_fn_(table_[segment][left],
                    table_[segment][right - (1 << segment)]);
  }

 private:
  static size_t GetRows(size_t size) {
    size_t rows = 0;
    while (size > 0) {
      ++rows;
      size >>= 1;
    }
    return rows;
  }

  static size_t GetSegment(size_t length) {
    size_t segment = 0;
    while (length > 1) {
      ++segment;
      length >>= 1;
    }
    return segment;
  }

  size_t rows_;
  std::vector<std::vector<T>> table_;
  std::function<T(T, T)> aggr_fn_;
};

int main() {
  size_t num_vertices;
  size_t num_queries;
  std::cin >> num_vertices >> num_queries;
  std::vector<Tree::Vertex> parents(num_vertices - 1);
  for (size_t i = 0; i < num_vertices - 1; ++i) {
    std::cin >> parents[i];
  }
  Tree tree{num_vertices, parents};
  auto tour = MakeEulerTour(tree);
  auto t_in = CountTimeIn(tour);
  SparseTable<TourPoint> table{tour, [](TourPoint first, TourPoint second) {
                                 if (first.depth < second.depth) {
                                   return first;
                                 }
                                 return second;
                               }};
  std::deque<size_t> a(2);
  std::cin >> a[0] >> a[1];
  size_t x, y, z;
  std::cin >> x >> y >> z;
  Tree::Vertex res = 0;
  Tree::Vertex sum = 0;
  for (size_t q = 0; q < num_queries; ++q) {
    res = table
              .Query(std::min(t_in[(a[0] + res) % num_vertices], t_in[a[1]]),
                     std::max(t_in[(a[0] + res) % num_vertices], t_in[a[1]]) + 1)
              .vertex;
    sum += res;
    a.push_back((a[0] * x + a[1] * y + z) % num_vertices);
    a.pop_front();
    a.push_back((a[0] * x + a[1] * y + z) % num_vertices);
    a.pop_front();
  }
  std::cout << sum << std::endl;
}
