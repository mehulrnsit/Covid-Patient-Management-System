#include <iostream>
#include <fstream>
#include <conio.h>
#include <stdio.h>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
#include <bits/stdc++.h>
#define filename (char *)"secondary_memory.txt"
#define indexfilename (char *)"indexfile.txt"
using namespace std;
fstream ifile, indexfile;

class indexnode
{
public:
    char aadhar_no[12 + 1], address_no[6];
    long long aadhar, address;
    indexnode(char *aadharno, char *addressno)
    {
        strcpy(aadhar_no, aadharno);
        strcpy(address_no, addressno);
        //cout<<aadhar_no<<" in constructor "<<address_no<<endl;
        aadhar = stoll(aadhar_no);
        address = stoll(address_no);
    }
    indexnode()
    {
    }
};
// A BTree node
class BTreeNode
{
    indexnode **keys; // An array of keys
    int t;            // Minimum degree (defines the range for number of keys)
    BTreeNode **C;    // An array of child pointers
    int n;            // Current number of keys
    bool leaf;        // Is true when node is leaf. Otherwise false

public:
    BTreeNode(int _t, bool _leaf); // Constructor

    // A function to traverse all nodes in a subtree rooted with this node
    vector<indexnode *> traverse();

    // A function to search a key in subtree rooted with this node.
    indexnode *search(long long k); // returns NULL if k is not present.

    // A function that returns the index of the first key that is greater
    // or equal to k
    int findKey(indexnode *k);

    // A utility function to insert a new key in the subtree rooted with
    // this node. The assumption is, the node must be non-full when this
    // function is called
    void insertNonFull(indexnode *k);

    // A utility function to split the child y of this node. i is index
    // of y in child array C[]. The Child y must be full when this
    // function is called
    void splitChild(int i, BTreeNode *y);

    // A wrapper function to remove the key k in subtree rooted with
    // this node.
    void remove(indexnode *k);

    // A function to remove the key present in idx-th position in
    // this node which is a leaf
    void removeFromLeaf(int idx);

    // A function to remove the key present in idx-th position in
    // this node which is a non-leaf node
    void removeFromNonLeaf(int idx);

    // A function to get the predecessor of the key- where the key
    // is present in the idx-th position in the node
    indexnode *getPred(int idx);

    // A function to get the successor of the key- where the key
    // is present in the idx-th position in the node
    indexnode *getSucc(int idx);

    // A function to fill up the child node present in the idx-th
    // position in the C[] array if that child has less than t-1 keys
    void fill(int idx);

    // A function to borrow a key from the C[idx-1]-th node and place
    // it in C[idx]th node
    void borrowFromPrev(int idx);

    // A function to borrow a key from the C[idx+1]-th node and place it
    // in C[idx]th node
    void borrowFromNext(int idx);

    // A function to merge idx-th child of the node with (idx+1)th child of
    // the node
    void merge(int idx);
    void shownode()
    {
        queue<BTreeNode *> q,temp;
        // cout<<keys[0]->address<<" ";
        // cout<<keys[1]->address<<"\n";
        // cout<<C[0]->keys[0]->address<<" ";
        // cout<<C[0]->keys[1]->address<<",  ";
        // cout<<C[1]->keys[0]->address<<" ";
        // cout<<C[1]->keys[1]->address<<",  ";
        // cout<<C[2]->keys[0]->address<<" ";
        // cout<<C[2]->keys[1]->address<<"\n";
        // cout<<C[0]->n;
        // cout<<C[1]->n;
        // cout<<C[2]->n;
        q.push(this);
        cout<<"\t\t\t";
        while(!q.empty())
        {
            BTreeNode *ele=q.front();
            q.pop();
            cout<<"[";
            for(int i=0;i<ele->n;i++)
            {
                cout<<ele->keys[i]->aadhar;
                if(i!=ele->n-1)
                    cout<<", ";
                if(ele->leaf==false)
                temp.push(ele->C[i]);
            }
            cout<<"] ";
            if(ele->leaf==false)
                temp.push(ele->C[ele->n]);
            if(q.empty())
            {
                while(!temp.empty())
                {
                    q.push(temp.front());
                    temp.pop();
                }
                cout<<endl<<endl<<endl;
                cout<<"\t\t\t";
            }
            else{
                cout<<"---- ";
            }
        }
    }
    // Make BTree friend of this so that we can access private members of
    // this class in BTree functions
    friend class BTree;
};

