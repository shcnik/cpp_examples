#include <bitset>
#include <functional>
#include <iostream>
#include <list>
#include <numeric>
#include <optional>
#include <queue>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

template <typename Vertex, typename Weight>
class Heuristic {
 public:
  virtual Weight operator()(const Vertex&) const = 0;
};

enum MoveDirection : char { UP = 'D', DOWN = 'U', LEFT = 'R', RIGHT = 'L' };

template <size_t size>
class State {
 public:
  using Piece = uint64_t;

  static constexpr Piece kEmptyPiece = 0;

  State(const std::vector<Piece>& state) {
    state_.reset();
    for (size_t i = 0; i < NumPieces(); ++i) {
      state_ |= (std::bitset<size * size * size>(state[i]) << (size * i));
    }
  }

  State() : State(FinalPosition()) {}

  Piece GetPiece(size_t position) const {
    return ((state_ >> (size * (position - 1))) & kMask).to_ullong();
  }

  size_t operator[](size_t piece) const {
    for (size_t i = 0; i < NumPieces(); ++i) {
      if (GetPiece(i + 1) == (piece % NumPieces())) {
        return i + 1;
      }
    }
    return 0;
  }

  size_t NumPieces() const { return size * size; }

  size_t Distance(size_t piece, size_t position) const {
    size_t x1 = (operator[](piece) - 1) / size;
    size_t y1 = (operator[](piece) - 1) % size;
    size_t x2 = (position - 1) / size;
    size_t y2 = (position - 1) % size;
    size_t res = 0;
    if (x1 > x2) {
      res += x1 - x2;
    } else {
      res += x2 - x1;
    }
    if (y1 > y2) {
      res += y1 - y2;
    } else {
      res += y2 - y1;
    }
    return res;
  }

  size_t Displacement(Piece piece) const {
    if (piece == kEmptyPiece) {
      return Distance(kEmptyPiece, size * size);
    }
    return Distance(piece, piece);
  }

  State<size> Move(MoveDirection dir) const {
    State<size> res = *this;
    auto empty_pos = operator[](kEmptyPiece);
    switch (dir) {
      case MoveDirection::UP:
        if ((operator[](kEmptyPiece) - 1) / size == size - 1) {
          return res;
        }
        res.state_ |=
            ((kMask << (size * (operator[](kEmptyPiece) + size - 1))) &
             state_) >>
            (size * size);
        res.state_ &= ~(kMask << (size * (empty_pos + size - 1)));
        break;
      case MoveDirection::DOWN:
        if ((operator[](kEmptyPiece) - 1) / size == 0) {
          return res;
        }
        res.state_ |=
            ((kMask << (size * (operator[](kEmptyPiece) - size - 1))) & state_)
            << (size * size);
        res.state_ &= ~(kMask << (size * (empty_pos - size - 1)));
        break;
      case MoveDirection::LEFT:
        if ((operator[](kEmptyPiece) - 1) % size == size - 1) {
          return res;
        }
        res.state_ |=
            ((kMask << (size * operator[](kEmptyPiece))) & state_) >> size;
        res.state_ &= ~(kMask << (size * empty_pos));
        break;
      case MoveDirection::RIGHT:
        if ((operator[](kEmptyPiece) - 1) % size == 0) {
          return res;
        }
        res.state_ |=
            ((kMask << (size * (operator[](kEmptyPiece) - 2))) & state_)
            << size;
        res.state_ &= ~(kMask << (size * (empty_pos - 2)));
        break;
    }
    return res;
  }

  bool IsMovePossible(MoveDirection dir) const { return Move(dir) != *this; }

  bool operator==(const State<size>& other) const {
    return state_ == other.state_;
  }

  bool operator!=(const State<size>& other) const {
    return state_ != other.state_;
  }

  size_t Hash() const {
    return std::hash<std::bitset<size * size * size>>{}(state_);
  }

