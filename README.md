# Bank System (C++) — Data Structures Project

A simple console **Bank System** implemented in **C++** for practicing core **Data Structures** (linked lists, stacks, queues, and a BST) in a realistic scenario.

## Project Overview
This project simulates basic bank operations (creating customers, deposits, withdrawals, transfers) while intentionally implementing the underlying logic using classic data structures instead of STL containers.

The application is menu-driven and persists its data into CSV files so that customers/queue/IDs are restored on the next run.

## Repository Structure
```
.
├── DsFinalProject.cpp              # Main program (all logic + data structures)
├── DsFinalProject.sln              # Visual Studio solution
├── DsFinalProject.vcxproj          # Visual Studio project
├── DsFinalProject.vcxproj.user     # VS user settings
├── DsFinalProject.vcxproj.filters.txt
├── README.md
├── customers.csv                   # Persisted customers data (generated at runtime)
├── queue.csv                       # Persisted waiting queue (generated at runtime)
└── used_ids.csv                    # Persisted used IDs (generated at runtime)
```

## Key Data Structures (How the Code is Organized)
All core structures are defined in **`DsFinalProject.cpp`**:

### 1) Linked List — Customers
Customers are stored in a singly linked list.

```cpp
struct Customer {
    int id;
    string name;
    string password;
    string accountType;
    double balance;
    HistNode* history;   // stack (transaction history)
    Customer* next;      // linked list next
};

Customer* head = NULL;
```

- **Why linked list?** Easy insert at the head and simple traversal/search.
- **Main operations:**
  - `addCustomer()` inserts a new `Customer` at the head.
  - `searchCustomer(int id)` linearly searches by ID.
  - `deleteCustomer()` removes a node from the list.

### 2) Stack — Transaction History
Each customer has a stack of actions (deposit/withdraw/transfer/name change).

```cpp
struct HistNode {
    string action;
    HistNode* next;
};
```

- `pushHistory(Customer* c, const string& action)` pushes a new action on top.
- `printHistory(Customer* c)` prints from newest to oldest.

**Persistence:**
- `historyToString(HistNode* h)` converts the stack to a single string separated by `|`.
- `stringToHistory(const string& s)` rebuilds the stack when loading from CSV.

### 3) Queue — Waiting Line
A basic queue represents customers waiting for service.

```cpp
struct QueueNode {
    string name;
    QueueNode* next;
};

QueueNode* qFront = NULL;
QueueNode* qRear  = NULL;
```

- `enqueue(const string& name)` adds to the rear.
- `printQueue()` prints from front to rear.

**Persistence:** stored in `queue.csv` (one name per line).

### 4) BST (Binary Search Tree) — Balance Ranking
A BST is built on-demand to rank customers by balance.

```cpp
struct TreeNode {
    string name;
    double balance;
    TreeNode* left;
    TreeNode* right;
};

TreeNode* rankRoot = NULL;
```

- `buildRankTree()` inserts each customer into the BST.
- `insertRank(TreeNode*& root, const string& name, double balance)` inserts based on balance.
  - Higher balances go to the **left** so an in-order traversal prints **high → low**.
- `showBalanceRanking()` prints the ranking.

## Features (Implemented)
From the menu in `printMenu()`:

1. **Add Customer**
2. **Deposit Money** (login required)
3. **Withdraw Money** (login required)
4. **Show All Customers** (includes history)
5. **Transfer Money** (sender login + receiver ID)
6. **Show Balance Ranking** (BST)
7. **Add Waiting Customer** (enqueue)
8. **Show Waiting Queue**
9. **Show Used IDs**
10. **Delete Customer** (login required)
11. **Update Name** (login required)
12. **View My Account** (login required)
13. **Exit** (saves all CSV)

## Data Persistence (CSV Files)
The program automatically loads and saves data:

### Load on startup
`loadAllCSV()` calls:
- `loadUsedIDsCSV()` → `used_ids.csv`
- `loadCustomersCSV()` → `customers.csv`
- `loadQueueCSV()` → `queue.csv`

### Save after changes
`saveAllCSV()` calls:
- `saveCustomersCSV()`
- `saveQueueCSV()`
- `saveUsedIDsCSV()`

## ID Generation
`generateUniqueID()` generates an ID in the format:

- **YYYYMMXXX**
  - `YYYY` = current year
  - `MM` = current month
  - `XXX` = random 3 digits

The program checks uniqueness using the in-memory array `usedIDs[100]` and also persists them in `used_ids.csv`.

## How to Build & Run

### Option A: Visual Studio (recommended for this repo)
1. Open `DsFinalProject.sln`
2. Build and run.

### Option B: g++ (MinGW/Linux/macOS)
```bash
g++ -std=c++17 -O2 -o bank DsFinalProject.cpp
./bank
```

## Notes / Limitations
- This is an educational project: data structures are implemented manually.
- IDs storage uses a fixed-size array: `usedIDs[100]`.
- Passwords are stored in plain text in `customers.csv` (not secure; acceptable for learning only).