class BTree
{
    BTreeNode *root; // Pointer to root node
    int t;           // Minimum degree
public:
    // Constructor (Initializes tree as empty)
    BTree(int _t)
    {
        root = NULL;
        t = _t;
    }
    vector<indexnode *> traverse()
    {
        vector<indexnode *> temp;
        if (root != NULL)
            return root->traverse();
        return temp;
    }

    // function to search a key in this tree
    indexnode *search(long long k)
    {
        // cout<<k<<" root "<<root<<endl;
        return (root == NULL) ? NULL : root->search(k);
    }

    // The main function that inserts a new key in this B-Tree
    void insert(indexnode *k);

    // The main function that removes a new key in thie B-Tree
    void remove(indexnode *k);

    void showtree(){
        if(root!=NULL)
            root->shownode();
    }
} tree(3);

BTreeNode::BTreeNode(int t1, bool leaf1)
{
    // Copy the given minimum degree and leaf property
    t = t1;
    leaf = leaf1;

    // Allocate memory for maximum number of possible keys
    // and child pointers
    keys = new indexnode *[2 * t - 1];
    C = new BTreeNode *[2 * t];

    // Initialize the number of keys as 0
    n = 0;
}

// A utility function that returns the index of the first key that is
// greater than or equal to k
int BTreeNode::findKey(indexnode *k)
{
    int idx = 0;
    while (idx < n && keys[idx]->aadhar < k->aadhar)
        ++idx;
    return idx;
}

// A function to remove the key k from the sub-tree rooted with this node
void BTreeNode::remove(indexnode *k)
{
    // cout<<k->aadhar<<" removing "<<k->address<<endl;

    int idx = findKey(k);

    // The key to be removed is present in this node
    if (idx < n && keys[idx]->aadhar == k->aadhar)
    {

        // If the node is a leaf node - removeFromLeaf is called
        // Otherwise, removeFromNonLeaf function is called
        if (leaf)
            removeFromLeaf(idx);
        else
            removeFromNonLeaf(idx);
    }
    else
    {

        // If this node is a leaf node, then the key is not present in tree
        if (leaf)
        {
            // cout << "The key "<< k->aadhar <<" is does not exist in the tree\n";
            return;
        }

        // The key to be removed is present in the sub-tree rooted with this node
        // The flag indicates whether the key is present in the sub-tree rooted
        // with the last child of this node
        bool flag = ((idx == n) ? true : false);

        // If the child where the key is supposed to exist has less that t keys,
        // we fill that child
        if (C[idx]->n < t)
            fill(idx);

        // If the last child has been merged, it must have merged with the previous
        // child and so we recurse on the (idx-1)th child. Else, we recurse on the
        // (idx)th child which now has atleast t keys
        if (flag && idx > n)
            C[idx - 1]->remove(k);
        else
            C[idx]->remove(k);
    }
    return;
}

// A function to remove the idx-th key from this node - which is a leaf node
void BTreeNode::removeFromLeaf(int idx)
{

    // Move all the keys after the idx-th pos one place backward
    for (int i = idx + 1; i < n; ++i)
        keys[i - 1] = keys[i];

    // Reduce the count of keys
    n--;

    return;
}

// A function to remove the idx-th key from this node - which is a non-leaf node
void BTreeNode::removeFromNonLeaf(int idx)
{

    indexnode *k = keys[idx];

    // If the child that precedes k (C[idx]) has atleast t keys,
    // find the predecessor 'pred' of k in the subtree rooted at
    // C[idx]. Replace k by pred. Recursively delete pred
    // in C[idx]
    if (C[idx]->n >= t)
    {
        indexnode *pred = getPred(idx);
        keys[idx] = pred;
        C[idx]->remove(pred);
    }

    // If the child C[idx] has less that t keys, examine C[idx+1].
    // If C[idx+1] has atleast t keys, find the successor 'succ' of k in
    // the subtree rooted at C[idx+1]
    // Replace k by succ
    // Recursively delete succ in C[idx+1]
    else if (C[idx + 1]->n >= t)
    {
        indexnode *succ = getSucc(idx);
        keys[idx] = succ;
        C[idx + 1]->remove(succ);
    }

    // If both C[idx] and C[idx+1] has less that t keys,merge k and all of C[idx+1]
    // into C[idx]
    // Now C[idx] contains 2t-1 keys
    // Free C[idx+1] and recursively delete k from C[idx]
    else
    {
        merge(idx);
        C[idx]->remove(k);
    }
    return;
}

