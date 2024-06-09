#include <algorithm>
#include <iostream>
#include <limits>
#include <optional>
#include <queue>
#include <unordered_map>

template <typename T>
class DisjointSetUnion {
 public:
  DisjointSetUnion() = default;

  template <typename Container>
  DisjointSetUnion(const Container& items) {
    for (T item : items) {
      MakeSet(item);
    }
  }

  void MakeSet(const T& item) {
    parents_[item] = std::nullopt;
    size_[item] = 1;
  }

  void Union(const T& first, const T& second) {
    T top_first = FindSet(first);
    T top_second = FindSet(second);
    if (top_first == top_second) {
      return;
    }
    if (size_[top_first] < size_[top_second]) {
      std::swap(top_first, top_second);
    }
    parents_[top_second] = top_first;
    size_[top_first] += size_[top_second];
  }

  bool IsSameSet(const T& first, const T& second) const {
    return FindSet(first) == FindSet(second);
  }

 private:
  T FindSet(const T& item) {
    if (parents_[item] == std::nullopt) {
      return item;
    }
    return *(parents_[item] = FindSet(*(parents_[item])));
  }

  T FindSet(const T& item) const {
    if (parents_.at(item) == std::nullopt) {
      return item;
    }
    return FindSet(*parents_.at(item));
  }

  std::unordered_map<T, std::optional<T>> parents_;
  std::unordered_map<T, size_t> size_;
};

using Room = size_t;

struct Tube {
  Room first;
  Room second;
  size_t length;

  Tube Inverse() { return Tube{second, first, length}; }
};

using Station = std::vector<std::vector<Tube>>;

using QueueItem = std::pair<size_t, Room>;

std::vector<std::pair<size_t, Tube>> FindLengths(
    const Station& station, const std::vector<Room>& starts) {
  constexpr size_t kInfinity = std::numeric_limits<size_t>::max();
  std::vector<size_t> dist(station.size(), kInfinity);
  std::priority_queue<QueueItem, std::vector<QueueItem>,
                      std::greater<QueueItem>>
      queue;
  for (Room start : starts) {
    dist[start] = 0;
    queue.push({0, start});
  }
  std::vector<bool> used(station.size(), false);
  std::vector<std::pair<size_t, Tube>> tubes;
  while (!queue.empty()) {
    Room cur = queue.top().second;
    queue.pop();
    if (used[cur]) {
      continue;
    }
    used[cur] = true;
    for (Tube edge : station[cur]) {
      tubes.push_back({0, edge});
      dist[edge.second] = std::min(dist[edge.second], dist[cur] + edge.length);
      queue.push({dist[edge.second], edge.second});
    }
  }
  for (size_t i = 0; i < tubes.size(); ++i) {
    tubes[i].first = dist[tubes[i].second.first] + tubes[i].second.length +
                     dist[tubes[i].second.second];
  }
  std::sort(tubes.begin(), tubes.end(),
            [](std::pair<size_t, Tube> first, std::pair<size_t, Tube> second) {
              return first.first < second.first;
            });
  return tubes;
}

DisjointSetUnion<Room> GetConnected(
    const Station& station, const std::vector<std::pair<size_t, Tube>>& tubes,
    size_t limit) {
  DisjointSetUnion<Room> res;
  for (size_t v = 1; v <= station.size(); ++v) {
    res.MakeSet(v);
  }
  auto end = std::upper_bound(
      tubes.begin(), tubes.end(), std::pair<size_t, Tube>{limit, {0, 0, 0}},
      [](std::pair<size_t, Tube> first, std::pair<size_t, Tube> second) {
        return first.first < second.first;
      });
  for (auto iter = tubes.begin(); iter < end; ++iter) {
    res.Union(iter->second.first, iter->second.second);
  }
  return res;
}

struct Query {
  size_t id;
  Room start;
  Room finish;
  size_t limit;
};

int main() {
  size_t num_rooms, num_tubes, num_rechargers;
  std::cin >> num_rooms >> num_rechargers >> num_tubes;
  std::vector<Room> rechargers(num_rechargers);
  for (size_t i = 0; i < num_rechargers; ++i) {
    std::cin >> rechargers[i];
  }
  Station station(num_rooms + 1);
  for (size_t i = 0; i < num_tubes; ++i) {
    Tube tube;
    std::cin >> tube.first >> tube.second >> tube.length;
    station[tube.first].push_back(tube);
    station[tube.second].push_back(tube.Inverse());
  }
  auto tubes = FindLengths(station, rechargers);
  DisjointSetUnion<Room> connect;
  for (size_t v = 1; v <= station.size(); ++v) {
    connect.MakeSet(v);
  }
  auto cur_tube = tubes.begin();
  size_t num_queries;
  std::cin >> num_queries;
  std::vector<Query> queries(num_queries);
  for (size_t q = 0; q < num_queries; ++q) {
    queries[q].id = q;
    std::cin >> queries[q].start >> queries[q].finish >> queries[q].limit;
  }
  std::sort(queries.begin(), queries.end(), [](Query first, Query second) {
    return first.limit < second.limit;
  });
  std::vector<bool> res(num_queries);
  for (auto query : queries) {
    while ((cur_tube != tubes.end()) && (cur_tube->first <= query.limit)) {
      connect.Union(cur_tube->second.first, cur_tube->second.second);
      ++cur_tube;
    }
    res[query.id] = connect.IsSameSet(query.start, query.finish);
  }
  for (bool item : res) {
    std::cout << (item ? "YES" : "NO") << std::endl;
  }
}
