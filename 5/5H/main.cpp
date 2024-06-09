#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

template <typename Symbol>
using String = std::basic_string<Symbol>;

template <typename Symbol>
class Automaton {
 public:
  virtual ~Automaton() = default;
  virtual void Reset() = 0;
  virtual void Move(Symbol) = 0;
};

template <typename Symbol>
class AhoKorasikAutomaton : public Automaton<Symbol> {
 public:
  AhoKorasikAutomaton() : nodes_(1) {}

  template <typename InputIterator>
  requires std::input_iterator<InputIterator>
  AhoKorasikAutomaton(InputIterator begin, InputIterator end) : AhoKorasikAutomaton() {
    size_t total_size = 0;
    for (auto it = begin; it != end; ++it) {
      total_size += it->length();
    } 
    nodes_.reserve(total_size);
    for (auto it = begin; it != end; ++it) {
      AddPattern(*it);
    }
  }

  void AddPattern(const String<Symbol>& pattern) {
    size_t cur = 0;
    for (auto sym : pattern) {
      if (!nodes_[cur].sons.contains(sym)) {
        Node new_node;
        new_node.parent = cur;
        new_node.sym_to_parent = sym;
        nodes_[cur].sons[sym] = nodes_.size();
        nodes_.push_back(new_node);
      }
      cur = nodes_[cur].sons[sym];
    }
    nodes_[cur].is_leaf = true;
    nodes_[cur].leaf_patterns.push_back(patterns_.size());
    patterns_.push_back(pattern);
  }

  void Reset() override final {
    state_ = 0;
  }

  void Move(Symbol sym) override final {
    state_ = GetNext(state_, sym);
  }

  std::vector<std::vector<size_t>> TestString(const String<Symbol>& string) {
    Reset();
    std::vector<std::vector<size_t>> res(patterns_.size());
    for (size_t i = 0; i < patterns_.size(); ++i) {
      res[i].reserve(patterns_[i].length());
    }
    for (size_t i = 0; i < string.length(); ++i) {
      Move(string[i]);
      size_t cur = state_;
      for (size_t cur = state_; cur != 0; cur = GetUp(cur)) {
        for (size_t pattern : nodes_[cur].leaf_patterns) {
          res[pattern].push_back(i + 2 - patterns_[pattern].length());
        }
      }
    }
    return res;
  }

 private:
  static constexpr size_t kEmpty = std::numeric_limits<size_t>::max();

  struct Node {
    std::unordered_map<Symbol, size_t> sons;
    std::unordered_map<Symbol, size_t> go;
    size_t parent = kEmpty;
    size_t link = kEmpty;
    size_t up = kEmpty;
    Symbol sym_to_parent = static_cast<Symbol>(0);
    bool is_leaf = false;
    std::vector<size_t> leaf_patterns;
  };

  size_t GetLink(size_t state) {
    Node& node = nodes_[state];
    if (node.link == kEmpty) {
      if ((state == 0) || (node.parent == 0)) {
        node.link = 0;
      } else {
        node.link = GetNext(GetLink(node.parent), node.sym_to_parent);
      }
    }
    return node.link;
  }

  size_t GetNext(size_t state, Symbol sym) {
    Node& node = nodes_[state];
    if (!node.go.contains(sym)) {
      if (node.sons.contains(sym)) {
        node.go[sym] = node.sons[sym];
      } else if (state == 0) {
        node.go[sym] = 0;
      } else {
        node.go[sym] = GetNext(GetLink(state), sym);
      }
    }
    return node.go[sym];
  }

  size_t GetUp(size_t state) {
    Node& node = nodes_[state];
    if (node.up == kEmpty) {
      size_t link = GetLink(state);
      if (nodes_[link].is_leaf) {
        node.up = link;
      } else if (link == 0) {
        node.up = 0;
      } else {
        node.up = GetUp(link);
      }
    }
    return node.up;
  }

  std::vector<size_t> GetTerminals(size_t state) {
    if (state == 0) {
      return std::vector<size_t>{};
    }
    std::vector<size_t> patterns = GetTerminals(GetUp(state));
    for (auto pattern : nodes_[state].leaf_patterns) {
      patterns.push_back(pattern);
    }
    return patterns;
  }

  std::vector<Node> nodes_;
  std::vector<String<Symbol>> patterns_;
  size_t state_ = 0;
};

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);
  std::string text;
  std::cin >> text;
  size_t num_patterns;
  std::cin >> num_patterns;
  std::vector<std::string> patterns(num_patterns);
  for (size_t i = 0; i < num_patterns; ++i) {
    std::cin >> patterns[i];
  }
  AhoKorasikAutomaton<char> automaton(patterns.begin(), patterns.end());
  auto indices = automaton.TestString(text);
  for (auto poss : indices) {
    std::cout << poss.size() << " ";
    for (auto pos : poss) {
      std::cout << pos << " ";
    }
    std::cout << '\n';
  }
}