// A function to get predecessor of keys[idx]
indexnode *BTreeNode::getPred(int idx)
{
    // Keep moving to the right most node until we reach a leaf
    BTreeNode *cur = C[idx];
    while (!cur->leaf)
        cur = cur->C[cur->n];

    // Return the last key of the leaf
    return cur->keys[cur->n - 1];
}

indexnode *BTreeNode::getSucc(int idx)
{

    // Keep moving the left most node starting from C[idx+1] until we reach a leaf
    BTreeNode *cur = C[idx + 1];
    while (!cur->leaf)
        cur = cur->C[0];

    // Return the first key of the leaf
    return cur->keys[0];
}

// A function to fill child C[idx] which has less than t-1 keys
void BTreeNode::fill(int idx)
{

    // If the previous child(C[idx-1]) has more than t-1 keys, borrow a key
    // from that child
    if (idx != 0 && C[idx - 1]->n >= t)
        borrowFromPrev(idx);

    // If the next child(C[idx+1]) has more than t-1 keys, borrow a key
    // from that child
    else if (idx != n && C[idx + 1]->n >= t)
        borrowFromNext(idx);

    // Merge C[idx] with its sibling
    // If C[idx] is the last child, merge it with with its previous sibling
    // Otherwise merge it with its next sibling
    else
    {
        if (idx != n)
            merge(idx);
        else
            merge(idx - 1);
    }
    return;
}

// A function to borrow a key from C[idx-1] and insert it
// into C[idx]
void BTreeNode::borrowFromPrev(int idx)
{

    BTreeNode *child = C[idx];
    BTreeNode *sibling = C[idx - 1];

    // The last key from C[idx-1] goes up to the parent and key[idx-1]
    // from parent is inserted as the first key in C[idx]. Thus, the loses
    // sibling one key and child gains one key

    // Moving all key in C[idx] one step ahead
    for (int i = child->n - 1; i >= 0; --i)
        child->keys[i + 1] = child->keys[i];

    // If C[idx] is not a leaf, move all its child pointers one step ahead
    if (!child->leaf)
    {
        for (int i = child->n; i >= 0; --i)
            child->C[i + 1] = child->C[i];
    }

    // Setting child's first key equal to keys[idx-1] from the current node
    child->keys[0] = keys[idx - 1];

    // Moving sibling's last child as C[idx]'s first child
    if (!child->leaf)
        child->C[0] = sibling->C[sibling->n];

    // Moving the key from the sibling to the parent
    // This reduces the number of keys in the sibling
    keys[idx - 1] = sibling->keys[sibling->n - 1];

    child->n += 1;
    sibling->n -= 1;

    return;
}

// A function to borrow a key from the C[idx+1] and place
// it in C[idx]
void BTreeNode::borrowFromNext(int idx)
{

    BTreeNode *child = C[idx];
    BTreeNode *sibling = C[idx + 1];

    // keys[idx] is inserted as the last key in C[idx]
    child->keys[(child->n)] = keys[idx];

    // Sibling's first child is inserted as the last child
    // into C[idx]
    if (!(child->leaf))
        child->C[(child->n) + 1] = sibling->C[0];

    //The first key from sibling is inserted into keys[idx]
    keys[idx] = sibling->keys[0];

    // Moving all keys in sibling one step behind
    for (int i = 1; i < sibling->n; ++i)
        sibling->keys[i - 1] = sibling->keys[i];

    // Moving the child pointers one step behind
    if (!sibling->leaf)
    {
        for (int i = 1; i <= sibling->n; ++i)
            sibling->C[i - 1] = sibling->C[i];
    }

    // Increasing and decreasing the key count of C[idx] and C[idx+1]
    // respectively
    child->n += 1;
    sibling->n -= 1;

    return;
}

