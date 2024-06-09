#include <algorithm>
#include <array>
#include <iostream>
#include <limits>
#include <locale>
#include <string>
#include <tuple>
#include <vector>

class SuffixAutomaton {
 public:
  SuffixAutomaton() : nodes_(1) {
    nodes_[0].go.fill(kEmpty);
  }

  void Extend(size_t sym) {
    Node& new_node = nodes_.emplace_back();
    new_node.len = nodes_[final_].len + 1;
    size_t new_id = nodes_.size() - 1;
    size_t cur = final_;
    while ((cur != kEmpty) && (nodes_[cur].go[sym] == kEmpty)) {
      nodes_[cur].go[sym] = new_id;
      cur = nodes_[cur].link;
    }
    if (cur != kEmpty) {
      size_t next = nodes_[cur].go[sym];
      if (nodes_[cur].len + 1 == nodes_[next].len) {
        new_node.link = next;
        nodes_[next].is_leaf = false;
      } else {
        Node& clone = nodes_.emplace_back(nodes_[next]);
        size_t clone_id = nodes_.size() - 1;
        clone.len = nodes_[cur].len + 1;
        nodes_[next].link = nodes_[new_id].link = clone_id;
        clone.is_leaf = false;
        while ((cur != kEmpty) && (nodes_[cur].go[sym] == next)) {
          nodes_[cur].go[sym] = clone_id;
          cur = nodes_[cur].link;
          if ((cur != kEmpty) && (nodes_[cur].go[sym] == kEmpty)) {
            return;
          }
        }
      }
    } else {
      new_node.link = 0;
      nodes_[0].is_leaf = false;
    }
    final_ = new_id;
  }

  void Extend(const std::vector<size_t>& str) {
    nodes_.reserve(2 * str.size());
    for (auto ch : str) {
      Extend(ch);
    }
  }

  void Reset() { state_ = 0; }

  bool Move(size_t sym) {
    if (nodes_[state_].go[sym] == kEmpty) {
      return false;
    }
    state_ = nodes_[state_].go[sym];
    return true;
  }

  std::tuple<size_t, size_t, size_t> AnalyzeString(const std::vector<size_t>& str) {
    std::vector<size_t> indices(nodes_.size(), 0);
    std::tuple<size_t, size_t, size_t> cur_index = std::make_tuple(str.size() - 1, 0, str.size() - 1);
    std::vector<std::vector<size_t>> links(nodes_.size());
    for (size_t i = 0; i < nodes_.size(); ++i) {
      if (nodes_[i].link == kEmpty) {
        continue;
      }
      links[nodes_[i].link].push_back(i);
    }
    std::vector<size_t> children(nodes_.size(), 0);
    std::vector<size_t> starts(nodes_.size(), kEmpty);
    DFS(str.size(), 0, links, children, starts);
    for (size_t i = 0; i < nodes_.size(); ++i) {
      indices[i] = children[i] * nodes_[i].len;
      if ((indices[i] > std::get<0>(cur_index)) && (nodes_[i].len < str.size())) {
        cur_index = std::make_tuple(indices[i], starts[i], nodes_[i].len);
      }
    }
    return cur_index;
  }

  void Print() {
    for (size_t i = 0; i < nodes_.size(); ++i) {
      std::cout << i << "(" << nodes_[i].link << (nodes_[i].is_leaf ? "!" : "") << "): ";
      for (size_t j = 0; j < nodes_[i].go.size(); ++j) {
        if (nodes_[i].go[j] == kEmpty) continue;
        std::cout << j << "->" << nodes_[i].go[j] << " ";
      }
      std::cout << std::endl;
    }
  }

 private:
  static constexpr size_t kEmpty = std::numeric_limits<size_t>::max();

  struct Node {
    size_t len = 0;
    size_t link = kEmpty;
    bool is_leaf = true;
    std::array<size_t, 10> go{kEmpty, kEmpty, kEmpty, kEmpty, kEmpty, kEmpty, kEmpty, kEmpty, kEmpty, kEmpty};
  };

  void DFS(size_t length, size_t vertex, const std::vector<std::vector<size_t>>& links, std::vector<size_t>& children, std::vector<size_t>& starts) {
    if (links[vertex].size() == 0) {
      children[vertex] = 1;
      starts[vertex] = length - nodes_[vertex].len;
      return;
    }
    for (auto child : links[vertex]) {
      DFS(length, child, links, children, starts);
      children[vertex] += children[child];
      starts[vertex] = starts[child];
    }
  }

  std::vector<Node> nodes_;
  size_t final_ = 0;
  size_t state_ = 0;
};

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);
  size_t num_items;
  size_t alph_size;
  std::cin >> num_items >> alph_size;
  SuffixAutomaton automat;
  std::vector<size_t> str(num_items + 1);
  for (size_t i = 0; i < num_items; ++i) {
    std::cin >> str[i];
  }
  str[num_items] = 0;
  std::reverse(str.begin(), str.end());
  automat.Extend(str);
  std::reverse(str.begin(), str.end());
  auto [index, start, size] = automat.AnalyzeString(str);
  std::cout << index << std::endl;
  std::cout << size << std::endl;
  for (size_t i = 0; i < size; ++i) {
    std::cout << str[start + i] << " ";
  }
  std::cout << std::endl;
}
