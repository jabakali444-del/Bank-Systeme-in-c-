#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <limits>
#include <thread>
#include <ctime>
using namespace std;

//for used IDs
int usedIDs[100];
int idCount = 0;

// Stack node for history
struct HistNode {
    string action;
    HistNode* next;
};

// Queue node for waiting customers
struct QueueNode {
    string name;
    QueueNode* next;
};

QueueNode* qFront = NULL;
QueueNode* qRear = NULL;

// Linked list node for customers
struct Customer {
    int id;
    string name;
    string password;
    string accountType;
    double balance;
    HistNode* history;
    Customer* next;
};

Customer* head = NULL;

// BST node for ranking by balance
struct TreeNode {
    string name;
    double balance;
    TreeNode* left;
    TreeNode* right;
};

TreeNode* rankRoot = NULL;

// Check if ID already used
bool idExists(int id) {
    for (int i = 0; i < idCount; i++) {
        if (usedIDs[i] == id) return true;
    }
    return false;
}

// Generate unique ID in format YYYYMMXXX
int generateUniqueID() {
    time_t now = time(NULL);
    tm tStruct;
    localtime_s(&tStruct, &now);
    tm* t = &tStruct;

    int year = t->tm_year + 1900;
    int month = t->tm_mon + 1;   // <-- fixed here

    int base = year * 100 + month;

    while (true) {
        int rand3 = rand() % 1000;
        int finalID = base * 1000 + rand3;
        if (!idExists(finalID)) return finalID;
    }
}


// Convert history stack to string separated by '|'
string historyToString(HistNode* h) {
    string result = "";
    HistNode* t = h;

    while (t) {
        result += t->action;
        if (t->next) result += "|";
        t = t->next;
    }
    return result;
}

// Convert string back to history stack
HistNode* stringToHistory(const string& s) {
    if (s == "") return NULL;

    string parts[200];
    int partCount = 0;
    string temp = "";

    for (int i = 0; i < (int)s.length(); i++) {
        if (s[i] == '|') {
            parts[partCount++] = temp;
            temp = "";
        }
        else {
            temp += s[i];
        }
    }
    if (temp != "") parts[partCount++] = temp;

    HistNode* h = NULL;

    for (int i = partCount - 1; i >= 0; i--) {
        HistNode* n = new HistNode();
        n->action = parts[i];
        n->next = h;
        h = n;
    }

    return h;
}

// Save customers to CSV
void saveCustomersCSV() {
    ofstream file("customers.csv");
    if (!file) return;

    Customer* t = head;
    while (t) {
        file << t->id << ","
            << t->name << ","
            << t->password << ","
            << t->accountType << ","
            << t->balance << ","
            << historyToString(t->history)
            << "\n";
        t = t->next;
    }
}

// Load customers from CSV
void loadCustomersCSV() {
    ifstream file("customers.csv");

    if (!file) {
        ofstream create("customers.csv");
        return;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string idStr, name, pass, type, balStr, histStr;

        getline(ss, idStr, ',');
        getline(ss, name, ',');
        getline(ss, pass, ',');
        getline(ss, type, ',');
        getline(ss, balStr, ',');
        getline(ss, histStr);

        Customer* c = new Customer();
        c->id = stoi(idStr);
        c->name = name;
        c->password = pass;
        c->accountType = type;
        c->balance = stod(balStr);
        c->history = stringToHistory(histStr);
        c->next = head;
        head = c;

        usedIDs[idCount++] = c->id;
    }
}

// Save queue to CSV
void saveQueueCSV() {
    ofstream file("queue.csv");
    if (!file) return;

    QueueNode* t = qFront;
    while (t) {
        file << t->name << "\n";
        t = t->next;
    }
}

// Load queue from CSV
void loadQueueCSV() {
    ifstream file("queue.csv");

    if (!file) {
        ofstream create("queue.csv");
        return;
    }

    string name;
    while (getline(file, name)) {
        QueueNode* n = new QueueNode();
        n->name = name;
        n->next = NULL;

        if (!qRear) {
            qFront = qRear = n;
        }
        else {
            qRear->next = n;
            qRear = n;
        }
    }
}

// Save used IDs
void saveUsedIDsCSV() {
    ofstream file("used_ids.csv");
    if (!file) return;

    for (int i = 0; i < idCount; i++) {
        file << usedIDs[i] << "\n";
    }
}

// Load used IDs
void loadUsedIDsCSV() {
    ifstream file("used_ids.csv");

    if (!file) {
        ofstream create("used_ids.csv");
        return;
    }

    string line;
    while (getline(file, line)) {
        usedIDs[idCount++] = stoi(line);
    }
}

// Load all CSV files
void loadAllCSV() {
    loadUsedIDsCSV();
    loadCustomersCSV();
    loadQueueCSV();
}

// Save all CSV files
void saveAllCSV() {
    saveCustomersCSV();
    saveQueueCSV();
    saveUsedIDsCSV();
}