 private:
  static constexpr std::bitset<size* size* size> kMask =
      std::bitset<size * size * size>{(1ull << size) - 1};

  static std::vector<Piece> FinalPosition() {
    std::vector<Piece> pieces(size * size);
    std::iota(pieces.begin(), pieces.end(), 1);
    pieces[size * size - 1] = 0;
    return pieces;
  }

  std::bitset<size * size * size> state_;
};

template <>
class State<4> {
 public:
  using Piece = uint64_t;

  static constexpr Piece kEmptyPiece = 0;

  State(const std::vector<Piece>& state) : state_(0) {
    for (size_t i = 0; i < NumPieces(); ++i) {
      state_ |= (state[i] << (4 * i));
    }
  }

  State() : State(FinalPosition()) {}

  Piece GetPiece(size_t position) const {
    return ((state_ >> (4 * (position - 1))) & kMask).to_ullong();
  }

  size_t operator[](Piece piece) const {
    for (size_t i = 0; i < NumPieces(); ++i) {
      if (GetPiece(i + 1) == (piece % NumPieces())) {
        return i + 1;
      }
    }
    return 0;
  }

  size_t NumPieces() const { return 16; }

  size_t Distance(Piece piece, size_t position) const {
    size_t x1 = (operator[](piece) - 1) / 4;
    size_t y1 = (operator[](piece) - 1) % 4;
    size_t x2 = (position - 1) / 4;
    size_t y2 = (position - 1) % 4;
    size_t res = 0;
    if (x1 > x2) {
      res += x1 - x2;
    } else {
      res += x2 - x1;
    }
    if (y1 > y2) {
      res += y1 - y2;
    } else {
      res += y2 - y1;
    }
    return res;
  }

  size_t Displacement(Piece piece) const {
    if (piece == kEmptyPiece) {
      return Distance(kEmptyPiece, 16);
    }
    return Distance(piece, piece);
  }

  State<4> Move(MoveDirection dir) const {
    State<4> res = *this;
    auto empty_pos = operator[](kEmptyPiece);
    switch (dir) {
      case MoveDirection::UP:
        if ((operator[](kEmptyPiece) - 1) / 4 == 4 - 1) {
          return res;
        }
        res.state_ |= ((kMask << (4 * (empty_pos + 3))) & state_) >> 16;
        res.state_ &= ~(kMask << (4 * (empty_pos + 3)));
        break;
      case MoveDirection::DOWN:
        if ((operator[](kEmptyPiece) - 1) / 4 == 0) {
          return res;
        }
        res.state_ |= ((kMask << (4 * (empty_pos - 5))) & state_) << 16;
        res.state_ &= ~(kMask << (4 * (empty_pos - 5)));
        break;
      case MoveDirection::LEFT:
        if ((operator[](kEmptyPiece) - 1) % 4 == 3) {
          return res;
        }
        res.state_ |= ((kMask << (4 * empty_pos)) & state_) >> 4;
        res.state_ &= ~(kMask << (4 * empty_pos));
        break;
      case MoveDirection::RIGHT:
        if ((operator[](kEmptyPiece) - 1) % 4 == 0) {
          return res;
        }
        res.state_ |= ((kMask << (4 * (empty_pos - 2))) & state_) << 4;
        res.state_ &= ~(kMask << (4 * (empty_pos - 2)));
        break;
    }
    return res;
  }

  bool IsMovePossible(MoveDirection dir) const { return Move(dir) != *this; }

  bool operator==(const State<4>& other) const {
    return state_ == other.state_;
  }

  bool operator!=(const State<4>& other) const {
    return state_ != other.state_;
  }

  size_t Hash() const { return std::hash<std::bitset<64>>{}(state_); }

 private:
  static constexpr std::bitset<64> kMask{0xF};

  static std::vector<Piece> FinalPosition() {
    std::vector<Piece> pieces(16);
    std::iota(pieces.begin(), pieces.end(), 1);
    pieces[15] = 0;
    return pieces;
  }

