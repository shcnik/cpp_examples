#include <iostream>
#include <iterator>
#include <limits>
#include <locale>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

template <typename Symbol>
using String = std::basic_string<Symbol>;

template <typename Symbol>
class Automaton {
 public:
  virtual ~Automaton() = default;
  virtual void Reset() = 0;
  virtual bool Move(Symbol) = 0;
};

template <typename Symbol>
class SuffixAutomaton : public Automaton<Symbol> {
 public:
  SuffixAutomaton() : nodes_(1) {}

  void Extend(Symbol sym) {
    Node& new_node = nodes_.emplace_back();
    new_node.len = nodes_[final_].len + 1;
    size_t new_id = nodes_.size() - 1;
    size_t cur = final_;
    while ((cur != kEmpty) && !nodes_[cur].go.contains(sym)) {
      nodes_[cur].go.emplace(sym, new_id);
      cur = nodes_[cur].link;
      if ((cur > nodes_.size()) && (cur != kEmpty)) return;
    }
    if (cur != kEmpty) {
      size_t next = nodes_[cur].go[sym];
      if (nodes_[cur].len + 1 == nodes_[next].len) {
        new_node.link = next;
      } else {
        Node& clone = nodes_.emplace_back(nodes_[next]);
        size_t clone_id = nodes_.size() - 1;
        clone.len = nodes_[cur].len + 1;
        nodes_[next].link = new_node.link = clone_id;
        while ((cur != kEmpty) && (nodes_[cur].go[sym] == next)) {
          nodes_[cur].go[sym] = clone_id;
          cur = nodes_[cur].link;
          if ((cur != kEmpty) && !nodes_[cur].go.contains(sym)) {
            return;
          }
        }
      }
    } else {
      new_node.link = 0;
    }
    final_ = new_id;
  }

  void Reset() { state_ = 0; }

  bool Move(Symbol sym) {
    if (!nodes_[state_].go.contains(sym)) {
      return false;
    }
    state_ = nodes_[state_].go[sym];
    return true;
  }

 private:
  static constexpr size_t kEmpty = std::numeric_limits<size_t>::max();

  struct Node {
    size_t len = 0;
    size_t link = kEmpty;
    std::unordered_map<Symbol, size_t> go;
  };

  std::vector<Node> nodes_;
  size_t final_ = 0;
  size_t state_ = 0;
};

namespace commands {
const std::string kAppend = "A";
const std::string kSearch = "?";
}  // namespace commands

int main() {
  std::string command;
  SuffixAutomaton<char> automat;
  while (std::cin >> command) {
    if (command == commands::kAppend) {
      std::string word;
      std::cin >> word;
      for (auto ch : word) {
        automat.Extend(std::tolower(ch, std::locale()));
      }
    } else if (command == commands::kSearch) {
      std::string pattern;
      std::cin >> pattern;
      bool res = true;
      automat.Reset();
      for (auto ch : pattern) {
        res = automat.Move(std::tolower(ch, std::locale()));
        if (!res) {
          break;
        }
      }
      std::cout << (res ? "YES\n" : "NO\n");
    }
  }
}
