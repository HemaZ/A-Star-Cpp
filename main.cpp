#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using std::cout;
using std::ifstream;
using std::istringstream;
using std::sort;
using std::string;
using std::vector;
// typedef std::pair<int,int> Point;
enum class State {kEmpty, kObstacle, kClosed, kPath, kStart, kFinish};

struct Point {
  int x;
  int y;
};
struct Node {
  Point location;
  int g;
  int h;
};

string CellString(State cell) {
  switch(cell) {
    case State::kObstacle: return "⛰️   ";
    case State::kPath: return "🚗   ";
    case State::kStart: return "🚦   ";
    case State::kFinish: return "🏁   ";
    default: return "0   "; 
  }
}

void PrintBoard(vector<vector<State>> board) {
  for (auto vec : board) {
    for (State i : vec) {
      cout << CellString(i);
    }
    cout << "\n";
  }
}

vector<vector<State>> ReadBoardFile(string filePath) {
  vector<vector<State>> board;
  ifstream board_file(filePath);
  if (board_file) {
    cout << "Reading Board File in " << filePath << "\n";
    string line;
    while (getline(board_file, line)) {
      istringstream stream_line(line);
      int n;
      char c;
      vector<State> row;
      while (stream_line >> n) {
        if (n == 0) {
          row.push_back(State::kEmpty);
        } else {
          row.push_back(State::kObstacle);
        }

        stream_line >> c;
      }
      board.push_back(row);
    }
  }
  return board;
}
int Heuristic(Point p1, Point p2) {
  return (p2.x - p1.x) + (p2.y - p1.y); // (X2-X1) + (Y2-Y1)
}

void AddToOpen(Node node, vector<Node> &openNodes,
               vector<vector<State>> &board) {
  openNodes.push_back(node);
  board[node.location.x][node.location.y] = State::kClosed;
}

bool Compare(Node n1, Node n2) { return ((n1.g + n1.h) > (n2.g + n2.h)); }

void CellSort(vector<Node> *v) { sort(v->begin(), v->end(), Compare); }

bool CheckValidCell(Point p, vector<vector<State>> &grid) {
  bool on_grid_x = (p.x >= 0 && p.x < grid.size());
  bool on_grid_y = (p.y >= 0 && p.y < grid[0].size());
  if (on_grid_x && on_grid_y)
    return grid[p.x][p.y] == State::kEmpty;
  return false;
}

void ExpandNeighbors(Node n, vector<Node> &open, vector<vector<State>> &board, Point goal){
  int x = n.location.x;
  int y = n.location.y;
  int g = n.g;
  int h = n.h;
  const int delta[4][2]{{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
  for (size_t i = 0; i < 4; i++)
  {
    Point pn;
    pn.x = x + delta[i][0];
    pn.y = y + delta[i][1];
    bool valid = CheckValidCell(pn, board);
    if(valid){
      Node Nn;
      Nn.location = pn;
      Nn.g = g + 1;
      Nn.h = Heuristic(pn, goal);
      AddToOpen(Nn, open, board);
    }
  }
  
}

vector<vector<State>> Search(vector<vector<State>> board, Point start,
                             Point goal) {
  vector<Node> open;
  Node init;
  init.location = start;
  init.g = 0;
  init.h = Heuristic(start, goal);
  AddToOpen(init, open, board);

  while (!open.empty()) {
    CellSort(&open);
    auto current = open.back();
    open.pop_back();
    int x = current.location.x;
    int y = current.location.y;
    board[x][y] = State::kPath;
    if (x == goal.x && y == goal.y) {
      board[start.x][start.y] = State::kStart;
      board[goal.x][goal.y] = State::kFinish;
      return board;
    }else{
      ExpandNeighbors(current, open, board, goal);
    }
  }

  cout << "No Path Found ! \n";
  return vector<vector<State>>();
}

int main() {

  vector<vector<State>> board = ReadBoardFile("files/1.board");
  
  Point init{0, 0};
  Point goal{4, 5};
  auto solution = Search(board, init, goal);
  PrintBoard(solution);
  return 0;
}