// A function to merge C[idx] with C[idx+1]
// C[idx+1] is freed after merging
void BTreeNode::merge(int idx)
{
    BTreeNode *child = C[idx];
    BTreeNode *sibling = C[idx + 1];

    // Pulling a key from the current node and inserting it into (t-1)th
    // position of C[idx]
    child->keys[t - 1] = keys[idx];

    // Copying the keys from C[idx+1] to C[idx] at the end
    for (int i = 0; i < sibling->n; ++i)
        child->keys[i + t] = sibling->keys[i];

    // Copying the child pointers from C[idx+1] to C[idx]
    if (!child->leaf)
    {
        for (int i = 0; i <= sibling->n; ++i)
            child->C[i + t] = sibling->C[i];
    }

    // Moving all keys after idx in the current node one step before -
    // to fill the gap created by moving keys[idx] to C[idx]
    for (int i = idx + 1; i < n; ++i)
        keys[i - 1] = keys[i];

    // Moving the child pointers after (idx+1) in the current node one
    // step before
    for (int i = idx + 2; i <= n; ++i)
        C[i - 1] = C[i];

    // Updating the key count of child and the current node
    child->n += sibling->n + 1;
    n--;

    // Freeing the memory occupied by sibling
    delete (sibling);
    return;
}

// The main function that inserts a new key in this B-Tree
void BTree::insert(indexnode *k)
{
    // If tree is empty
    if (root == NULL)
    {
        // Allocate memory for root
        root = new BTreeNode(t, true);
        // cout<<"inserting1";
        root->keys[0] = k; // Insert key
        // cout<<"inserting2";
        root->n = 1; // Update number of keys in root
                     // cout<<"inserting3";
    }
    else // If tree is not empty
    {
        // If root is full, then tree grows in height
        if (root->n == 2 * t - 1)
        {
            // Allocate memory for new root
            BTreeNode *s = new BTreeNode(t, false);

            // Make old root as child of new root
            s->C[0] = root;

            // Split the old root and move 1 key to the new root
            s->splitChild(0, root);

            // New root has two children now. Decide which of the
            // two children is going to have new key
            int i = 0;
            if (s->keys[0]->aadhar < k->aadhar)
                i++;
            s->C[i]->insertNonFull(k);

            // Change root
            root = s;
        }
        else // If root is not full, call insertNonFull for root
            root->insertNonFull(k);
    }
}

// A utility function to insert a new key in this node
// The assumption is, the node must be non-full when this
// function is called
void BTreeNode::insertNonFull(indexnode *k)
{
    // Initialize index as index of rightmost element
    int i = n - 1;

    // If this is a leaf node
    if (leaf == true)
    {
        // The following loop does two things
        // a) Finds the location of new key to be inserted
        // b) Moves all greater keys to one place ahead
        while (i >= 0 && keys[i]->aadhar > k->aadhar)
        {
            keys[i + 1] = keys[i];
            i--;
        }

        // Insert the new key at found location
        keys[i + 1] = k;
        n = n + 1;
    }
    else // If this node is not leaf
    {
        // Find the child which is going to have the new key
        while (i >= 0 && keys[i]->aadhar > k->aadhar)
            i--;

        // See if the found child is full
        if (C[i + 1]->n == 2 * t - 1)
        {
            // If the child is full, then split it
            splitChild(i + 1, C[i + 1]);

            // After split, the middle key of C[i] goes up and
            // C[i] is splitted into two. See which of the two
            // is going to have the new key
            if (keys[i + 1]->aadhar < k->aadhar)
                i++;
        }
        C[i + 1]->insertNonFull(k);
    }
}