  std::bitset<64> state_ = 0;
};

template <>
class State<3> {
 public:
  using Piece = uint64_t;

  static constexpr Piece kEmptyPiece = 0;

  State(const std::vector<Piece>& state) : state_(0) {
    for (size_t i = 0; i < NumPieces(); ++i) {
      state_ |= (state[i] << (4 * i));
    }
  }

  State() : State(FinalPosition()) {}

  Piece GetPiece(size_t position) const {
    return ((state_ >> (4 * (position - 1))) & kMask).to_ullong();
  }

  size_t operator[](Piece piece) const {
    for (size_t i = 0; i < NumPieces(); ++i) {
      if (GetPiece(i + 1) == (piece % NumPieces())) {
        return i + 1;
      }
    }
    return 0;
  }

  size_t NumPieces() const { return 9; }

  size_t Distance(Piece piece, size_t position) const {
    size_t x1 = (operator[](piece) - 1) / 3;
    size_t y1 = (operator[](piece) - 1) % 3;
    size_t x2 = (position - 1) / 3;
    size_t y2 = (position - 1) % 3;
    size_t res = 0;
    if (x1 > x2) {
      res += x1 - x2;
    } else {
      res += x2 - x1;
    }
    if (y1 > y2) {
      res += y1 - y2;
    } else {
      res += y2 - y1;
    }
    return res;
  }

  size_t Displacement(Piece piece) const {
    if (piece == kEmptyPiece) {
      return Distance(piece, 9);
    }
    return Distance(piece, piece);
  }

  State<3> Move(MoveDirection dir) const {
    State<3> res = *this;
    auto empty_pos = operator[](kEmptyPiece);
    switch (dir) {
      case MoveDirection::UP:
        if ((operator[](kEmptyPiece) - 1) / 3 == 2) {
          return res;
        }
        res.state_ |= ((kMask << (4 * (empty_pos + 2))) & state_) >> 12;
        res.state_ &= ~(kMask << (4 * (empty_pos + 2)));
        break;
      case MoveDirection::DOWN:
        if ((operator[](kEmptyPiece) - 1) / 3 == 0) {
          return res;
        }
        res.state_ |= ((kMask << (4 * (empty_pos - 4))) & state_) << 12;
        res.state_ &= ~(kMask << (4 * (empty_pos - 4)));
        break;
      case MoveDirection::LEFT:
        if ((operator[](kEmptyPiece) - 1) % 3 == 2) {
          return res;
        }
        res.state_ |= ((kMask << (4 * empty_pos)) & state_) >> 4;
        res.state_ &= ~(kMask << (4 * empty_pos));
        break;
      case MoveDirection::RIGHT:
        if ((operator[](kEmptyPiece) - 1) % 3 == 0) {
          return res;
        }
        res.state_ |= ((kMask << (4 * (empty_pos - 2))) & state_) << 4;
        res.state_ &= ~(kMask << (4 * (empty_pos - 2)));
        break;
    }
    return res;
  }

  bool IsMovePossible(MoveDirection dir) const { return Move(dir) != *this; }

  bool operator==(const State<3>& other) const {
    return state_ == other.state_;
  }

  bool operator!=(const State<3>& other) const {
    return state_ != other.state_;
  }

  size_t Hash() const { return std::hash<std::bitset<64>>{}(state_); }

 private:
  static constexpr std::bitset<64> kMask{0xF};

  static std::vector<Piece> FinalPosition() {
    std::vector<Piece> pieces(9);
    std::iota(pieces.begin(), pieces.end(), 1);
    pieces[8] = 0;
    return pieces;
  }

  std::bitset<64> state_ = 0;
};

template <size_t size>
struct std::hash<State<size>> {
  size_t operator()(const State<size>& state) const { return state.Hash(); }
};

