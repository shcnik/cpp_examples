#include <initializer_list>
#include <iostream>
#include <numeric>
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

using Student = size_t;

template <>
class DisjointSetUnion<Student> {
 public:
  DisjointSetUnion() = default;

  DisjointSetUnion(size_t num_students) : parents_(num_students + 1), rank_(num_students + 1, 0) {
    for (size_t student = 1; student <= num_students; ++student) {
      parents_[student] = student;
    }
  }

  Student FindSet(Student value) {
    if (!HasParent(value)) {
      return value;
    }
    return parents_[value] = FindSet(parents_[value]);
  }

  void Union(Student first, Student second) {
    Student top_first = FindSet(first);
    Student top_second = FindSet(second);
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

  bool IsSameSet(Student first, Student second) {
    return FindSet(first) == FindSet(second);
  }

 private:
  bool HasParent(Student value) { return parents_[value] != value; }

  std::vector<Student> parents_;
  std::vector<size_t> rank_;
};

namespace operations {
constexpr int kConnect = 1;
constexpr int kGetStrength = 2;
}  // namespace operations

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);
  size_t num_students;
  size_t num_queries;
  std::cin >> num_students >> num_queries;
  DisjointSetUnion<Student> friendship(num_students);
  std::vector<size_t> strengths(num_students + 1, 0);
  std::vector<size_t> output;
  for (size_t q = 0; q < num_queries; ++q) {
    int operation;
    std::cin >> operation;
    if (operation == operations::kConnect) {
      Student first;
      Student second;
      size_t strength;
      std::cin >> first >> second >> strength;
      first = friendship.FindSet(first);
      second = friendship.FindSet(second);
      if (first == second) {
        strength += strengths[first];
      } else {
        strength += strengths[first] + strengths[second];
      }
      friendship.Union(first, second);
      strengths[friendship.FindSet(first)] = strength;
    } else if (operation == operations::kGetStrength) {
      Student student;
      std::cin >> student;
      output.push_back(strengths[friendship.FindSet(student)]);
    }
  }
  for (size_t i = 0; i < output.size(); ++i) {
    std::cout << output[i] << "\n";
  }
  return 0;
}