// A utility function to split the child y of this node
// Note that y must be full when this function is called
void BTreeNode::splitChild(int i, BTreeNode *y)
{
    // Create a new node which is going to store (t-1) keys
    // of y
    BTreeNode *z = new BTreeNode(y->t, y->leaf);
    z->n = t - 1;

    // Copy the last (t-1) keys of y to z
    for (int j = 0; j < t - 1; j++)
        z->keys[j] = y->keys[j + t];

    // Copy the last t children of y to z
    if (y->leaf == false)
    {
        for (int j = 0; j < t; j++)
            z->C[j] = y->C[j + t];
    }

    // Reduce the number of keys in y
    y->n = t - 1;

    // Since this node is going to have a new child,
    // create space of new child
    for (int j = n; j >= i + 1; j--)
        C[j + 1] = C[j];

    // Link the new child to this node
    C[i + 1] = z;

    // A key of y will move to this node. Find location of
    // new key and move all greater keys one space ahead
    for (int j = n - 1; j >= i; j--)
        keys[j + 1] = keys[j];

    // Copy the middle key of y to this node
    keys[i] = y->keys[t - 1];

    // Increment count of keys in this node
    n = n + 1;
}

// Function to traverse all nodes in a subtree rooted with this node
vector<indexnode *> BTreeNode::traverse()
{
    // There are n keys and n+1 children, travers through n keys
    // and first n children
    int i;
    vector<indexnode *> ans;
    for (i = 0; i < n; i++)
    {
        // If this is not leaf, then before printing key[i],
        // traverse the subtree rooted with child C[i].
        vector<indexnode *> temp;
        if (leaf == false)
            temp = (C[i]->traverse());
        for (indexnode *j : temp)
            ans.push_back(j);
        ans.push_back(keys[i]);
        //cout << keys[i]->aadhar<<"|"<<keys[i]->address<<"|#";
    }

    vector<indexnode *> temp;
    // Print the subtree rooted with last child
    if (leaf == false)
        temp = C[i]->traverse();

    for (indexnode *j : temp)
        ans.push_back(j);
    return ans;
}

// Function to search key k in subtree rooted with this node
indexnode *BTreeNode::search(long long k)
{
    // cout<<k<<" search "<<k<<endl;
    // Find the first key greater than or equal to k
    int i = 0;
    while (i < n && k > keys[i]->aadhar)
        i++;

    // If the found key is equal to k, return this node
    if (i<n&&keys[i]->aadhar == k)
        return keys[i];

    // If key is not found here and this is a leaf node
    if (leaf == true)
        return NULL;

    // Go to the appropriate child
    return C[i]->search(k);
}

void BTree::remove(indexnode *k)
{
    if (!root)
    {
        // cout << "The tree is empty\n";
        return;
    }
    if (!k)
    {
        // cout<<"key not there\n";
        return;
    }
    // Call the remove function for root
    root->remove(k);

    // If the root node has 0 keys, make its first child as the new root
    // if it has a child, otherwise set root as NULL
    if (root->n == 0)
    {
        BTreeNode *tmp = root;
        if (root->leaf)
            root = NULL;
        else
            root = root->C[0];

        // Free the old root
        delete tmp;
    }
    return;
}

