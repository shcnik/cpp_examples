#include <iostream>
#include <string>
#include <vector>

using std::string;

class LongValue {
 public:
  LongValue(const string& number, uint8_t base)
      : number_(number.length()), base_(base) {
    for (size_t i = 0; i < number.length(); ++i) {
      number_[i] = static_cast<uint8_t>(number[i] - '0');
    }
  }

  void Convert(uint8_t new_base) {
    std::vector<uint8_t> res;
    do {
      res.push_back(NextDigit(new_base));
      while ((res.size() > 1) && (res.back() == 0)) {
        res.pop_back();
      }
    } while ((res.size() > 1) || (res.front() > 0));
    number_ = res;
    base_ = new_base;
  }

  string ToString() const {
    string res = "";
    for (size_t i = number_.size() - 1; i < number_.size(); --i) {
      res += static_cast<char>(number_[i] + '0');
    }
    return res;
  }

 private:
  uint8_t NextDigit(uint8_t new_base) {
    uint8_t temp = 0;
    for (size_t i = 0; i < number_.size(); ++i) {
      temp *= base_;
      temp += number_[i];
      number_[i] = temp / new_base;
      temp %= new_base;
    }
    return temp;
  }

  std::vector<uint8_t> number_;
  uint8_t base_;
};

int main() {
  uint8_t base;
  std::cin >> base;
  string number;
  std::cin >> number;
  LongValue value(number, base);
  uint8_t new_base;
  std::cin >> new_base;
  value.Convert(new_base);
  std::cout << value.ToString() << std::endl;
}