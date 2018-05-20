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
  REQUIRE( selectLowestOneBit(1) == 1 );
  REQUIRE( selectLowestOneBit(2) == 2 );
  REQUIRE( selectLowestOneBit(3) == 1 );
  REQUIRE( selectLowestOneBit(4) == 4 );
  REQUIRE( selectLowestOneBit(5) == 1 );
  REQUIRE( selectLowestOneBit(6) == 2 );
  REQUIRE( selectLowestOneBit(7) == 1 );
  REQUIRE( selectLowestOneBit(8) == 8 );
  REQUIRE( selectLowestOneBit(16) == 16 );
  REQUIRE( selectLowestOneBit(32) == 32 );
}

// Rounds x up to the next power of 2.
constexpr int roundToPowerOfTwo(int x) {
  assert(x > 0);
  int result = 1;
  while (result < x) result <<= 1;
  return result;
}

TEST_CASE("Rounding to next power of two", "[roundToPowerOfTwo]") {
  REQUIRE( roundToPowerOfTwo(1) == 1 );
  REQUIRE( roundToPowerOfTwo(2) == 2 );
  REQUIRE( roundToPowerOfTwo(3) == 4 );
  REQUIRE( roundToPowerOfTwo(4) == 4 );
  REQUIRE( roundToPowerOfTwo(5) == 8 );
  REQUIRE( roundToPowerOfTwo(6) == 8 );
  REQUIRE( roundToPowerOfTwo(7) == 8 );
  REQUIRE( roundToPowerOfTwo(8) == 8 );
  REQUIRE( roundToPowerOfTwo(9) == 16 );
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

  // TODO optimize by not always going to root
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

// TODO port

TEST_CASE("Tests raw data after updating all fields", "[Tree1D.update]") {
  Tree1D tree = createTree({1, 2, 3, 4, 5, 6, 7, 8});
  std::vector<int> expected {1, 3, 3, 10, 5, 11, 7, 36};
  std::vector<int> actual = tree.getRawData();
  REQUIRE( actual == expected );
}

TEST_CASE("Verifies all the sums from 0 to every position", "[sumUntil]") {
  std::vector<int> data {1, 2, 3, 4, 5, 6, 7, 8};

  Tree1D tree = createTree(data);
  REQUIRE(tree.sumUntil(0) == 0);
  
  int sum = 0;
  int i = 0;
  for (auto count : data) {
    sum += count;
    i++;
    REQUIRE(tree.sumUntil(i) == sum);
  }
}

TEST_CASE("Tests all the sums from", "[test_sumBetween]") {
  std::vector<int> data {1, 2, 3, 4, 5, 6, 7, 8};
    
  Tree1D tree = createTree(data);
  REQUIRE(tree.sumUntil(0) == 0);
  for (unsigned int l = 0; l < data.size(); ++l) {
    for (unsigned int r = l; r < data.size(); ++r) {
      int sum = 0;
      for (unsigned int i = l; i <= r; ++i) {
	sum += data[i];
      }
      REQUIRE(tree.sumBetween(l,r) == sum);
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