// Clear customers list
void clearCustomersList() {
    Customer* t = head;
    while (t) {
        Customer* del = t;
        t = t->next;

        HistNode* h = del->history;
        while (h) {
            HistNode* hd = h;
            h = h->next;
            delete hd;
        }

        delete del;
    }
    head = NULL;
}

// Clear queue
void clearQueue() {
    QueueNode* t = qFront;
    while (t) {
        QueueNode* del = t;
        t = t->next;
        delete del;
    }
    qFront = qRear = NULL;
}

// Clear used IDs
void clearUsedIDs() {
    idCount = 0;
}

// safe input
void safeInput(string& s) {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, s);
}

// push history
void pushHistory(Customer* c, const string& action) {
    HistNode* n = new HistNode();
    n->action = action;
    n->next = c->history;
    c->history = n;
}

// print history
void printHistory(Customer* c) {
    HistNode* t = c->history;
    if (!t) {
        cout << "   (No history)\n";
        return;
    }
    while (t) {
        cout << "   - " << t->action << endl;
        t = t->next;
    }
}

// enqueue
void enqueue(const string& name) {
    QueueNode* n = new QueueNode();
    n->name = name;
    n->next = NULL;

    if (!qRear) {
        qFront = qRear = n;
    }
    else {
        qRear->next = n;
        qRear = n;
    }
}

// print queue
void printQueue() {
    QueueNode* t = qFront;
    if (!t) {
        cout << "(Waiting queue empty)\n";
        return;
    }

    while (t) {
        cout << "- " << t->name << endl;
        t = t->next;
    }
}

// search customer
Customer* searchCustomer(int id) {
    Customer* t = head;
    while (t) {
        if (t->id == id) return t;
        t = t->next;
    }
    return NULL;
}

// add customer
void addCustomer() {
    int id = generateUniqueID();
    usedIDs[idCount++] = id;

    string name, password, type;
    double initialBalance;

    cout << "Generated Customer ID: " << id << endl;

    cout << "Enter name: ";
    safeInput(name);

    cout << "Create password: ";
    cin >> password;

    cout << "Account type: ";
    safeInput(type);

    cout << "Initial deposit: ";
    cin >> initialBalance;
    if (initialBalance < 0) initialBalance = 0;

    Customer* c = new Customer();
    c->id = id;
    c->name = name;
    c->password = password;
    c->accountType = type;
    c->balance = initialBalance;
    c->history = NULL;
    c->next = head;
    head = c;

    pushHistory(c, "Account opened with deposit +" + to_string(initialBalance));

    saveAllCSV();

    cout << "Customer added.\n";
}

// print customers
void printAllCustomers() {
    Customer* t = head;
    if (!t) {
        cout << "(No customers)\n";
        return;
    }

    while (t) {
        cout << "ID: " << t->id
            << " | Name: " << t->name
            << " | Type: " << t->accountType
            << " | Balance: $" << t->balance << endl;

        printHistory(t);
        cout << "-------------------------\n";

        t = t->next;
    }
}

// login
Customer* loginCustomer() {
    int id;
    string pass;

    cout << "Enter ID: ";
    cin >> id;

    Customer* c = searchCustomer(id);
    if (!c) {
        cout << "Customer not found.\n";
        return NULL;
    }

    cout << "Enter password: ";
    cin >> pass;

    if (pass != c->password) {
        cout << "Wrong password.\n";
        return NULL;
    }

    return c;
}

// deposit
void depositMoney() {
    Customer* c = loginCustomer();
    if (!c) return;

    double amount;
    cout << "Deposit amount: ";
    cin >> amount;

    if (amount <= 0) {
        cout << "Invalid amount.\n";
        return;
    }

    c->balance += amount;
    pushHistory(c, "Deposit +" + to_string(amount));

    saveAllCSV();

    cout << "Deposit successful.\n";
}

// withdraw
void withdrawMoney() {
    Customer* c = loginCustomer();
    if (!c) return;

    double amount;
    cout << "Withdraw amount: ";
    cin >> amount;

    if (amount <= 0) {
        cout << "Invalid amount.\n";
        return;
    }
    if (amount > c->balance) {
        cout << "Insufficient balance.\n";
        return;
    }

    c->balance -= amount;
    pushHistory(c, "Withdraw -" + to_string(amount));

    saveAllCSV();

    cout << "Withdrawal successful.\n";
}

// transfer
void transferMoney() {
    cout << "--- Transfer Money ---\n";

    cout << "Sender login:\n";
    Customer* sender = loginCustomer();
    if (!sender) return;

    int receiverID;
    cout << "Enter receiver ID: ";
    cin >> receiverID;

    Customer* receiver = searchCustomer(receiverID);
    if (!receiver) {
        cout << "Receiver not found.\n";
        return;
    }

    if (receiver == sender) {
        cout << "Cannot transfer to the same account.\n";
        return;
    }

    double amount;
    cout << "Transfer amount: ";
    cin >> amount;

    if (amount <= 0) {
        cout << "Invalid amount.\n";
        return;
    }
    if (amount > sender->balance) {
        cout << "Insufficient balance.\n";
        return;
    }

    sender->balance -= amount;
    receiver->balance += amount;

    pushHistory(sender, "Transfer sent -" + to_string(amount) + " to ID " + to_string(receiver->id));
    pushHistory(receiver, "Transfer received +" + to_string(amount) + " from ID " + to_string(sender->id));

    saveAllCSV();

    cout << "Transfer successful.\n";
}

