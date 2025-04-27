#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

using namespace std;

// Helper function to trim whitespace
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, (last - first + 1));
}

// Helper function to convert string to uppercase
string toUpper(string str) {
    str = trim(str);
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] = str[i] - ('a' - 'A');
        }
    }
    return str;
}

// Exception classes
class ECommerceException {
private:
    string message;
public:
    ECommerceException(const string& msg) : message(msg) {}
    const char* what() const { return message.c_str(); }
};

class ProductNotFoundException : public ECommerceException {
public:
    ProductNotFoundException() : ECommerceException("Product not found!") {}
};

class InvalidInputException : public ECommerceException {
public:
    InvalidInputException() : ECommerceException("Invalid input!") {}
};

// Product class with 3-letter ID
class Product {
private:
    string id;
    string name;
    double price;
public:
    Product(const string& id, const string& name, double price) 
        : id(toUpper(id)), name(name), price(price) {}
    string getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
};

// Shopping Cart Item class
class CartItem {
private:
    Product product;
    int quantity;
public:
    CartItem(const Product& product, int quantity) : product(product), quantity(quantity) {}
    Product getProduct() const { return product; }
    int getQuantity() const { return quantity; }
    double getTotalPrice() const { return product.getPrice() * quantity; }
};

// Order class
class Order {
private:
    static int nextOrderId;
    int orderId;
    double totalAmount;
    string paymentMethod;
    CartItem* items[100];
    int itemCount = 0;

public:
    Order(double total, const string& paymentMethod) 
        : orderId(nextOrderId++), totalAmount(total), paymentMethod(paymentMethod) {}

    void addItem(const CartItem* item) {
        if (itemCount < 100) {
            items[itemCount++] = new CartItem(*item);
        }
    }

    void display() const {
        cout << "\nOrder ID: " << orderId << endl
             << "Total Amount: " << fixed << setprecision(2) << totalAmount << endl
             << "Payment Method: " << paymentMethod << endl
             << "Order Details:\n"
             << left << setw(10) << "Product ID" << setw(20) << "Name" 
             << setw(10) << "Price" << setw(10) << "Quantity" << endl;
             
        for (int i = 0; i < itemCount; i++) {
            const Product& p = items[i]->getProduct();
            cout << left << setw(10) << p.getId()
                 << setw(20) << p.getName()
                 << fixed << setprecision(2) << setw(10) << p.getPrice()
                 << setw(10) << items[i]->getQuantity()
                 << endl;
        }
    }

    void logToFile() const {
        ofstream file("orders.log", ios::app);
        if (file.is_open()) {
            file << "[LOG] -> Order ID: " << orderId 
                 << " has been successfully checked out and paid using " 
                 << paymentMethod << ".\n";
            file.close();
        }
    }
};
int Order::nextOrderId = 1;

// Payment Strategy
class PaymentStrategy {
public:
    virtual void pay(double amount) = 0;
    virtual string getMethodName() const = 0;
    virtual ~PaymentStrategy() = default;
};

class CashPayment : public PaymentStrategy {
public:
    void pay(double amount) override {
        cout << "Paid " << fixed << setprecision(2) << amount << " using Cash\n";
    }
    string getMethodName() const override { return "Cash"; }
};

class CardPayment : public PaymentStrategy {
public:
    void pay(double amount) override {
        cout << "Paid " << fixed << setprecision(2) << amount << " using Credit/Debit Card\n";
    }
    string getMethodName() const override { return "Credit/Debit Card"; }
};

class GCashPayment : public PaymentStrategy {
public:
    void pay(double amount) override {
        cout << "Paid " << fixed << setprecision(2) << amount << " using GCash\n";
    }
    string getMethodName() const override { return "GCash"; }
};

// Singleton E-Commerce System
class ECommerceSystem {
private:
    static ECommerceSystem* instance;
    
    Product products[5] = {
        {"ABC", "Eraser", 56.00},
        {"DEF", "Sharpener", 48.00},
        {"GHI", "Ballpen", 25.00},
        {"JKL", "Candy", 1.50},
        {"MNO", "Gum", 4.99}
    };
    
    CartItem* cartItems[100];
    int cartItemCount = 0;
    Order* orders[100];
    int orderCount = 0;

    ECommerceSystem() = default;

    // Helper function to validate yes/no input
    bool validateYesNo(char& choice) {
        string input;
        getline(cin, input);
        input = trim(input);
        if (input.length() == 1) {
            choice = toupper(input[0]);
            return (choice == 'Y' || choice == 'N');
        }
        return false;
    }

    // Helper function to validate product ID
    string validateProductID() {
        string input;
        bool valid = false;
        do {
            cout << "Enter product ID to add to cart: ";
            getline(cin, input);
            input = trim(input);
            if (input.length() == 3) {
                valid = true;
                for (char c : input) {
                    if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))) {
                        valid = false;
                        break;
                    }
                }
            }
            if (!valid) {
                cout << "Invalid input! Product ID must be 3 letters. Try again.\n";
            }
        } while (!valid);
        return toUpper(input);
    }

    // Helper function to validate quantity
    int validateQuantity() {
        string input;
        int quantity = 0;
        bool valid = false;
        
        do {
            cout << "Enter quantity: ";
            getline(cin, input);
            input = trim(input);
            try {
                quantity = stoi(input);
                if (quantity > 0) {
                    valid = true;
                } else {
                    cout << "Please enter a positive number: ";
                }
            } catch (...) {
                cout << "Invalid input! Please enter a number: ";
            }
        } while (!valid);
        
        return quantity;
    }

    // Helper function to validate menu choice
    char validateMenuChoice() {
        string input;
        char choice = 0;
        bool valid = false;
        
        do {
            getline(cin, input);
            input = trim(input);
            if (input.length() == 1 && input[0] >= '1' && input[0] <= '4') {
                choice = input[0];
                valid = true;
            } else {
                cout << "Invalid choice! Please enter 1-4: ";
            }
        } while (!valid);
        
        return choice;
    }

