#include <cmath>
#include <complex>
#include <iostream>
#include <numbers>
#include <valarray>
#include <vector>

using std::complex;

using Polynom = std::valarray<complex<double>>;

Polynom FFT(const Polynom& polynom, bool invert = false) {
  if (polynom.size() == 1) {
    return polynom;
  }
  Polynom pol1(polynom.size() / 2);
  Polynom pol2(polynom.size() / 2);
  for (size_t i = 0, j = 0; i < polynom.size(); i += 2, ++j) {
    pol1[j] = polynom[i];
    pol2[j] = polynom[i + 1];
  }
  auto res1 = FFT(pol1, invert);
  auto res2 = FFT(pol2, invert);
  double angle = 2 * std::numbers::pi / static_cast<double>(polynom.size()) * (invert ? -1.0 : 1.0);
  complex<double> root = 1;
  complex<double> root_n{cos(angle), sin(angle)};
  Polynom res(polynom.size());
  for (size_t i = 0; i < polynom.size() / 2; ++i) {
    res[i] = res1[i] + root * res2[i];
    res[i + polynom.size() / 2] = res1[i] - root * res2[i];
    root *= root_n;
  }
  if (invert) {
    res /= 2.0;
  }
  return res;
}

Polynom multiply(const Polynom& first, const Polynom& second) {
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
  auto arr = arr_first * arr_second;
  auto res = FFT(arr, true);
  return res[std::slice{0, first.size() + second.size() - 1, 1}];
}

int main() {
  size_t deg_first;
  std::cin >> deg_first;
  Polynom polynom1(deg_first + 1);
  for (size_t i = 0; i <= deg_first; ++i) {
    std::cin >> polynom1[i];
  }
  size_t deg_second;
  std::cin >> deg_second;
  Polynom polynom2(deg_second + 1);
  for (size_t i = 0; i <= deg_second; ++i) {
    std::cin >> polynom2[i];
  }
  auto res = multiply(polynom1, polynom2);
  std::cout << res.size() - 1 << " ";
  for (size_t i = 0; i < res.size(); ++i) {
    std::cout << static_cast<int>(res[i].real() + (res[i].real() >= 0 ? 0.5 : -0.5)) << " ";
  }
  std::cout << std::endl;
}
