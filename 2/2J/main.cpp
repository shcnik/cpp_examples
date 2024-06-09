#include <iostream>
#include <limits>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Ship {
 public:
  using Room = size_t;

  struct Path {
    Room first;
    Room second;
    size_t price;
  };

  struct Pricelist {
    size_t up_price;
    size_t down_price;
    size_t enter_price;
    size_t exit_price;
  };

  using Teleport = std::vector<Room>;

  Ship(size_t rooms, const std::vector<Teleport>& teleports,
       Pricelist pricelist)
      : adjacent_(rooms + 1 + teleports.size()),
        num_rooms_(rooms),
        pricelist_(pricelist) {
    for (size_t i = 0; i < teleports.size(); ++i) {
      for (Room room : teleports[i]) {
        adjacent_[room].push_back(rooms + 1 + i);
        adjacent_[rooms + 1 + i].push_back(room);
      }
    }
  }

  size_t CountRooms() const { return num_rooms_; }

  size_t Size() const { return adjacent_.size() - 1; }

  std::vector<Path> GetNeighbours(Room room) const {
    std::vector<Path> res;
    if (room <= CountRooms()) {
      if (room > 1) {
        res.push_back({room, room - 1, pricelist_.down_price});
      }
      if (room < CountRooms()) {
        res.push_back({room, room + 1, pricelist_.up_price});
      }
    }
    for (size_t neigh : adjacent_[room]) {
      if (neigh > num_rooms_) {
        res.push_back({room, neigh, pricelist_.enter_price});
      } else {
        res.push_back({room, neigh, pricelist_.exit_price});
      }
    }
    return res;
  }

 private:
  std::vector<std::vector<Room>> adjacent_;
  size_t num_rooms_;
  Pricelist pricelist_;
};

size_t FindMinimumPath(const Ship& ship, Ship::Room start, Ship::Room finish) {
  std::vector<size_t> dist(ship.Size() + 1, std::numeric_limits<size_t>::max());
  dist[start] = 0;
  std::vector<bool> used(ship.Size() + 1);
  std::priority_queue<std::pair<size_t, Ship::Room>,
                      std::vector<std::pair<size_t, Ship::Room>>,
                      std::greater<std::pair<size_t, Ship::Room>>>
      queue;
  queue.push({0, start});
  while (!queue.empty()) {
    auto cur = queue.top().second;
    queue.pop();
    if (used[cur]) {
      continue;
    }
    used[cur] = true;
    if (cur == finish) {
      return dist[finish];
    }
    for (auto next : ship.GetNeighbours(cur)) {
      if ((dist[next.first] + next.price < dist[next.second])) {
        dist[next.second] = dist[next.first] + next.price;
        queue.push({dist[next.second], next.second});
      }
    }
  }
  return dist[finish];
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);
  constexpr Ship::Room kNumRooms = 1000000;
  size_t office, up_price, down_price, num_teleports, enter_price, exit_price;
  std::cin >> office >> up_price >> down_price >> enter_price >> exit_price >>
      num_teleports;
  std::vector<Ship::Teleport> teleports(num_teleports);
  for (size_t i = 0; i < num_teleports; ++i) {
    size_t num_exits;
    std::cin >> num_exits;
    teleports[i].resize(num_exits);
    for (size_t j = 0; j < num_exits; ++j) {
      std::cin >> teleports[i][j];
    }
  }
  Ship ship(kNumRooms, teleports,
            {up_price, down_price, enter_price, exit_price});
  std::cout << FindMinimumPath(ship, 1, office) << std::endl;
}