template <size_t size>
class DisplacedHeuristic : public Heuristic<State<size>, size_t> {
 public:
  size_t operator()(const State<size>& state) const override {
    size_t res = 0;
    for (size_t i = 0; i < state.NumPieces(); ++i) {
      if (state[i] != i) {
        ++res;
      }
    }
    return res;
  }
};

template <size_t size>
class DistanceHeuristic : public Heuristic<State<size>, size_t> {
 public:
  size_t operator()(const State<size>& state) const override {
    size_t res = 0;
    for (size_t i = 1; i < state.NumPieces(); ++i) {
      res += state.Displacement(i);
    }
    return res;
  }
};

template <size_t size>
class EmptyDistanceHeuristic : public Heuristic<State<size>, size_t> {
 public:
  size_t operator()(const State<size>& state) const override {
    return state.Displacement(state.kEmptyPiece);
  }
};

template <size_t size>
class CornerDistanceHeuristic : public Heuristic<State<size>, size_t> {
 public:
  size_t operator()(const State<size>& state) const override {
    return state.Displacement(1) + state.Displacement(size) +
           state.Displacement(state.NumPieces() - size + 1) +
           state.Displacement(State<size>::kEmptyPiece);
  }
};

template <size_t size>
class LinearConflictHeuristic : public Heuristic<State<size>, size_t> {
 public:
  size_t operator()(const State<size>& state) const override {
    size_t res = 0;
    for (size_t i = 0; i < size; ++i) {
      for (size_t j = 0; j < size; ++j) {
        if ((state[size * i + j + 1] / size) > i) {
          ++res;
        }
        if (((state[size * i + j + 1] - 1) % size) > j) {
          ++res;
        }
      }
    }
    return res;
  }
};

template <typename Vertex, typename Weight>
class ConvexHeuristic : public Heuristic<Vertex, Weight> {
 public:
  ConvexHeuristic(
      const std::vector<const Heuristic<Vertex, Weight>&>& heuristics,
      const std::vector<Weight>& coefficients) {
    if (heuristics.size() != coefficients.size()) {
      throw std::logic_error(
          "numbers of heuristics and coefficients don't match");
    }
    for (size_t i = 0; i < heuristics.size(); ++i) {
      combination_.push_back({heuristics[i], coefficients[i]});
    }
  }

  Weight operator()(const Vertex& vertex) const override {
    Weight res = 0;
    for (auto member : combination_) {
      res += member.second * member.first(vertex);
    }
  }

 private:
  std::vector<std::pair<const Heuristic<Vertex, Weight>&, Weight>> combination_;
};

template <size_t size>
class GameTable {
 public:
  struct Move {
    State<size> first;
    State<size> second;
    MoveDirection move;
    size_t weight = 1;
  };

  using VertexType = State<size>;
  using EdgeType = Move;
  using WeightType = size_t;

  std::list<Move> GetNeighbours(const State<size>& state) const {
    std::list<Move> res;
    if (state.IsMovePossible(MoveDirection::UP)) {
      res.push_back({state, state.Move(MoveDirection::UP), MoveDirection::UP});
    }
    if (state.IsMovePossible(MoveDirection::DOWN)) {
      res.push_back(
          {state, state.Move(MoveDirection::DOWN), MoveDirection::DOWN});
    }
    if (state.IsMovePossible(MoveDirection::LEFT)) {
      res.push_back(
          {state, state.Move(MoveDirection::LEFT), MoveDirection::LEFT});
    }
    if (state.IsMovePossible(MoveDirection::RIGHT)) {
      res.push_back(
          {state, state.Move(MoveDirection::RIGHT), MoveDirection::RIGHT});
    }
    return res;
  }
};

template <typename Graph>
class Visitor {
 public:
  virtual void Initialize(const Graph& /*unused*/) {}
  virtual void Start(const Graph& /*unused*/,
                     const typename Graph::VertexType& /*unused*/) {}
  virtual void Discover(const Graph& /*unused*/,
                        const typename Graph::VertexType& /*unused*/) {}
  virtual void ExamineVertex(const Graph& /*unused*/,
                             const typename Graph::VertexType& /*unused*/) {}
  virtual void ExamineEdge(const Graph& /*unused*/,
                           const typename Graph::EdgeType& /*unused*/) {}
  virtual void Optimize(const Graph& /*unused*/,
                        const typename Graph::EdgeType& /*unused*/) {}
};

