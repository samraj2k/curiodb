#include "utils/btreememory.h"
using namespace std;

BTreeNodeMem *insertRoot(int flag = 0);
bool insert(BTreeNodeMem *node, std::string key, std::string value, BTreeNodeMem*& root);
void searchExact(BTreeNodeMem* node, string key, BTreeSearchResult &searchResult);
void searchRange(BTreeNodeMem* node, string lkey, string rkey, vector<BTreeSearchResult> &searchResults);