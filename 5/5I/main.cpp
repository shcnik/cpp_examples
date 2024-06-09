#include <array>
#include <cassert>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

using std::string;

class Trie {
 public:
  Trie() : nodes_(1) {
    nodes_[0].parent = 0;
  }

  Trie(const std::vector<string>& strings) : Trie() {
    size_t reserve = 0;
    for (auto str : strings) {
      reserve += str.length();
    }
    nodes_.reserve(reserve);
    for (auto str : strings) {
      AddString(str);
    }
  }

  void AddString(const string& str) {
    size_t cur = 0;
    for (size_t i = 0; i < str.length(); ++i) {
      size_t ch = static_cast<size_t>(str[i] - '0');
      if (nodes_[cur].next[ch] == kEmpty) {
        Node new_node;
        new_node.parent = cur;
        new_node.pch = ch;
        nodes_[cur].next[ch] = nodes_.size();
        nodes_.push_back(new_node);
      }
      cur = nodes_[cur].next[ch];
    }
    nodes_[cur].term = true;
  }

  void Reset() { pos_ = 0; }

  void Move(size_t ch) { pos_ = Move(pos_, ch); }

  bool IsTerminal() { return IsTerminal(pos_); }

  bool HasInfiniteCycle() {
    return HasInfiniteCycle(0);
  }

 private:
  bool HasInfiniteCycle(size_t node) {
    static std::vector<bool> used(nodes_.size(), false);
    static std::vector<bool> visit(nodes_.size(), false);
    if (IsTerminal(node)) {
      return false;
    }
    if (visit[node]) {
      return true;
    }
    if (used[node]) {
      return false;
    }
    used[node] = true;
    visit[node] = true;
    for (size_t ch = 0; ch <= 1; ++ch) {
      if (HasInfiniteCycle(Move(node, ch))) {
        return true;
      }
    }
    visit[node] = false;
    return false;
  }

  bool IsTerminal(size_t node) {
    if (node == 0) {
      return false;
    }
    return nodes_[node].term = nodes_[node].term || IsTerminal(GetUp(node));
  }

  static constexpr size_t kEmpty = std::numeric_limits<size_t>::max();

  struct Node {
    std::array<size_t, 2> next{kEmpty, kEmpty};
    bool term = false;
    size_t parent;
    size_t pch;
    size_t link = kEmpty;
    size_t up = kEmpty;
    std::array<size_t, 2> go{kEmpty, kEmpty};
  };

  std::vector<Node> nodes_;
  size_t pos_ = 0;

  size_t GetLink(size_t node) {
    if (nodes_[node].link == kEmpty) {
      if ((node == 0) || (nodes_[node].parent == 0)) {
        nodes_[node].link = 0;
      } else {
        nodes_[node].link =
            Move(GetLink(nodes_[node].parent), nodes_[node].pch);
      }
    }
    return nodes_[node].link;
  }

  size_t GetUp(size_t node) {
    if (nodes_[node].up == kEmpty) {
      if (nodes_[GetLink(node)].term) {
        nodes_[node].up = GetLink(node);
      } else if (GetLink(node) == 0) {
        nodes_[node].up = 0;
      } else {
        nodes_[node].up = GetUp(GetLink(node));
      }
    }
    return nodes_[node].up;
  }

  size_t Move(size_t node, size_t ch) {
    if (nodes_[node].go[ch] == kEmpty) {
      if (nodes_[node].next[ch] != kEmpty) {
        nodes_[node].go[ch] = nodes_[node].next[ch];
      } else if (node == 0) {
        nodes_[node].go[ch] = 0;
      } else {
        nodes_[node].go[ch] = Move(GetLink(node), ch);
      }
    }
    return nodes_[node].go[ch];
  }
};

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);
  size_t num_strings;
  std::cin >> num_strings;
  std::vector<string> strings(num_strings);
  for (size_t i = 0; i < num_strings; ++i) {
    std::cin >> strings[i];
  }
  Trie trie{strings};
  std::cout << (trie.HasInfiniteCycle() ? "TAK\n" : "NIE\n");
}
