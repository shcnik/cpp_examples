#include <algorithm>
#include <cmath>
#include <complex>
#include <iostream>
#include <numbers>
#include <string>
#include <valarray>
#include <vector>

using std::complex;

using Polynom = std::valarray<complex<double>>;

Polynom FFT(Polynom& polynom, bool invert = false) {
  if (polynom.size() == 1) {
    return polynom;
  }
  for (size_t i = 1, j = 0; i < polynom.size(); ++i) {
    size_t bit = polynom.size() >> 1;
    while (j >= bit) {
      j -= bit;
      bit >>= 1;
    }
    j += bit;
    if (i < j) {
      auto tmp = polynom[i];
      polynom[i] = polynom[j];
      polynom[j] = tmp;
    }
  }
  for (size_t len = 2; len <= polynom.size(); len <<= 1) {
    double ang = 2 * std::numbers::pi / static_cast<double>(len) * (invert ? -1.0 : 1.0);
    complex<double> w_len(cos(ang), sin(ang));
    for (size_t i = 0; i < polynom.size(); i += len) {
      complex<double> root(1);
      for (size_t j = 0; j < len / 2; ++j) {
        complex<double> next1 = polynom[i + j];
        complex<double> next2 = polynom[i + j + len / 2] * root;
        polynom[i + j] = next1 + next2;
        polynom[i + j + len / 2] = next1 - next2;
        root *= w_len;
      }
    }
  }
  if (invert) {
    polynom /= polynom.size();
  }
  return polynom;
}

Polynom Multiply(const Polynom& first, const Polynom& second) {
  size_t deg = 1;
  while (deg < first.size() + second.size() - 1) {
    deg <<= 1;
  }
  Polynom fft_first(deg);
  Polynom fft_second(deg);
  for (size_t i = 0; i < first.size(); ++i) {
    fft_first[i] = first[i];
  }
  for (size_t i = 0; i < second.size(); ++i) {
    fft_second[i] = second[i];
  }
  auto arr_first = FFT(fft_first);
  auto arr_second = FFT(fft_second);
  Polynom arr = arr_first * arr_second;
  auto res = FFT(arr, true);
  return res[std::slice{0, first.size() + second.size() - 1, 1}];
}

int main() {
  std::string str;
  std::cin >> str;
  Polynom arr(str.length());
  for (size_t i = 0; i < str.length(); ++i) {
    arr[i] = str[i] - '0';
  }
  auto res_arr = Multiply(arr, arr);
  size_t res = 0;
  for (size_t i = 0; 2 * i < res_arr.size(); ++i) {
    if (str[i] == '0') {
      continue;
    }
    res += static_cast<size_t>(floor((res_arr[2 * i] / 2.0).real() + 0.1) + (res_arr[2 * i].real() >= 0 ? 0.5 : -0.5));
  }
  std::cout << res << std::endl;
}