public:
    ECommerceSystem(const ECommerceSystem&) = delete;
    ECommerceSystem& operator=(const ECommerceSystem&) = delete;

    static ECommerceSystem* getInstance() {
        if (!instance) {
            instance = new ECommerceSystem();
        }
        return instance;
    }

    void displayProducts() {
        cout << "\nAvailable Products:\n"
             << left << setw(15) << "Product ID" << setw(20) << "Name" 
             << setw(10) << "Price" << endl;
        
        for (const auto& product : products) {
            cout << left << setw(15) << product.getId()
                 << setw(20) << product.getName()
                 << fixed << setprecision(2) << setw(10) << product.getPrice()
                 << endl;
        }
    }

    void addToCart() {
        char choice;
        do {
            displayProducts();
            
            string id = validateProductID();
            
            bool found = false;
            for (const auto& product : products) {
                if (product.getId() == id) {
                    int quantity = validateQuantity();
                    
                    if (cartItemCount < 100) {
                        cartItems[cartItemCount++] = new CartItem(product, quantity);
                        cout << "Product added successfully!\n";
                    }
                    found = true;
                    break;
                }
            }

            if (!found) {
                cout << "Product not found! Try again.\n";
                continue;
            }

            cout << "Add another product? (Y/N): ";
            if (!validateYesNo(choice)) {
                cout << "Invalid input! Please enter Y or N: ";
                continue;
            }
        } while (choice == 'Y');
    }

    void viewCart() {
        if (cartItemCount == 0) {
            cout << "Your cart is empty.\n";
            return;
        }

        double total = 0;
        cout << "\nYour Shopping Cart:\n"
             << left << setw(10) << "Product ID" << setw(20) << "Name" 
             << setw(10) << "Price" << setw(10) << "Quantity" << endl;
        
        for (int i = 0; i < cartItemCount; i++) {
            const Product& p = cartItems[i]->getProduct();
            cout << left << setw(10) << p.getId()
                 << setw(20) << p.getName()
                 << fixed << setprecision(2) << setw(10) << p.getPrice()
                 << setw(10) << cartItems[i]->getQuantity()
                 << endl;
            total += cartItems[i]->getTotalPrice();
        }
        cout << "Total: " << fixed << setprecision(2) << total << endl;

        cout << "\nCheckout? (Y/N): ";
        char choice;
        if (!validateYesNo(choice)) {
            cout << "Invalid input! Please enter Y or N: ";
            return;
        }
        if (choice == 'Y') checkout();
    }

    void checkout() {
        if (cartItemCount == 0) return;

        double total = 0;
        for (int i = 0; i < cartItemCount; i++) {
            total += cartItems[i]->getTotalPrice();
        }

        cout << "\nSelect payment method:\n"
             << "1. Cash\n2. Credit/Debit Card\n3. GCash\nChoice: ";
        
        char choice = validateMenuChoice();
        while (choice < '1' || choice > '3') {
            cout << "Invalid choice! Please enter 1-3: ";
            choice = validateMenuChoice();
        }

        PaymentStrategy* payment = nullptr;
        switch (choice) {
            case '1': payment = new CashPayment(); break;
            case '2': payment = new CardPayment(); break;
            case '3': payment = new GCashPayment(); break;
        }

        payment->pay(total);
        
        if (orderCount < 100) {
            orders[orderCount] = new Order(total, payment->getMethodName());
            for (int i = 0; i < cartItemCount; i++) {
                orders[orderCount]->addItem(cartItems[i]);
            }
            orders[orderCount]->logToFile();
            orderCount++;
        }

        cout << "[ORDER] Order placed successfully!\n";
        
        // Clear cart
        for (int i = 0; i < cartItemCount; i++) {
            delete cartItems[i];
        }
        cartItemCount = 0;
        delete payment;
    }

    void viewOrders() {
        if (orderCount == 0) {
            cout << "No orders found.\n";
            return;
        }

        cout << "\nOrder History:\n";
        for (int i = 0; i < orderCount; i++) {
            orders[i]->display();
        }
    }

    void run() {
        char choice;
        do {
            cout << "\n===== E-Commerce Menu =====\n"
                 << "1. View Products\n"
                 << "2. View Cart\n"
                 << "3. View Orders\n"
                 << "4. Exit\n"
                 << "Choice: ";
            
            choice = validateMenuChoice();

            try {
                switch (choice) {
                    case '1': 
                        addToCart(); 
                        break;
                    case '2': 
                        viewCart(); 
                        break;
                    case '3': 
                        viewOrders(); 
                        break;
                    case '4': 
                        return;
                }
            } catch (const ECommerceException& e) {
                cout << "Error: " << e.what() << endl;
            }
        } while (choice != '4');
    }
};

ECommerceSystem* ECommerceSystem::instance = nullptr;

int main() {
    ECommerceSystem* system = ECommerceSystem::getInstance();
    system->run();
    return 0;
}