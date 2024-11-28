// AVL_Database.cpp
// Mateo Saettone Ascenzo
// U56111212
#include "AVL_Database.hpp"
#include <algorithm>
#include <cmath>

// record constructor
Record::Record(const std::string& k, int v) : key(k), value(v) {}

// avl node constructor
AVLNode::AVLNode(Record* r) : record(r), left(nullptr), right(nullptr), height(1) {}

// avl tree constructor
AVLTree::AVLTree() : root(nullptr), nodeCount(0), searchComparisonCount(0) {}

// function to get the height of a node
int AVLTree::height(AVLNode* node) {
    if (node == nullptr) {
        // if node is null, height is 0
        return 0;
    } else {
        // else return the node's height
        return node->height;
    }
}

// function to update the height of a node
void AVLTree::updateHeight(AVLNode* node) {
    if (node != nullptr) {
        // get heights of left and right subtrees
        int leftHeight = height(node->left);
        int rightHeight = height(node->right);
        // set node's height to max of left and right heights plus 1
        node->height = 1 + std::max(leftHeight, rightHeight);
    }
}

// function to get the balance factor of a node
int AVLTree::getBalance(AVLNode* node) {
    if (node == nullptr) {
        // if node is null, balance is 0
        return 0;
    } else {
        // balance is height difference between left and right subtrees
        return height(node->left) - height(node->right);
    }
}

// function to perform right rotation
AVLNode* AVLTree::rotateRight(AVLNode* y) {
    // y is the unbalanced node
    AVLNode* x = y->left;      // x is y's left child
    AVLNode* T2 = x->right;    // T2 is x's right subtree
    
    // perform rotation
    x->right = y;
    y->left = T2;
    
    // update heights
    updateHeight(y);
    updateHeight(x);
    
    // return new root after rotation
    return x;
}

// function to perform left rotation
AVLNode* AVLTree::rotateLeft(AVLNode* x) {
    // x is the unbalanced node
    AVLNode* y = x->right;     // y is x's right child
    AVLNode* T2 = y->left;     // T2 is y's left subtree
    
    // perform rotation
    y->left = x;
    x->right = T2;
    
    // update heights
    updateHeight(x);
    updateHeight(y);
    
    // return new root after rotation
    return y;
}

// function to insert a record into the avl tree
void AVLTree::insert(Record* record) {
    // create a new node with the record
    AVLNode* newNode = new AVLNode(record);
    
    if (root == nullptr) {
        // if tree is empty, set root to new node
        root = newNode;
        nodeCount++;
        return;
    }
    
    AVLNode* current = root;
    AVLNode* parent = nullptr;
    std::vector<AVLNode*> path; // to keep track of the path
    
    // traverse the tree to find the correct spot
    while (current != nullptr) {
        parent = current;
        path.push_back(current); // add current node to path
        
        if (record->value < current->record->value) {
            // go left if record value is less
            current = current->left;
        } else if (record->value > current->record->value) {
            // go right if record value is greater
            current = current->right;
        } else {
            // duplicate value, do not insert
            delete newNode;
            return;
        }
    }
    
    // insert the new node
    if (record->value < parent->record->value) {
        parent->left = newNode;
    } else {
        parent->right = newNode;
    }
    nodeCount++;
    
    // backtrack and update heights and balance
    for (int i = path.size() - 1; i >= 0; --i) {
        updateHeight(path[i]); // update height of the node
        int balance = getBalance(path[i]); // get balance factor
        
        // check for imbalance and perform rotations
        if (balance > 1) {
            if (record->value < path[i]->left->record->value) {
                // left left case
                if (i == 0) {
                    root = rotateRight(path[i]); // rotate right at root
                } else {
                    // rotate right at current node
                    if (path[i - 1]->left == path[i]) {
                        path[i - 1]->left = rotateRight(path[i]);
                    } else {
                        path[i - 1]->right = rotateRight(path[i]);
                    }
                }
            } else {
                // left right case
                path[i]->left = rotateLeft(path[i]->left); // rotate left at left child
                if (i == 0) {
                    root = rotateRight(path[i]); // rotate right at root
                } else {
                    // rotate right at current node
                    if (path[i - 1]->left == path[i]) {
                        path[i - 1]->left = rotateRight(path[i]);
                    } else {
                        path[i - 1]->right = rotateRight(path[i]);
                    }
                }
            }
        } else if (balance < -1) {
            if (record->value > path[i]->right->record->value) {
                // right right case
                if (i == 0) {
                    root = rotateLeft(path[i]); // rotate left at root
                } else {
                    // rotate left at current node
                    if (path[i - 1]->left == path[i]) {
                        path[i - 1]->left = rotateLeft(path[i]);
                    } else {
                        path[i - 1]->right = rotateLeft(path[i]);
                    }
                }
            } else {
                // right left case
                path[i]->right = rotateRight(path[i]->right); // rotate right at right child
                if (i == 0) {
                    root = rotateLeft(path[i]); // rotate left at root
                } else {
                    // rotate left at current node
                    if (path[i - 1]->left == path[i]) {
                        path[i - 1]->left = rotateLeft(path[i]);
                    } else {
                        path[i - 1]->right = rotateLeft(path[i]);
                    }
                }
            }
        }
    }
}

