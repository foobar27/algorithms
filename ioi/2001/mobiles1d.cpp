#include "common.h"

#include <cassert>
#include <vector>
#include <iostream>

// Select the lowest bit, e.g. 11010100 => 00000100
inline constexpr int selectLowestOneBit(int x) {
  // Proof by example:
  // Let's consider the number
  //   x = ...1000
  // If we decrement this number we get:
  //   x-1 = ...0100
  // When we xor both numbers we get:
  //   x^(x-1) =  ...1100
  // Please note that the bits which are hidden in the "..." are untouhched until now..
  // We want to filter out the lowest one-bit of x, so we need to filter by x:
  //   x & (x ^ (x - 1)) = 0001000
  return x & (x ^ (x - 1));
}

TEST_CASE("Lowest bit is selected properly", "[selectLowestOneBit]") {
  using namespace std;
  vector<vector<int>> tests {
    {1, 1}, {2, 2}, {3, 1}, {4, 4}, {5, 1}, {6, 2}, {7, 1}, {8, 8}, {16, 16}, {32, 32}
  };
  for (auto & test: tests) {
    int input = test[0];
    int expected = test[1];
    REQUIRE(selectLowestOneBit(input) == expected);
  }
}

// Rounds x up to the next power of 2.
constexpr int roundToPowerOfTwo(int x) {
  assert(x > 0);
  int result = 1;
  while (result < x) result <<= 1;
  return result;
}

TEST_CASE("Rounding to next power of two", "[roundToPowerOfTwo]") {
  using namespace std;
  vector<vector<int>> tests {
    {1, 1}, {2, 2}, {3, 4}, {4, 4}, {5, 8}, {6, 8}, {7, 8}, {8, 8}, {9, 16}
  };
  for (auto & test : tests) {
    int input = test[0];
    int expected = test[1];
    REQUIRE(roundToPowerOfTwo(input) == expected);
  }
}

class Tree1D {

public:
  Tree1D(int size)
    : m_data(roundToPowerOfTwo(size))
  {}
  
  void update(unsigned int x, int count) {
    assert(x < m_data.size());
    ++x;
    do {
      m_data[x - 1] += count;
    } while ((x += selectLowestOneBit(x)) <= m_data.size());
  }
  
  // Get the sum of all the elements until index x (included).
  int sumUntil(int x) const {
    int sum = 0;
    while (x > 0) {
      sum += m_data[x - 1];
      // Go up one level by changing the lowest one-bit to a zero-bit.
      x -= selectLowestOneBit(x);
    }
    return sum;
  }

  int sumBetween(unsigned int left, unsigned int right) const {
    assert(left < m_data.size());
    assert(right < m_data.size());
    assert(left <= right);
    int untilLeft = left == 0 ? 0 : sumUntil(left);
    int untilRight = right == 0 ? 0 : sumUntil(right);
    return untilRight - untilLeft;
  }

  std::vector<int> getRawData() const {
    return m_data;
  }
  
  void dump() const {
    using namespace std;
    for (auto count : m_data) {
      cout << count << " ";
    }
    cout << endl;
  }

private:

  std::vector<int> m_data;
  
};

Tree1D createTree(const std::vector<int> & counts) {
  Tree1D tree(counts.size());
  int i = 0;
  for (auto count : counts) {
    tree.update(i, count);
    ++i;
  }
  return tree;
}

TEST_CASE("A tree can be updated and queried", "[Tree1D]") {
  using namespace std;
  vector<int> data {1, 2, 3, 4, 5, 6, 7, 8};

  Tree1D tree = createTree(data);

  SECTION("Verify raw data after updates") {
    vector<int> expected {1, 3, 3, 10, 5, 11, 7, 36};
    REQUIRE( tree.getRawData() == expected );
  }

  SECTION("Verify sums from 0 to every position") {
    REQUIRE(tree.sumUntil(0) == 0);
    int sum = 0;
    int i = 0;
    for (auto count : data) {
      SECTION(string("i=") + to_string(i)) {
	sum += count;
	i++;
	REQUIRE(tree.sumUntil(i) == sum);
      }
    }
  }

  SECTION("Verify sums from every pair of positions") {
    for (unsigned int l = 0; l < data.size(); ++l) {
      SECTION(string("l=") + to_string(l)) {
	for (unsigned int r = l; r < data.size(); ++r) {
	  SECTION(string("r=") + to_string(r)) {
	    int sum = 0;
	    for (unsigned int i = l; i <= r; ++i) {
	      sum += data[i];
	    }
	    REQUIRE(tree.sumBetween(l,r) == sum);
	  }
	}
      }
    }
  }

}

int MAIN() {
  using namespace std;
  int op, s;
  cin >> op;
  assert(op == 0);
  cin >> s;
  Tree1D tree(s);
  while (true) {
    cin >> op;
    switch (op) {
    case 1:
      {
	int x, a;
	cin >> x >> a;
	tree.update(x, a);
	break;
      }
    case 2:
      {
	int l, r;
	cin >> l >> r;
	cout << tree.sumBetween(l, r) << endl << flush;
	break;
      }
    case 3:
      exit(0);
    }
  }
  return 0;
}
