// http://olympiads.win.tue.nl/ioi/ioi94/contest/day1prb1/problem.html

#include <iostream>
#include <vector>

using namespace std;

// indices:
//     0     row 0
//    1 2    row 1
//   3 4 5   row 2
//  6 7 8 9  row 3

// example input: (n = 5)
//     7
//    3 8
//   8 1 0 
//  2 7 4 4
// 4 5 2 6 5

void addSums(int n, vector<int> & data) {
  int index = 1;
  for (int row = 1; row < n; ++row) {
    int rowStart = index;
    int prevRowStart = rowStart - row;
    
    for (int col = 0; col < row + 1; ++col) {
      int bestParent = 0;
      if (col > 0) {
	// there is a left parent
	bestParent = data[prevRowStart + col - 1];
      }
      if (col < row) {
	// there is a right parent
	bestParent = max(bestParent, data[prevRowStart + col]);
      }
      data[index] += bestParent;
      ++index;
    }
  }
}

void print(int n, const vector<int> & data) {
  int index = 0;
  for (int row = 0; row < n; ++row) {
    for (int col = 0; col < row + 1; ++col) {
      cout << data[ index++ ] << " ";
    }
    cout << endl;
  }
}

int main() {
    int n;
    cin >> n;
    vector<int> data;
    for (int i = 0; i < n; ++i) {
        for (int j=0; j < i + 1; ++j) {
	  int value;
	  cin >> value;
	  data.push_back(value);
        }
    }
    cout << "input:" << endl;
    print(n, data);
    addSums(n, data);
    //cout << "tmp: " << endl;
    //print(n, data);
    int result = 0;
    for (size_t i = (n*(n-1))/2; i < data.size(); ++i) {
      result = max(result, data[i]);
    }
    cout << "result: " << result << endl;
}