// function to delete a node from the avl tree
void AVLTree::deleteNode(const std::string& key, int value) {
    AVLNode* current = root;
    AVLNode* parent = nullptr;
    std::vector<AVLNode*> path; // to keep track of the path
    
    // find the node to delete
    while (current != nullptr && (current->record->value != value || current->record->key != key)) {
        parent = current;
        path.push_back(current);
        
        if (value < current->record->value) {
            // go left if value is less
            current = current->left;
        } else if (value > current->record->value) {
            // go right if value is greater
            current = current->right;
        }
    }
    
    if (current == nullptr) {
        // node not found, nothing to delete
        return;
    }
    
    nodeCount--;
    
    // case 1: node has two children
    if (current->left != nullptr && current->right != nullptr) {
        // find the inorder successor
        AVLNode* successorParent = current;
        AVLNode* successor = current->right;
        path.push_back(current);
        
        while (successor->left != nullptr) {
            successorParent = successor;
            successor = successor->left;
            path.push_back(successorParent);
        }
        
        // copy successor's data to current node
        current->record = successor->record;
        
        // delete successor node
        if (successorParent->left == successor) {
            successorParent->left = successor->right;
        } else {
            successorParent->right = successor->right;
        }
        delete successor;
        
    } else {
        // case 2: node has one child or no child
        AVLNode* child = (current->left != nullptr) ? current->left : current->right;
        
        if (current == root) {
            // if current is root, set root to child
            root = child;
        } else {
            // set parent's child to current's child
            if (parent->left == current) {
                parent->left = child;
            } else {
                parent->right = child;
            }
        }
        delete current;
    }
    
    // backtrack and update heights and balance
    for (int i = path.size() - 1; i >= 0; --i) {
        updateHeight(path[i]); // update height
        int balance = getBalance(path[i]); // get balance factor
        
        // check for imbalance and perform rotations
        if (balance > 1) {
            if (getBalance(path[i]->left) >= 0) {
                // left left case
                if (i == 0) {
                    root = rotateRight(path[i]); // rotate right at root
                } else {
                    // rotate right at current node
                    if (path[i - 1]->left == path[i]) {
                        path[i - 1]->left = rotateRight(path[i]);
                    } else {
                        path[i - 1]->right = rotateRight(path[i]);
                    }
                }
            } else {
                // left right case
                path[i]->left = rotateLeft(path[i]->left); // rotate left at left child
                if (i == 0) {
                    root = rotateRight(path[i]); // rotate right at root
                } else {
                    // rotate right at current node
                    if (path[i - 1]->left == path[i]) {
                        path[i - 1]->left = rotateRight(path[i]);
                    } else {
                        path[i - 1]->right = rotateRight(path[i]);
                    }
                }
            }
        } else if (balance < -1) {
            if (getBalance(path[i]->right) <= 0) {
                // right right case
                if (i == 0) {
                    root = rotateLeft(path[i]); // rotate left at root
                } else {
                    // rotate left at current node
                    if (path[i - 1]->left == path[i]) {
                        path[i - 1]->left = rotateLeft(path[i]);
                    } else {
                        path[i - 1]->right = rotateLeft(path[i]);
                    }
                }
            } else {
                // right left case
                path[i]->right = rotateRight(path[i]->right); // rotate right at right child
                if (i == 0) {
                    root = rotateLeft(path[i]); // rotate left at root
                } else {
                    // rotate left at current node
                    if (path[i - 1]->left == path[i]) {
                        path[i - 1]->left = rotateLeft(path[i]);
                    } else {
                        path[i - 1]->right = rotateLeft(path[i]);
                    }
                }
            }
        }
    }
}

