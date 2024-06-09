#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::vector;

class PrefixFunction {
 public:
  PrefixFunction(const string& str) : prefix_(str.size(), 0) {
    for (size_t i = 1; i < str.size(); ++i) {
      size_t j = prefix_[i - 1];
      while ((j > 0) && (str[i] != str[j])) {
        j = prefix_[j - 1];
      }
      if (str[i] == str[j]) {
        ++j;
      }
      prefix_[i] = j;
    }
  }

  size_t operator()(size_t index) const { return prefix_[index]; }

  size_t max_index() const { return prefix_.size() - 1; }

 private:
  vector<size_t> prefix_;
};

vector<size_t> FindPattern(const string& text, const string& pattern) {
  constexpr char kSeparator = '#';
  auto prefix = PrefixFunction{pattern + kSeparator + text};
  vector<size_t> res;
  for (size_t i = 0; i <= prefix.max_index(); ++i) {
    if (prefix(i) == pattern.size()) {
      res.push_back(i - 2 * pattern.size());
    }
  }
  return res;
}

int main() {
  string text;
  string pattern;
  std::cin >> text >> pattern;
  auto res = FindPattern(text, pattern);
  for (auto index : res) {
    std::cout << index << std::endl;
  }
  return 0;
}
