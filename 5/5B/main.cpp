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

 private:
  vector<size_t> prefix_;
};

void ConcatCompress(string& first, const string& second) {
  constexpr char kSeparator = '#';
  string unite = "";
  if (second.size() > first.size()) {
    unite = second.substr(0, first.size()) + kSeparator + first;
  } else {
    unite = second + kSeparator + first.substr(first.size() - second.size());
  }
  size_t cut = PrefixFunction{unite}(unite.size() - 1);
  first += second.substr(cut);
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);
  size_t num_words;
  std::cin >> num_words;
  vector<string> message(num_words);
  for (size_t i = 0; i < num_words; ++i) {
    std::cin >> message[i];
  }
  string compress = message[0];
  for (size_t i = 1; i < num_words; ++i) {
    ConcatCompress(compress, message[i]);
  }
  std::cout << compress << std::endl;
}