template <size_t size>
class GameVisitor : public Visitor<GameTable<size>> {
 public:
  void Optimize(const GameTable<size>& /*unused*/,
                const typename GameTable<size>::Move& move) override {
    parents_[move.second] = move;
  }

  std::optional<std::list<typename GameTable<size>::Move>> GetPath(
      const State<size>& start, const State<size>& finish) const {
    std::list<typename GameTable<size>::Move> res;
    State<size> cur = finish;
    while (parents_.count(cur) != 0) {
      res.push_front(parents_.at(cur));
      cur = parents_.at(cur).first;
    }
    if ((res.empty() && (start != finish)) ||
        (!res.empty() && (res.front().first != start))) {
      return std::nullopt;
    }
    return res;
  }

 private:
  std::unordered_map<State<size>, typename GameTable<size>::Move> parents_;
};

template <typename Graph, typename Visitor>
class AStarSearch {
 public:
  AStarSearch(const Heuristic<typename Graph::VertexType,
                              typename Graph::WeightType>& heuristic)
      : heuristic_(heuristic) {}

  Visitor operator()(const Graph& graph,
                     const typename Graph::VertexType& start,
                     const typename Graph::VertexType& finish) {
    visitor_.Initialize(graph);
    std::unordered_set<typename Graph::VertexType> used;
    auto cmp = [](QueueItem first, QueueItem second) {
      return first.first > second.first;
    };
    std::priority_queue<QueueItem, std::vector<QueueItem>, decltype(cmp)> queue(
        cmp);
    std::unordered_map<typename Graph::VertexType, typename Graph::WeightType>
        dist;
    dist[start] = 0;
    std::unordered_map<typename Graph::VertexType, typename Graph::WeightType>
        est;
    est[start] = dist.at(start) + heuristic_(start);
    queue.push({est.at(start), start});
    visitor_.Start(graph, start);
    while (!queue.empty()) {
      auto cur = queue.top().second;
      queue.pop();
      if (used.count(cur) != 0) {
        continue;
      }
      visitor_.ExamineVertex(graph, cur);
      used.insert(cur);
      if (cur == finish) {
        break;
      }
      for (auto next : graph.GetNeighbours(cur)) {
        visitor_.ExamineEdge(graph, next);
        auto score = dist.at(cur) + next.weight;
        if ((used.count(next.second) != 0) && (score >= dist.at(next.second))) {
          continue;
        }
        visitor_.Optimize(graph, next);
        dist[next.second] = score;
        est[next.second] = dist.at(next.second) + heuristic_(next.second);
        queue.push({est.at(next.second), next.second});
      }
    }
    return visitor_;
  }

 private:
  using QueueItem =
      std::pair<typename Graph::WeightType, typename Graph::VertexType>;

  Visitor visitor_;
  const Heuristic<typename Graph::VertexType, typename Graph::WeightType>&
      heuristic_;
};

int main() {
  std::vector<State<3>::Piece> pieces(9);
  for (size_t i = 0; i < 9; ++i) {
    std::cin >> pieces[i];
  }
  State<3> start(pieces);
  State<3> finish;
  auto res = AStarSearch<GameTable<3>, GameVisitor<3>>{DistanceHeuristic<3>{}}(
                 GameTable<3>{}, start, finish)
                 .GetPath(start, finish);
  if (!res.has_value()) {
    std::cout << -1 << std::endl;
  } else {
    std::cout << res->size() << std::endl;
    for (auto cur : *res) {
      std::cout << static_cast<char>(cur.move);
    }
    std::cout << std::endl;
  }
}
