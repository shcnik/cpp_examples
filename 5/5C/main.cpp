#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using std::string;

std::pair<size_t, size_t> GCD(size_t value, size_t modulo) {
  if (value == 0) {
    return std::make_pair(0, 1);
  }
  auto [x1, y1] = GCD(modulo % value, value);
  return std::make_pair(y1 - (modulo / value) * x1, x1);
}

size_t Inverse(size_t value) {
  if (value == 0) {
    return 0;
  }
  auto [x1, y1] = GCD((~value + 1) % value, value);
  return y1 - ((~value + 1) / value + 1) * x1;
}

int main() {
  string pattern;
  std::cin >> pattern;
  string text;
  std::cin >> text;
  if (text.length() < pattern.length()) {
    std::cout << 0 << std::endl;
    return 0;
  }
  std::unordered_map<size_t, size_t> inverses;
  constexpr size_t kModulo = 31;
  size_t inverse = Inverse(kModulo);
  size_t hash = 0;
  size_t power = 1;
  power = 1;
  std::unordered_set<size_t> hashes;
  for (size_t i = 0; i < pattern.size(); ++i) {
    hash += static_cast<size_t>(pattern[i] - 'a' + 1) * power;
    power *= kModulo;
  }
  hashes.insert(hash);
  size_t lpower = 1;
  size_t inv_power = inverse;
  for (size_t i = 1; i < pattern.size(); ++i) {
    hash -= static_cast<size_t>(pattern[i - 1] - 'a' + 1) * lpower;
    lpower *= kModulo;
    hash += static_cast<size_t>(pattern[i - 1] - 'a' + 1) * power;
    power *= kModulo;
    hashes.insert(hash * inv_power);
    inv_power *= inverse;
  }
  hash = 0;
  power = 1;
  size_t ans = 0;
  for (size_t i = 0; i < pattern.size(); ++i) {
    hash += static_cast<size_t>(text[i] - 'a' + 1) * power;
    power *= kModulo;
  }
  if (hashes.contains(hash)) {
    ++ans;
  }
  lpower = 1;
  inv_power = inverse;
  for (size_t i = 1; i + pattern.size() <= text.size(); ++i) {
    hash -= static_cast<size_t>(text[i - 1] - 'a' + 1) * lpower;
    lpower *= kModulo;
    hash += static_cast<size_t>(text[i + pattern.size() - 1] - 'a' + 1) * power;
    power *= kModulo;
    if (hashes.contains(hash * inv_power)) {
      ++ans;
    }
    inv_power *= inverse;
  }
  std::cout << ans << std::endl;
  return 0;
}