class Patient
{
    char aadhar_no[12 + 1], name[25 + 1], age[3 + 1], gender[1 + 1], phone_no[10 + 1], city[31], dop[8 + 1], result[1 + 1], don[8 + 1];

public:
    void opener(fstream &ifile, char *fn, ios_base::openmode mode, fstream &indexfile, char *jn);
    void read();
    void pack();
    void display(bool checkpositive);
    void unpack();
    indexnode *search();
    void modify(indexnode *);
};
// function to open a file
void Patient::opener(fstream &sfile, char *fn, ios_base::openmode mode, fstream &jfile, char *jn)
{
    sfile.open(fn, mode);
    jfile.open(jn, mode);
    // if(!sfile)
    // cout<<"Secondary file ";
    // if(!jfile)
    // cout<<"index file ";
    if (!sfile || !jfile)
    {
        cout << "unable to open a file" << endl;
        getch();
        exit(1);
    }
}
int validate_adhar(char *adharNo)
{
    if (strlen(adharNo) != 12)
        return 0;
    int i = 0;
    for (i = 0; adharNo[i] != '\0'; i++)
    {
        if (isalpha(adharNo[i]))
            return 0;
    }
    return 1;
}
//function to read the Patient record
void Patient::read()
{

    cin.ignore();
    getAdhar:
    cout << setiosflags(ios::left);
    cout << "\t\t\t";
    cout << setw(70) << "Enter aadhar number:";
    gets(aadhar_no);
    if (validate_adhar(aadhar_no) == 0)
    {
        cout << "\n\n\t\t\tAdhar Number should have 12 digits and no characters \n ";
        cin.ignore();
        goto getAdhar;
    }
    indexnode *k = tree.search(stoll(aadhar_no));
    if (k)
    {
        cout << "\n\n\t\t\tThis aadhar already exists. U can choose option 5 modify. To edit this aadhar number info.\n\n";
        cout << "\n\n\t\t\tHit Enter to exit";
        cin.ignore();
        return;
    }
    cout << "\n\n";
    cout << "\t\t\t";
    cout << setw(70) << "Enter name: ";
    gets(name);
    cout << "\n\n";
    cout << "\t\t\t";
    cout << setw(70) << "Enter age: ";
    gets(age);
    cout << "\n\n";
    cout << "\t\t\t";
    cout << setw(70) << "Enter gender(M/F/O): ";
    gets(gender);
    cout << "\n\n";
    cout << "\t\t\t";
    cout << setw(70) << "Enter mobile no.(No ext. required): ";
    gets(phone_no);
    cout << "\n\n";
    cout << "\t\t\t";
    cout << setw(70) << "Enter city: ";
    gets(city);
    cout << "\n\n";
    cout << "\t\t\t";
    cout << setw(70) << "Enter Date of positive(dd/mm/yy): ";
    gets(dop);
    cout << "\n\n";
    cout << "\t\t\t";
    cout << setw(70) << "Enter Result(Positive=P and Negative=N): ";
    gets(result);
    cout << "\n\n";
    cout << "\t\t\t";
    cout << setw(70) << "Enter Date of negative(dd/mm/yy): ";
    gets(don);
    int count = 0;
    count = 0;
    while (count < 6)
    {
        cout << endl;

        count++;
    }
    pack();
}
// function to pack the Patient record using delimiter
void Patient::pack()
{
    ifile.seekg(0, ios::end);
    int currentpointer = ifile.tellg();
    char copy[13], copy2[6];
    strcpy(copy, (const char *)aadhar_no);
    strcpy(copy2, to_string(currentpointer).c_str());
    tree.insert(new indexnode(copy, copy2));
    tree.traverse();
    char buffer[200];
    strcpy(buffer, aadhar_no);
    strcat(buffer, "|");
    strcat(buffer, name);
    strcat(buffer, "|");
    strcat(buffer, age);
    strcat(buffer, "|");
    strcat(buffer, gender);
    strcat(buffer, "|");
    strcat(buffer, phone_no);
    strcat(buffer, "|");
    strcat(buffer, city);
    strcat(buffer, "|");
    strcat(buffer, dop);
    strcat(buffer, "|");
    strcat(buffer, result);
    strcat(buffer, "|");
    strcat(buffer, don);
    strcat(buffer, "|");
    ifile << buffer << "#";
}
//function to display Patient record
void Patient::display(bool checkpositive)
{

    char buffer[200];
    cout << setiosflags(ios::left);
    cout <<setw(10)<<"Address"<< setw(15) << "Aadhar No." << setw(30) << "Name";
    cout << setw(5) << "Age" << setw(10) << "Gender";
    cout << setw(15) << "Phone no." << setw(35) << "City" << setw(20) << "Date of Positive";
    cout << setw(10) << "Result" << setw(20) << "Date of Negative" << endl
         << endl;

    vector<indexnode *> indexes = tree.traverse();
    for (indexnode *i : indexes)
    {
        ifile.seekg(i->address);
        unpack();
        if (!checkpositive || (checkpositive && !strcmp(result, "P")))
        {
            cout <<setw(10)<<i->address;
            cout << setw(15) << aadhar_no << setw(30) << name;
            cout << setw(5) << age << setw(10) << gender;
            cout << setw(15) << phone_no << setw(35) << city << setw(20) << dop;
            cout << setw(10) << result << setw(20) << don << endl;
        }
    }

    int count = 0;
    while (count < 3)
    {
        cout << endl;
        count++;
    }
    // while (1)
    // {
    //     unpack();
    //     if (ifile.eof())
    //         break;
    //     if (aadhar_no[0] != '$')
    //     {
    // cout << setw(15)<<aadhar_no<< setw(30)<<name;
    // cout<< setw(5)<<age<< setw(10)<<gender;
    // cout<<setw(15)<<phone_no<< setw(35)<<city<< setw(20)<<dop;
    // cout<< setw(10)<<result<< setw(20)<<don<<endl;
    //     }
    // }
}
// function to unpack
void Patient::unpack()
{
    char dummy[200];
    ifile.getline(aadhar_no, 13, '|');
    ifile.getline(name, 26, '|');
    ifile.getline(age, 4, '|');
    ifile.getline(gender, 2, '|');
    ifile.getline(phone_no, 11, '|');
    ifile.getline(city, 31, '|');
    ifile.getline(dop, 9, '|');
    ifile.getline(result, 2, '|');
    ifile.getline(don, 9, '|');
    ifile.getline(dummy, 10, '#');
}
//function to search Patient record based on USN.

