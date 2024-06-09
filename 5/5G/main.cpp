#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using std::string;

template <>
struct std::hash<std::pair<char, char>> {
  size_t operator()(std::pair<char, char> values) const {
    return static_cast<size_t>(values.first - '0') * 10ull + static_cast<size_t>(values.second - '0');
  }
};

class Trie {
 public:
  Trie() : nodes_{std::vector<Node>(1)} {}

  void AddString(const string& str) {
    size_t cur = 0;
    ++nodes_[0][0].num_leafs;
    for (size_t i = 0; i < str.length(); ++i) {
      auto index = std::make_pair(str[i], str[str.length() - 1 - i]);
      if (!nodes_[i][cur].next.contains(index)) {
        while (i + 1 >= nodes_.size()) {
          nodes_.emplace_back();
        }
        nodes_[i + 1].emplace_back();
        nodes_[i][cur].next.emplace(index, nodes_[i + 1].size() - 1);
      }
      cur = nodes_[i][cur].next[index];
      ++nodes_[i + 1][cur].num_leafs;
    }
  }

  Trie(const std::vector<string>& strings) : Trie() {
    size_t total_size = 0;
    for (auto str : strings) {
      total_size = std::max(total_size, str.length() + 1);
    }
    nodes_.reserve(total_size);
    for (auto str : strings) {
      AddString(str);
    }
  }

  std::vector<size_t> GetAnswers(size_t thres) {
    std::vector<size_t> ans(nodes_.size(), 0);
    for (size_t l = 0; l < nodes_.size(); ++l) {
      for (size_t i = 0; i < nodes_[l].size(); ++i) {
        if (nodes_[l][i].num_leafs >= thres) {
          ++ans[l];
        }
      }
    }
    return ans;
  }

 private:
  struct Node {
    std::unordered_map<std::pair<char, char>, size_t> next;
    size_t num_leafs = 0;
  };

  std::vector<std::vector<Node>> nodes_;
};

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);
  size_t num_strings;
  size_t fake_thres;
  std::cin >> num_strings >> fake_thres;
  std::vector<string> strings(num_strings);
  for (size_t i = 0; i < num_strings; ++i) {
    std::cin >> strings[i];
  }
  Trie trie{strings};
  auto data = trie.GetAnswers(fake_thres);
  size_t num_queries;
  std::cin >> num_queries;
  for (size_t q = 0; q < num_queries; ++q) {
    size_t length;
    std::cin >> length;
    if (length >= data.size()) {
      std::cout << 0 << "\n";
    } else {
      std::cout << data[length] << "\n";
    }
  }
}
