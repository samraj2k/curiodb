#include "inmemorybtreeutils.h"
#include <chrono>
#include <iostream>
#include <random>
#include <cstddef>
using namespace std::chrono;
using namespace std;

std::vector<string> random_string() {
  vector<string> randoms;
  std::random_device rd;
  std::mt19937 g(rd());
  for (int i = 1; i < 1e6; i++) {
    string cur = to_string(i);
    randoms.push_back(cur);
  }
  shuffle(randoms.begin(), std::end(randoms), g);
  return randoms;
}

int main() {
  vector<string> randomStrings;

  BTreeNodeMem *root = insertRoot();

  vector<string> randomUniqueKeys = random_string();
  auto start = high_resolution_clock::now();

  for(auto to: randomUniqueKeys) {
    insert(root, to, to, root);
  }

  auto end = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(end - start);
  cout << "Time taken: " << duration.count() << " microseconds" << endl;
  // insert(root, "1", "1", root);
  // insert(root, "2", "2", root);
  // insert(root, "3", "33", root);
  // insert(root, "4", "444", root);
  // insert(root, "5", "555", root);
  // insert(root, "6", "666", root);
  // insert(root, "7", "77", root);
  // insert(root, "8", "64", root);
  // insert(root, "9", "46", root);
  // insert(root, "10", "3536", root);
  // insert(root, "11", "463", root);
  // insert(root, "12", "346346", root);
  // insert(root, "13", "4563", root);
  // insert(root, "14", "33", root);
  // insert(root, "15", "66", root);

  BTreeSearchResult searchResult;
  for(int i = 1; i < 16; i++) {
    searchExact(root, to_string(i), searchResult);
  }
  vector<BTreeSearchResult> searchResults;
  searchRange(root, "4", "14", searchResults);
}