indexnode *Patient::search()
{
    int flag;
    char saadharno[13];
    cout << "\t\t\t\tEnter the aadhar no to be searched:\t\t\t";
    cin >> saadharno;
    cout << "\n\n";
    indexnode *k = tree.search(stoll(saadharno));
    if (k)
    {
        ifile.seekg(k->address);
        flag = ifile.tellg();
        unpack();
        cout << "\t\t\t\tadhar No.:" << aadhar_no << "\n\n\t\t\t\tName: " << name << "\n\n\t\t\t\tAge: " << age << "\n\n\t\t\t\tGender: " << gender << "\n\n\t\t\t\tPhone no.: " << phone_no << "\n\n\t\t\t\tCity: " << city << "\n\n\t\t\t\tDate of positive: " << dop << "\n\n\t\t\t\tResult: " << result << "\n\n\t\t\t\tDate of Negative: " << don << endl;
        return k;
    }
    // while (!ifile.eof())
    // {
    //     flag = ifile.tellg();
    //     unpack();
    //     if (aadhar_no[0] != '$' && strcmp(aadhar_no, saadharno) == 0)
    //     {
    //         cout << "Aadhar No.:" << aadhar_no << "\nName: "<<name<< "\nAge: "<<age<< "\nGender: "<<gender<< "\nPhone no.: "<<phone_no<< "\nCity: "<<city<< "\nDate of positive: "<<dop<< "\nResult: "<<result<< "\nDate of Negative: "<<don<<endl;
    //         return flag;
    //     }
    // }
    return NULL;
}
//function to modify record.
void Patient::modify(indexnode *recpos)
{
    ifile.seekp(recpos->address, ios::beg);
    ifile.put('$');
    tree.remove(recpos);
    ifile.seekp(0, ios::end);
    read();
}
//main program
void pack(fstream &sfile, char *fn)
{
    vector<indexnode *> indexes = tree.traverse();
    sfile.close();
    sfile.open(fn, ios::out | ios::trunc);
    for (indexnode *i : indexes)
    {
        sfile << i->aadhar_no << "|" << i->address_no << "|#";
    }
}
void unpack()
{
    char dummy[10000];
    indexfile.seekg(0, ios::end);
    int p = indexfile.tellg();
    //cout<<p<<"   pos"<<endl;
    if (p == -1)
    {
        return;
    }
    indexfile.seekg(0);
    while (true)
    {
        char a[15], b[10];
        indexfile.getline(a, 13, '|');
        indexfile.getline(b, 6, '|');
        indexfile.getline(dummy, 100, '#');
        if (indexfile.eof())
            break;
        //cout<<a<<" unpacking "<<b<<endl;
        //cout<<indexfile.tellg()<<endl;
        tree.insert(new indexnode(a, b));
    }
}

