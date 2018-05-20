#include <iostream>
#include <cassert>

using namespace std;

int size;
int* data;

void initialize(int s) {
  size = s;
  data = new int[s*s]();
}

inline int indexGrid(int x, int y) {
  return size * y + x;
}

void update(int x, int y, int a) {
  data[indexGrid(x, y)] += a;
}

int query(int l, int b, int r, int t) {
  int result = 0;
  for (int x = l; x <= r; ++x) {
    for (int y = b; y <= t; ++y) {
      auto value = data[indexGrid(x, y)];
      result += value; // TODO optimize via incremental computation of indexGrid
    }
  }
  return result;
}

int main() {
  int op, s;
  cin >> op;
  assert(op == 0);
  cin >> s;
  initialize(s);
  while (true) {
    cin >> op;
    switch (op) {
    case 1:
      {
	int x, y, a;
	cin >> x >> y >> a;
	update(x, y, a);
	break;
      }
    case 2:
      {
	int l, b, r, t;
	cin >> l >> b >> r >> t;
	cout << query(l, b, r, t) << endl << flush;
	break;
      }
    case 3:
      exit(1);
    }
  }
}