// function to search for a record in the avl tree
Record* AVLTree::search(const std::string& key, int value) {
    searchComparisonCount = 0; // reset comparison count
    AVLNode* current = root;
    
    // traverse the tree
    while (current != nullptr) {
        searchComparisonCount++; // increment comparison count
        
        if (value == current->record->value && key == current->record->key) {
            // found the record
            return current->record;
        } else if (value < current->record->value) {
            // go left if value is less
            current = current->left;
        } else {
            // go right if value is greater
            current = current->right;
        }
    }
    
    // record not found, return empty record
    return new Record("", 0);
}

// indexed database implementation

// function to insert a record into the database
void IndexedDatabase::insert(Record* record) {
    index.insert(record);
}

// function to search for a record in the database
Record* IndexedDatabase::search(const std::string& key, int value) {
    return index.search(key, value);
}

// function to delete a record from the database
void IndexedDatabase::deleteRecord(const std::string& key, int value) {
    index.deleteNode(key, value);
}

// function to perform range query on the database
std::vector<Record*> IndexedDatabase::rangeQuery(int start, int end) {
    std::vector<Record*> result;
    std::queue<AVLNode*> q;
    
    // use a queue for breadth-first traversal
    if (index.root != nullptr) {
        q.push(index.root);
    }
    
    while (!q.empty()) {
        AVLNode* node = q.front();
        q.pop();
        
        // check if node's value is within range
        if (node->record->value >= start && node->record->value <= end) {
            result.push_back(node->record);
        }
        // add left child to queue if it exists
        if (node->left != nullptr) {
            q.push(node->left);
        }
        // add right child to queue if it exists
        if (node->right != nullptr) {
            q.push(node->right);
        }
    }
    
    return result;
}

// function to clear the database
void IndexedDatabase::clearHelper(AVLNode* node) {
    if (node != nullptr) {
        // recursively delete left and right subtrees
        clearHelper(node->left);
        clearHelper(node->right);
        // delete the record and node
        delete node->record;
        delete node;
    }
}

void IndexedDatabase::clearDatabase() {
    // clear the tree starting from root
    clearHelper(index.root);
    index.root = nullptr;
}

// function to calculate the height of the tree
int IndexedDatabase::calculateHeight(AVLNode* node) const {
    if (node == nullptr) {
        // height is 0 if node is null
        return 0;
    } else {
        // height is max of left and right subtree heights plus 1
        int leftHeight = calculateHeight(node->left);
        int rightHeight = calculateHeight(node->right);
        return 1 + std::max(leftHeight, rightHeight);
    }
}

int IndexedDatabase::getTreeHeight() const {
    // get height of the tree starting from root
    return calculateHeight(index.root);
}

// function to get the number of comparisons during last search
int IndexedDatabase::getSearchComparisons(const std::string& key, int value) {
    search(key, value);
    return index.getLastSearchComparisons();
}
