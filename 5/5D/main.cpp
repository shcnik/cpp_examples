#include <cassert>
#include <iostream>
#include <limits>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using std::string;

class Trie {
 public:
  Trie(const std::vector<string>& strings) : root_(std::make_unique<Node>()) {
    for (size_t i = 0; i < strings.size(); ++i) {
      Node* cur = root_.get();
      for (size_t j = 0; j < strings[i].size(); ++j) {
        cur->ids.push_back(i);
        if (cur->next.count(strings[i][j]) == 0) {
          cur->next[strings[i][j]] = std::make_unique<Node>();
        }
        cur = cur->next[strings[i][j]].get();
      }
      cur->ids.push_back(i);
      cur->terminals.push_back(i);
    }
    Reset();
  }

  void Reset() { cur_pos_ = root_.get(); }

  bool Move(char ch) {
    if (cur_pos_->next.count(ch) == 0) {
      return false;
    }
    cur_pos_ = cur_pos_->next.at(ch).get();
    return true;
  }

  std::vector<size_t> CurrentStrings() { return cur_pos_->ids; }

  std::vector<size_t> TerminalStrings() { return cur_pos_->terminals; }

 private:
  struct Node {
    std::unordered_map<char, std::unique_ptr<Node>> next;
    std::vector<size_t> ids;
    std::vector<size_t> terminals;
  };

  std::unique_ptr<Node> root_;
  const Node* cur_pos_ = nullptr;
};

std::pair<std::vector<size_t>, std::vector<size_t>> GetPalindromes(
    const string& str) {
  std::pair<std::vector<size_t>, std::vector<size_t>> ans(
      std::vector<size_t>(str.size(), 0), std::vector<size_t>(str.size(), 0));
  size_t left = 0;
  size_t right = std::numeric_limits<size_t>::max();
  for (size_t i = 0; i < str.size(); ++i) {
    ans.first[i] =
        ((i > right) || (right == std::numeric_limits<size_t>::max()))
            ? 1
            : std::min(ans.first[left + right - i], right - i + 1);
    while ((i + ans.first[i] < str.size()) && (i >= ans.first[i]) &&
           (str[i + ans.first[i]] == str[i - ans.first[i]])) {
      ++ans.first[i];
    }
    if (i + ans.first[i] > right + 1) {
      left = i - ans.first[i] + 1;
      right = i + ans.first[i] - 1;
    }
  }
  left = 0;
  right = std::numeric_limits<size_t>::max();
  for (size_t i = 0; i < str.size(); ++i) {
    ans.second[i] =
        ((i > right) || (right == std::numeric_limits<size_t>::max()))
            ? 0
            : std::min(ans.second[left + right - i + 1], right - i + 1);
    while ((i + ans.second[i] < str.size()) && (i >= ans.second[i] + 1) &&
           (str[i + ans.second[i]] == str[i - ans.second[i] - 1])) {
      ++ans.second[i];
    }
    if (i + ans.second[i] > right + 1) {
      left = i - ans.second[i];
      right = i + ans.second[i] - 1;
    }
  }
  return ans;
}

bool IsPalindrome(
    const std::pair<std::vector<size_t>, std::vector<size_t>>& palindromes,
    size_t left, size_t right) {
  if (right == left) {
    return true;
  }
  if ((right - left) % 2 == 0) {
    return palindromes.second[left + (right - left - 1) / 2 + 1] >=
           (right - left) / 2;
  }
  return palindromes.first[left + (right - left - 1) / 2] >=
         (right - left - 1) / 2 + 1;
}

struct StringInfo {
  size_t index;
  size_t prefix_len;
};

int main() {
  size_t num_strings;
  std::cin >> num_strings;
  std::vector<string> strings(num_strings, "");
  std::vector<std::pair<std::vector<size_t>, std::vector<size_t>>>
      palindromes(num_strings);
  for (size_t i = 0; i < num_strings; ++i) {
    std::cin >> strings[i];
    palindromes[i] = GetPalindromes(strings[i]);
  }
  std::unordered_multimap<size_t, StringInfo> table;
  std::set<std::pair<size_t, size_t>> res;
  constexpr size_t kPower = 31;
  for (size_t i = 0; i < num_strings; ++i) {
    size_t hash = 0;
    size_t cur_power = 1;
    for (size_t j = 0; j < strings[i].size(); ++j) {
      hash += static_cast<size_t>(strings[i][j] - 'a' + 1) * cur_power;
      if (IsPalindrome(palindromes[i], j + 1, strings[i].size())) {
        table.insert({hash, {i + 1, j + 1}});
      }
      cur_power *= kPower;
    }
  }
  for (size_t i = 0; i < num_strings; ++i) {
    size_t hash = 0;
    size_t cur_power = 1;
    for (size_t j = strings[i].size() - 1; j < strings[i].size(); --j) {
      hash += static_cast<size_t>(strings[i][j] - 'a' + 1) * cur_power;
      cur_power *= kPower;
    }
    if (!table.contains(hash)) {
      continue;
    }
    auto [begin, end] = table.equal_range(hash);
    for (auto it = begin; it != end; ++it) {
      if (it->second.prefix_len != strings[i].size()) {
        continue;
      }
      if (it->second.index == i + 1) {
        continue;
      }
      res.emplace(it->second.index, i + 1);
    }
  }
  table.clear();
  for (size_t i = 0; i < num_strings; ++i) {
    size_t hash = 0;
    size_t cur_power = 1;
    for (size_t j = strings[i].size() - 1; j < strings[i].size(); --j) {
      hash += static_cast<size_t>(strings[i][j] - 'a' + 1) * cur_power;
      if (IsPalindrome(palindromes[i], 0, j)) {
        table.insert({hash, {i + 1, strings[i].size() - j}});
      }
      cur_power *= kPower;
    }
  }
  for (size_t i = 0; i < num_strings; ++i) {
    size_t hash = 0;
    size_t cur_power = 1;
    for (size_t j = 0; j < strings[i].size(); ++j) {
      hash += static_cast<size_t>(strings[i][j] - 'a' + 1) * cur_power;
      cur_power *= kPower;
    }
    if (!table.contains(hash)) {
      continue;
    }
    auto [begin, end] = table.equal_range(hash);
    for (auto it = begin; it != end; ++it) {
      if (it->second.prefix_len != strings[i].size()) {
        continue;
      }
      if (it->second.index == i + 1) {
        continue;
      }
      res.emplace(i + 1, it->second.index);
    }
  }
  std::cout << res.size() << std::endl;
  for (auto item : res) {
    std::cout << item.first << " " << item.second << std::endl;
  }
}
