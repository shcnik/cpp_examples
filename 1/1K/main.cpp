#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using Graph = std::vector<std::unordered_map<int, int>>;
using Vertex = int;

struct Formula {
  int coef;
  Vertex delta;

  Vertex Compute(Vertex x) { return coef * x + delta; }
};

std::vector<Formula> BFS(const Graph& graph, Vertex start) {
  std::vector<Formula> formulae(graph.size());
  std::vector<bool> used(graph.size());
  std::queue<Vertex> queue;
  formulae[start] = {1, 0};
  queue.push(start);
  used[start] = true;
  while (!queue.empty()) {
    Vertex cur = queue.front();
    queue.pop();
    for (auto next : graph[cur]) {
      if (used[next.first]) {
        if (formulae[next.first].coef != -formulae[cur].coef) {
          formulae[start].coef = 0;
          formulae[start].delta =
              (next.second - formulae[cur].delta - formulae[next.first].delta) /
              (formulae[next.first].coef + formulae[cur].coef);
        }
        continue;
      }
      formulae[next.first] = {-formulae[cur].coef,
                              next.second - formulae[cur].delta};
      queue.push(next.first);
      used[next.first] = true;
    }
  }
  return formulae;
}

using Language = Vertex;
using Subprocedure = int;

int main() {
  int num_lang, num_subproc;
  std::cin >> num_lang >> num_subproc;
  Graph graph(num_lang + 1);
  for (int i = 1; i <= num_subproc; ++i) {
    Language first, second;
    Subprocedure subproc;
    std::cin >> first >> second >> subproc;
    graph[first].insert({second, subproc});
    graph[second].insert({first, subproc});
  }
  auto formulae = BFS(graph, 1);
  std::vector<Language> res(num_lang + 1);
  if (formulae[1].coef == 0) {
    res[1] = formulae[1].delta;
    for (Language lang = 2; lang <= num_lang; ++lang) {
      res[lang] = formulae[lang].Compute(res[1]);
    }
  } else {
    Formula max_plus{1, std::numeric_limits<int>::min()},
        min_plus{1, std::numeric_limits<int>::max()},
        max_minus{-1, std::numeric_limits<int>::min()},
        min_minus{-1, std::numeric_limits<int>::max()};
    for (Language lang = 1; lang <= num_lang; ++lang) {
      if (formulae[lang].coef == 1) {
        max_plus.delta = std::max(max_plus.delta, formulae[lang].delta);
        min_plus.delta = std::min(min_plus.delta, formulae[lang].delta);
      } else if (formulae[lang].coef == -1) {
        max_minus.delta = std::max(max_minus.delta, formulae[lang].delta);
        min_minus.delta = std::min(min_minus.delta, formulae[lang].delta);
      }
    }
    Language res_min = std::max(1 - min_plus.delta, max_minus.delta - num_lang);
    Language res_max = std::min(min_minus.delta - 1, num_lang - max_plus.delta);
    for (Language x = res_min; x <= res_max; ++x) {
      std::unordered_set<Language> used;
      for (Language lang = 1; lang <= num_lang; ++lang) {
        res[lang] = formulae[lang].Compute(x);
        if (used.count(res[lang]) != 0) {
          break;
        }
        used.insert(res[lang]);
      }
      if (static_cast<int>(used.size()) == num_lang) {
        break;
      }
    }
  }
  for (int i = 1; i <= num_lang; ++i) {
    std::cout << res[i] << " ";
  }
  std::cout << std::endl;
}
