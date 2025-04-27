#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

using namespace std;

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

// Product class
class Product {
private:
    int id;
    string name;
    double price;
public:
    Product(int id, const string& name, double price) : id(id), name(name), price(price) {}
    int getId() const { return id; }
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
             << left << setw(12) << "Product ID" << setw(20) << "Name" 
             << setw(10) << "Price" << setw(10) << "Quantity" << endl;
             
        for (int i = 0; i < itemCount; i++) {
            const Product& p = items[i]->getProduct();
            cout << left << setw(12) << p.getId()
                 << setw(20) << p.getName()
                 << fixed << setprecision(2) << setw(10) << p.getPrice()
                 << setw(10) << items[i]->getQuantity() << endl;
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
        {1, "Laptop", 56.00},
        {2, "Smartphone", 48.00},
        {3, "Headphones", 25.00},
        {4, "Mouse", 1.50},
        {5, "Keyboard", 4.99}
    };
    
    CartItem* cartItems[100];
    int cartItemCount = 0;
    Order* orders[100];
    int orderCount = 0;

    ECommerceSystem() = default;

    // Helper function to validate yes/no input
    bool validateYesNo(char& choice) {
        while (true) {
            string input;
            getline(cin, input);
            
            if (input.length() == 1) {
                choice = toupper(input[0]);
                if (choice == 'Y' || choice == 'N') {
                    return true;
                }
            }
            cout << "Invalid input! Please enter Y or N: ";
        }
    }

    // Helper function to validate numeric input
    int validateNumberInput(const string& prompt, bool positiveOnly = true) {
        while (true) {
            cout << prompt;
            string input;
            getline(cin, input);
            
            try {
                size_t pos;
                int value = stoi(input, &pos);
                
                // Check if entire string was processed and meets positive requirement
                if (pos == input.length() && (!positiveOnly || value > 0)) {
                    return value;
                }
            } catch (...) {
                // Catch any conversion errors
            }
            
            cout << "Invalid input! Please enter " << (positiveOnly ? "a positive number" : "a number") << ": ";
        }
    }

    // Helper function to validate menu choice
    char validateMenuChoice() {
        while (true) {
            string input;
            getline(cin, input);
            
            if (input.length() == 1 && input[0] >= '1' && input[0] <= '4') {
                return input[0];
            }
            cout << "Invalid choice! Please enter 1-4: ";
        }
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
             << left << setw(12) << "Product ID" << setw(20) << "Name" 
             << setw(10) << "Price" << endl;
        
        for (const auto& product : products) {
            cout << left << setw(12) << product.getId()
                 << setw(20) << product.getName()
                 << fixed << setprecision(2) << setw(10) << product.getPrice()
                 << endl;
        }
    }

    void addToCart() {
        char choice;
        do {
            displayProducts();
            
            int id = validateNumberInput("\nEnter product ID to add to cart: ", false);
            
            bool found = false;
            for (const auto& product : products) {
                if (product.getId() == id) {
                    int quantity = validateNumberInput("Enter quantity: ");
                    
                    if (cartItemCount < 100) {
                        cartItems[cartItemCount++] = new CartItem(product, quantity);
                        cout << "Product added successfully!\n";
                    }
                    found = true;
                    break;
                }
            }

            if (!found) {
                throw ProductNotFoundException();
            }

            cout << "Add another product? (Y/N): ";
            validateYesNo(choice);
        } while (choice == 'Y');
    }

    void viewCart() {
        if (cartItemCount == 0) {
            cout << "Your cart is empty.\n";
            return;
        }

        double total = 0;
        cout << "\nYour Shopping Cart:\n"
             << left << setw(12) << "Product ID" << setw(20) << "Name" 
             << setw(10) << "Price" << setw(10) << "Quantity" << endl;
        
        for (int i = 0; i < cartItemCount; i++) {
            const Product& p = cartItems[i]->getProduct();
            cout << left << setw(12) << p.getId()
                 << setw(20) << p.getName()
                 << fixed << setprecision(2) << setw(10) << p.getPrice()
                 << setw(10) << cartItems[i]->getQuantity()
                 << endl;
            total += cartItems[i]->getTotalPrice();
        }
        cout << "Total: " << fixed << setprecision(2) << total << endl;

        cout << "\nCheckout? (Y/N): ";
        char choice;
        validateYesNo(choice);
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
        while (true) {
            cout << "\n===== E-Commerce Menu =====\n"
                 << "1. View Products\n"
                 << "2. View Cart\n"
                 << "3. View Orders\n"
                 << "4. Exit\n"
                 << "Choice: ";
            
            char choice = validateMenuChoice();

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
        }
    }
};

ECommerceSystem* ECommerceSystem::instance = nullptr;

int main() {
    ECommerceSystem* system = ECommerceSystem::getInstance();
    system->run();
    return 0;
}