void header()
{

    cout << "\t\t########################################################################################################################" << endl;
    cout << "\t\t\t\t\t\t\t"
         << "covid patient management system" << endl;
    cout << "\t\t########################################################################################################################" << endl;
}
int main()
{

    int ch;
    indexnode *flag;
    Patient s;
    s.opener(ifile, filename, ios::in | ios::app, indexfile, indexfilename);
    unpack();
    indexfile.close();
    ifile.close();
    int count = 0;
    //tree.traverse();
    system("cls");
    do
    {
        system("cls");
        header();
        count = 0;
        while (count < 4)
        {
            cout << endl;

            count++;
        }
        cout << endl
             << "\t\t\t1. Enter a new Record\n\n\t\t\t2. Display all the Patient\n\n\t\t\t3. Display Positive\n\n\t\t\t4. Search\n\n\t\t\t5. Modify\n\n\t\t\t6. Show Tree\n\n\t\t\t0. Exit" << endl;
        cout << "\n\t\t\tEnter the choice: ";
        cin >> ch;
        count = 0;
        while (count < 8)
        {
            cout << endl;

            count++;
        }
        switch (ch)
        {
        case 1:
            s.opener(ifile, filename, ios::app, indexfile, indexfilename);
            system("cls");
            header();
            count = 0;
            while (count < 4)
            {
                cout << endl;

                count++;
            }
            cout << "\t\t\t\t\t\t\t"
                 << "Enter the patient details\n\n\n\n";
            s.read();
            break;
        case 2:
            // clrscr();
            s.opener(ifile, filename, ios::in, indexfile, indexfilename);
            header();
            // cout << "The Patient details are:" << endl;
            count = 0;
            while (count < 4)
            {
                cout << endl;

                count++;
            }
            cout << "\t\t\t\t\t\t\t"
                 << "Patient Details are: \n\n\n\n";
            s.display(false);
            char ch;
        label:
            cout << "enter value 0 to go to menu " << endl;
            cin >> ch;
            if (ch == '0')
                break;
            else
            {
                goto label;
            }
            // break;
        case 3:
            s.opener(ifile, filename, ios::in, indexfile, indexfilename);
            system("cls");
            header();
            // cout << "The Patient details are:" << endl;
            count = 0;
            while (count < 4)
            {
                cout << endl;

                count++;
            }
            cout << "\t\t\t\t\t\t\t"
                 << "Positive Patient Details are: \n\n\n\n";

            s.display(true);

        label1:
            cout << "enter value 0 to go to menu " << endl;
            cin >> ch;
            if (ch == '0')
                break;
            else
            {
                goto label1;
            }
            break;
        case 4:
            system("cls");
            header();
            s.opener(ifile, filename, ios::in, indexfile, indexfilename);
            cout << "\t\t\t\t\t\t\t"
                 << "Searching based on Aadhar number \n\n\n\n";
            flag = s.search();
            if (flag == NULL)
                cout << "\t\t\tRecord not found\n"
                     << endl;
        label2:
            cout << "\t\t\t\tenter value 0 to go to menu: ";
            cin >> ch;
            if (ch == '0')
                break;
            else
            {
                goto label2;
            }
            break;
        case 5:
            system("cls");
            header();
            s.opener(ifile, filename, ios::in | ios::out, indexfile, indexfilename);
            cout << "\t\t\t\tTo modify the record based on Aadhar no.\n"
                 << endl;
            flag = s.search();
            if (flag == NULL)
                cout << "\t\t\tRecord not found\n"
                     << endl;
            else
            {
                system("cls");
                header();
                cout << "\n\n\t\t\t\tEnter New Record Details\n\n";
                s.modify(flag);
            }
        label3:
            cout << "\t\t\tenter value 0 to go to menu: ";
            cin >> ch;
            if (ch == '0')
                break;
            else
            {
                goto label3;
            }
            break;
        case 6:
            cout<<"\t\t\t\tShowing Tree\n";
            tree.showtree();
        label4:
            cout << "\t\t\tenter value 0 to go to menu: ";
            cin >> ch;
            if (ch == '0')
                break;
            else
            {
                goto label4;
            }
            break;
            break;
        }
        ifile.close();
        pack(indexfile, indexfilename);
        indexfile.close();
    } while (ch >= 1 && ch <= 6);
    return 0;
}