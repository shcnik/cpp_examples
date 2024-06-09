#include <iostream>
#include <string>
#include <vector>

using Deck = std::vector<std::vector<bool>>;

class GetPairing {
 public:
  GetPairing(const Deck& deck)
      : deck_(deck),
        px_(deck_.size(),
            std::vector<std::pair<size_t, size_t>>(deck_[0].size(), kEmpty)),
        py_(deck_.size(),
            std::vector<std::pair<size_t, size_t>>(deck_[0].size(), kEmpty)),
        used_(deck_.size(), std::vector<bool>(deck_[0].size(), false)) {}

  int64_t operator()() {
    for (size_t i = 0; i < deck_.size(); ++i) {
      for (size_t j = 0; j < deck_[i].size(); ++j) {
        px_[i][j] = kEmpty;
        py_[i][j] = kEmpty;
        used_[i][j] = false;
      }
    }
    bool is_path = true;
    while (is_path) {
      is_path = false;
      for (size_t i = 0; i < deck_.size(); ++i) {
        for (size_t j = 0; j < deck_[i].size(); ++j) {
          used_[i][j] = false;
        }
      }
      for (size_t i = 0; i < deck_.size(); ++i) {
        for (size_t j = i % 2; j < deck_[i].size(); j += 2) {
          if (!deck_[i][j]) {
            continue;
          }
          if (px_[i][j] == kEmpty) {
            if (DFS(i, j)) {
              is_path = true;
            }
          }
        }
      }
    }
    int64_t res = 0;
    for (size_t i = 0; i < deck_.size(); ++i) {
      for (size_t j = i % 2; j < deck_[i].size(); j += 2) {
        if (px_[i][j] != kEmpty) {
          ++res;
        }
      }
    }
    return res;
  }

 private:
  bool DFS(size_t row, size_t col) {
    if (used_[row][col]) {
      return false;
    }
    used_[row][col] = true;
    if ((row > 0) && deck_[row - 1][col]) {
      if (py_[row - 1][col] == kEmpty) {
        py_[row - 1][col] = {row, col};
        px_[row][col] = {row - 1, col};
        return true;
      }
      if (DFS(py_[row - 1][col].first, py_[row - 1][col].second)) {
        py_[row - 1][col] = {row, col};
        px_[row][col] = {row - 1, col};
        return true;
      }
    }
    if ((col > 0) && deck_[row][col - 1]) {
      if (py_[row][col - 1] == kEmpty) {
        py_[row][col - 1] = {row, col};
        px_[row][col] = {row, col - 1};
        return true;
      }
      if (DFS(py_[row][col - 1].first, py_[row][col - 1].second)) {
        py_[row][col - 1] = {row, col};
        px_[row][col] = {row, col - 1};
        return true;
      }
    }
    if ((row < deck_.size() - 1) && deck_[row + 1][col]) {
      if (py_[row + 1][col] == kEmpty) {
        py_[row + 1][col] = {row, col};
        px_[row][col] = {row + 1, col};
        return true;
      }
      if (DFS(py_[row + 1][col].first, py_[row + 1][col].second)) {
        py_[row + 1][col] = {row, col};
        px_[row][col] = {row + 1, col};
        return true;
      }
    }
    if ((col < deck_[row].size() - 1) && deck_[row][col + 1]) {
      if (py_[row][col + 1] == kEmpty) {
        py_[row][col + 1] = {row, col};
        px_[row][col] = {row, col + 1};
        return true;
      }
      if (DFS(py_[row][col + 1].first, py_[row][col + 1].second)) {
        py_[row][col + 1] = {row, col};
        px_[row][col] = {row, col + 1};
        return true;
      }
    }
    return false;
  }

  static constexpr std::pair<size_t, size_t> kEmpty = {
      std::numeric_limits<size_t>::max(), std::numeric_limits<size_t>::max()};
  const Deck& deck_;
  std::vector<std::vector<std::pair<size_t, size_t>>> px_;
  std::vector<std::vector<std::pair<size_t, size_t>>> py_;
  std::vector<std::vector<bool>> used_;
};

int main() {
  size_t height;
  size_t width;
  int64_t cost_unary;
  int64_t cost_binary;
  std::cin >> height >> width >> cost_binary >> cost_unary;
  Deck deck(height, std::vector<bool>(width));
  int64_t num_intact = 0;
  for (size_t i = 0; i < height; ++i) {
    std::string line;
    std::cin >> line;
    for (size_t j = 0; j < width; ++j) {
      deck[i][j] = (line[j] == '*');
      if (deck[i][j]) {
        ++num_intact;
      }
    }
  }
  int64_t delta = 2 * cost_unary - cost_binary;
  int64_t num_binary = (delta > 0) ? GetPairing{deck}() : 0;
  std::cout << cost_unary * num_intact - delta * num_binary << std::endl;
}
