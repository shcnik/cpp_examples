#include <algorithm>
#include <iostream>
#include <limits>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <utility>
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

  void Unite(const T& first, const T& second) {
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

struct Curriculum {
  std::vector<size_t> costs;
  size_t modulo;
};

using Course = size_t;
using MiniCourse = std::pair<Course, Course>;

size_t GetMinimumCost(const Curriculum& curriculum) {
  constexpr size_t kInfinity = std::numeric_limits<size_t>::max();
  std::vector<std::pair<Course, size_t>> costs(curriculum.costs.size());
  for (Course c = 0; c < curriculum.costs.size(); ++c) {
    costs[c] = {c, curriculum.costs[c]};
  }
  std::sort(
      costs.begin(), costs.end(),
      [](std::pair<Course, size_t> first, std::pair<Course, size_t> second) {
        return first.second < second.second;
      });
  std::vector<MiniCourse> courses;
  std::vector<std::pair<decltype(costs.begin()), decltype(costs.begin())>> pos(
      curriculum.costs.size());
  DisjointSetUnion<Course> connect;
  for (Course c = 0; c < curriculum.costs.size(); ++c) {
    pos[c] = {
        costs.begin(),
        std::lower_bound(costs.begin(), costs.end(), curriculum.modulo,
                         [&](std::pair<Course, size_t> cost, size_t value) {
                           return (costs[c].second + cost.second) < value;
                         })};
    connect.MakeSet(c);
  }
  std::vector<MiniCourse> minicourses;
  while (minicourses.size() < curriculum.costs.size() - 1) {
    for (Course c = 0; c < curriculum.costs.size(); ++c) {
      while ((pos[c].first != costs.end()) &&
             ((connect.IsSameSet(c, pos[c].first->first)) ||
              (pos[c].first->first <= c))) {
        ++pos[c].first;
      }
      while ((pos[c].first != costs.end()) &&
             ((connect.IsSameSet(c, pos[c].second->first)) ||
              (pos[c].second->first <= c))) {
        ++pos[c].second;
      }
      if (pos[c].first == costs.end()) {
        if (pos[c].second == costs.end()) {
          continue;
        }
        minicourses.push_back({pos[c].second->first, c});
        connect.Unite(c, pos[c].second->first);
        ++pos[c].second;
        continue;
      }
      if (pos[c].second == costs.end()) {
        minicourses.push_back({pos[c].first->first, c});
        connect.Unite(c, pos[c].first->first);
        ++pos[c].first;
        continue;
      }
      if ((pos[c].first->second + curriculum.modulo < pos[c].second->second) &&
          (pos[c].first->second + costs[c].second < curriculum.modulo)) {
        minicourses.push_back({pos[c].first->first, c});
        connect.Unite(c, pos[c].first->first);
        ++pos[c].first;
      } else {
        minicourses.push_back({pos[c].second->first, c});
        connect.Unite(c, pos[c].second->first);
        ++pos[c].second;
      }
    }
  }
  std::sort(minicourses.begin(), minicourses.end(),
            [&](MiniCourse first, MiniCourse second) {
              return (curriculum.costs[first.first] +
                      curriculum.costs[first.second]) %
                         curriculum.modulo <
                     (curriculum.costs[second.first] +
                      curriculum.costs[second.second]) %
                         curriculum.modulo;
            });
  minicourses.resize(curriculum.costs.size() - 1);
  size_t res = 0;
  for (auto minicourse : minicourses) {
    res += (curriculum.costs[minicourse.first] +
            curriculum.costs[minicourse.second]) %
           curriculum.modulo;
  }
  return res;
}

int main() {
  size_t num_courses;
  size_t modulo;
  std::cin >> num_courses >> modulo;
  Curriculum curr{std::vector<size_t>(num_courses), modulo};
  for (size_t i = 0; i < num_courses; ++i) {
    std::cin >> curr.costs[i];
  }
  std::cout << GetMinimumCost(curr) << std::endl;
}