// view account
void viewCustomerAccount() {
    Customer* c = loginCustomer();
    if (!c) return;

    cout << "ID: " << c->id << endl;
    cout << "Name: " << c->name << endl;
    cout << "Type: " << c->accountType << endl;
    cout << "Balance: $" << c->balance << endl;
    cout << "History:\n";
    printHistory(c);
}

// delete customer
void deleteCustomer() {
    Customer* c = loginCustomer();
    if (!c) return;

    int id = c->id;

    Customer* t = head;
    Customer* prev = NULL;

    while (t && t->id != id) {
        prev = t;
        t = t->next;
    }

    if (!t) return;

    if (!prev) head = t->next;
    else prev->next = t->next;

    HistNode* h = t->history;
    while (h) {
        HistNode* delh = h;
        h = h->next;
        delete delh;
    }

    delete t;

    saveAllCSV();

    cout << "Customer deleted.\n";
}

// update name
void updateCustomerName() {
    Customer* c = loginCustomer();
    if (!c) return;

    string newName;
    cout << "New name: ";
    safeInput(newName);

    string old = c->name;
    c->name = newName;

    pushHistory(c, "Name changed from " + old + " to " + newName);

    saveAllCSV();

    cout << "Name updated.\n";
}

// used IDs
void showUsedIDs() {
    cout << "Used IDs:\n";
    for (int i = 0; i < idCount; i++) {
        cout << usedIDs[i] << endl;
    }
}

// Insert node into BST
void insertRank(TreeNode*& root, const string& name, double balance) {
    if (!root) {
        root = new TreeNode();
        root->name = name;
        root->balance = balance;
        root->left = root->right = NULL;
        return;
    }

    if (balance > root->balance) {
        insertRank(root->left, name, balance);
    }
    else {
        insertRank(root->right, name, balance);
    }
}

// Clear BST
void clearRankTree(TreeNode* root) {
    if (!root) return;
    clearRankTree(root->left);
    clearRankTree(root->right);
    delete root;
}

// Build BST
void buildRankTree() {
    clearRankTree(rankRoot);
    rankRoot = NULL;

    Customer* t = head;
    while (t) {
        insertRank(rankRoot, t->name, t->balance);
        t = t->next;
    }
}

// Print BST
void printRankTree(TreeNode* root) {
    if (!root) return;
    printRankTree(root->left);
    cout << root->name << " | $" << root->balance << endl;
    printRankTree(root->right);
}

// Show ranking
void showBalanceRanking() {
    if (!head) {
        cout << "(No customers to rank)\n";
        return;
    }

    buildRankTree();
    cout << "--- Balance Ranking (High to Low) ---\n";
    printRankTree(rankRoot);
}

// print menu
void printMenu() {
    cout << "\n===== Bank Menu =====\n";
    cout << "1. Add Customer\n";
    cout << "2. Deposit Money\n";
    cout << "3. Withdraw Money\n";
    cout << "4. Show All Customers\n";
    cout << "5. Transfer Money\n";
    cout << "6. Show Balance Ranking (BST)\n";
    cout << "7. Add Waiting Customer\n";
    cout << "8. Show Waiting Queue\n";
    cout << "9. Show Used IDs\n";
    cout << "10. Delete Customer\n";
    cout << "11. Update Name\n";
    cout << "12. View My Account\n";
    cout << "13. Exit\n";
    cout << "=====================\n";
    cout << "Enter Choice: ";
}

int main() {
    srand(time(NULL));

    loadAllCSV();

    cout << "===== Welcome To The Bank System =====\n";
    cout << "===== Data Loaded Successfully =====\n";

    int choice;

    while (true) {
        printMenu();
        cin >> choice;

        switch (choice) {
        case 1: addCustomer(); break;
        case 2: depositMoney(); break;
        case 3: withdrawMoney(); break;
        case 4: printAllCustomers(); break;
        case 5: transferMoney(); break;
        case 6: showBalanceRanking(); break;
        case 7: {
            string name;
            cout << "Enter customer name: ";
            safeInput(name);
            enqueue(name);
            break;
        }
        case 8: printQueue(); break;
        case 9: showUsedIDs(); break;
        case 10: deleteCustomer(); break;
        case 11: updateCustomerName(); break;
        case 12: viewCustomerAccount(); break;

        case 13:
            saveAllCSV();
            cout << "Saving data and exiting...\n";
            return 0;

        default:
            cout << "Invalid choice!\n";
        }
    }
}
