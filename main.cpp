#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>
#include <QFile>
#include <QTextStream>
#include <QDialog>
#include <QHBoxLayout>
#include <QPixmap>
#include <QIcon>
#include <QListWidgetItem>
#include <QFont>
#include <QtSql>
#include <QTimer>
#include <QDebug>

// Hello Prof, we have carefully written the code and we have applied some of the things we learnt in the class to this program.
// Although some part was a bit Challenging , but we were able to achieve the goal of the project.
// Our Database Connection, it is hosted on https://neon.tech/. aws server.
// some sql query are used with our c++ code , for interaction with our database and to execute sql queries.

bool connectToDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("ep-restless-silence-a5ilvpoq.us-east-2.aws.neon.tech");
    db.setDatabaseName("Restaurant Ordering System");
    db.setUserName("Tundedb_owner");
    db.setPassword("pvmCY9Tae2xS");
    db.setConnectOptions("sslmode=require");

    if (!db.open()) {
        qDebug() << "Error: Unable to connect to the database!";
        QMessageBox::critical(nullptr, "Database Connection Error", db.lastError().text());
        return false;
    }

    qDebug() << "Successfully connected to the database!";
    QMessageBox::information(nullptr, "Database Connection", "Successfully connected to the database!");
    return true;
}


// Here we designed the Ui dialog box, the style ans size of each frames and button
class WelcomeDialog : public QDialog {
public:
    WelcomeDialog();
};

WelcomeDialog::WelcomeDialog() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    setMinimumWidth(800);  // we use 800 because the desktop is on larger screen

    QLabel *welcomeLabel = new QLabel("Welcome to Tunde and Sample Restaurant 😊\nEnjoy our delicious meals!", this);
    welcomeLabel->setStyleSheet(
        "font-size: 28px; "  
        "font-weight: bold; "
        "color: #2C3E50; "
        "background-color: #ECF0F1; "
        "border-radius: 5px; "
        "padding: 20px;"  
    );
    
    welcomeLabel->setAlignment(Qt::AlignCenter);

    QLabel *imageLabel = new QLabel(this);
    QPixmap pixmap("C:/Users/wmj/Desktop/RestaurantOrderingSystem/images/menu.jpg");
    pixmap = pixmap.scaled(400, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation);  // we increased the image size for better view 
    imageLabel->setPixmap(pixmap);
    imageLabel->setAlignment(Qt::AlignCenter);

    QPushButton *proceedButton = new QPushButton("Proceed", this);
    proceedButton->setStyleSheet(
        "background-color: #3498DB; "
        "color: white; "
        "border-radius: 5px; "
        "padding: 15px; "  
        "font-size: 18px;"  
    );
    proceedButton->setFixedWidth(200); 

    layout->addWidget(imageLabel);
    layout->addWidget(welcomeLabel);
    layout->addWidget(proceedButton, 0, Qt::AlignCenter);

    connect(proceedButton, &QPushButton::clicked, this, &QDialog::accept);
}


// we also styled the Wdiget for better Ui design and menu driven interface

class GridWidget : public QWidget {
public:
    QGridLayout *gridLayout;
    
    GridWidget(QWidget *parent = nullptr);

    void addItem(const QString &name, const QString &price, const QString &imagePath);
};

GridWidget::GridWidget(QWidget *parent) : QWidget(parent) {
    gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(10);
}

void GridWidget::addItem(const QString &name, const QString &price, const QString &imagePath) {
    int row = gridLayout->count() / 4;
    int col = gridLayout->count() % 4;

    QWidget *itemWidget = new QWidget;
    QVBoxLayout *itemLayout = new QVBoxLayout(itemWidget);

    QLabel *imageLabel = new QLabel;
    QPixmap pixmap(imagePath);
    pixmap = pixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imageLabel->setPixmap(pixmap);
    imageLabel->setAlignment(Qt::AlignCenter);

    QLabel *nameLabel = new QLabel(name);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setStyleSheet("font-weight: bold; font-size: 14px;");

    QLabel *priceLabel = new QLabel(price);
    priceLabel->setAlignment(Qt::AlignCenter);
    priceLabel->setStyleSheet("color: #27AE60; font-weight: bold;");

    itemLayout->addWidget(imageLabel);
    itemLayout->addWidget(nameLabel);
    itemLayout->addWidget(priceLabel);

    itemWidget->setStyleSheet(
        "QWidget {"
        "border: 1px solid #BDC3C7;"
        "border-radius: 5px;"
        "background-color: white;"
        "padding: 5px;"
        "}"
    );

    gridLayout->addWidget(itemWidget, row, col);
}

// we defined our Class Restautant app here and we use void because we don't need to return any value from these functions. 

class RestaurantApp : public QWidget {
public:
    RestaurantApp();

    void placeOrder(QListWidget *cartList, QLabel *revenueLabel, QLabel *avgOrderLabel, QLabel *mostPopularDishLabel);
    void fetchMenuItems(GridWidget *menuGrid);
    void fetchIngredients(GridWidget *ingredientGrid);
    void fetchAverageOrderPrice(QLabel *avgOrderLabel);
    void updateMostPopularDish(QLabel *mostPopularDishLabel);
    void updateOrderAnalysis();
    void showProfile();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void handleItemClick(const QString &name, const QString &price);
    
    QString currentCustomerName;
};

RestaurantApp::RestaurantApp() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setMinimumSize(1200, 800);  

    QLabel *title = new QLabel("Restaurant Ordering System 😊", this);
    title->setStyleSheet("font-size: 32px; font-weight: bold; color: #2980B9; padding: 20px;");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    //  we Created the QLabel for the typing effect

    QLabel *typingLabel = new QLabel(this);
    typingLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #3498DB; padding: 10px;");
    typingLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(typingLabel);

    QString typingText = "🎉✨ Welcome to the *Best* Restaurant in UESTC! 🍕🍔🍟 😋✨ "
                     "Get ready for an *unforgettable* dining experience! 🎉";

    int typingInterval = 100;  
    int delayBeforeRestart = 1000; 

    QTimer *typingTimer = new QTimer(this);
    QTimer *delayTimer = new QTimer(this); 

    int typingIndex = 0;

    connect(typingTimer, &QTimer::timeout, [=, &typingIndex]() mutable {
    
    if (typingIndex < typingText.length()) {
        typingLabel->setText(typingLabel->text() + typingText[typingIndex]);
        typingIndex++;
    } else {
        typingTimer->stop();
        delayTimer->start(delayBeforeRestart);
    }
});


// Here, the program  will Restart the typing effect after a delay

connect(delayTimer, &QTimer::timeout, [=, &typingIndex]() {
    typingLabel->clear(); 
    typingIndex = 0; 
    delayTimer->stop(); 
    typingTimer->start(typingInterval); 
});

QTimer::singleShot(1000, [=]() {
    typingTimer->start(typingInterval);
});

    //  We styled the Menu list Grid,. for proper display 

    QLabel *menuListTitle = new QLabel("Menu List:", this);
    menuListTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: #2980B9;");
    mainLayout->addWidget(menuListTitle);

    GridWidget *menuGrid = new GridWidget(this);
    fetchMenuItems(menuGrid);
    mainLayout->addWidget(menuGrid);

    // Also, we styled the Ingredients Grid to display grid format other than list format
    QLabel *ingredientListTitle = new QLabel("Ingredients List:", this);
    ingredientListTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: #2980B9;");
    mainLayout->addWidget(ingredientListTitle);

    GridWidget *ingredientGrid = new GridWidget(this);
    fetchIngredients(ingredientGrid);
    mainLayout->addWidget(ingredientGrid);

    // We desinged the Cart Section frame 
    QLabel *cartTitle = new QLabel("Your Cart:", this);
    cartTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: #27AE60;");
    mainLayout->addWidget(cartTitle);

    QListWidget *cartList = new QListWidget(this);
    cartList->setStyleSheet("QListWidget { font-size: 16px; }");
    mainLayout->addWidget(cartList);

    // We also designed the Buttons for visibility
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *checkoutButton = new QPushButton("Checkout", this);
    QPushButton *removeItemButton = new QPushButton("Remove Item", this);
    QPushButton *clearCartButton = new QPushButton("Clear Cart", this);
        QPushButton *profileButton = new QPushButton("My Profile", this);

    QString buttonStyle = 
        "QPushButton {"
        "   font-size: 16px;"
        "   padding: 10px 20px;"
        "   border-radius: 5px;"
        "   font-weight: bold;"
        "   color: white;"
        "}";

    checkoutButton->setStyleSheet(buttonStyle + "background-color: #3498DB;");  // Blue
    removeItemButton->setStyleSheet(buttonStyle + "background-color: #E74C3C;");  // Red
    clearCartButton->setStyleSheet(buttonStyle + "background-color: #F39C12;");  // Orange
    profileButton->setStyleSheet(buttonStyle + "background-color: #9B59B6;");  // Purple

    buttonLayout->addWidget(checkoutButton);
    buttonLayout->addWidget(removeItemButton);
    buttonLayout->addWidget(clearCartButton);
    buttonLayout->addWidget(profileButton);
    mainLayout->addLayout(buttonLayout);

    // Below the frame, we have the Order_ analysis column
    QLabel *revenueLabel = new QLabel("Total Revenue: $0.00", this);
    QLabel *avgOrderLabel = new QLabel("Average Order Price: $0.00", this);
    QLabel *mostPopularDishLabel = new QLabel("Most Popular Dish: N/A", this);

    QString statsStyle = "font-size: 16px; font-weight: bold; color: #2C3E50; margin-top: 10px;";
    revenueLabel->setStyleSheet(statsStyle);
    avgOrderLabel->setStyleSheet(statsStyle);
    mostPopularDishLabel->setStyleSheet(statsStyle);

    mainLayout->addWidget(revenueLabel);
    mainLayout->addWidget(avgOrderLabel);
    mainLayout->addWidget(mostPopularDishLabel);

    // the loops iterate all over items in the grid layout of our menu grid one by one for the menus
    for (int i = 0; i < menuGrid->gridLayout->count(); ++i) {
        QWidget *widget = menuGrid->gridLayout->itemAt(i)->widget();
        if (widget) {
            widget->setMouseTracking(true);
            widget->installEventFilter(this);
        }
    }

    // same here for the ingredients
    for (int i = 0; i < ingredientGrid->gridLayout->count(); ++i) {
        QWidget *widget = ingredientGrid->gridLayout->itemAt(i)->widget();
        if (widget) {
            widget->setMouseTracking(true);
            widget->installEventFilter(this);
        }
    }

    // we defined the triggers for Connect buttons, place order, remove item, clear cart fucntions
    connect(checkoutButton, &QPushButton::clicked, [=]() {
        placeOrder(cartList, revenueLabel, avgOrderLabel, mostPopularDishLabel);
    });

    connect(removeItemButton, &QPushButton::clicked, [=]() {
        QListWidgetItem *item = cartList->currentItem();
        if (item) delete item;
    });

    connect(clearCartButton, &QPushButton::clicked, cartList, &QListWidget::clear);
    connect(profileButton, &QPushButton::clicked, this, &RestaurantApp::showProfile);
}

// here we confirmed if cart is empty and return error. customer must select item before checking out 
void RestaurantApp::placeOrder(QListWidget *cartList, QLabel *revenueLabel, QLabel *avgOrderLabel, QLabel *mostPopularDishLabel) {
    if (cartList->count() == 0) {
        QMessageBox::warning(this, "Empty Cart", "Please add items to your cart before checking out.");
        return;
    }

    bool ok;
    QString customerName = QInputDialog::getText(this, "Customer Name", "Enter your name:", QLineEdit::Normal, "", &ok);
    if (!ok || customerName.isEmpty()) return;

    // Storing the customer's name
    currentCustomerName = customerName;

    QString phoneNumber = QInputDialog::getText(this, "Phone Number", "Enter your phone number:", QLineEdit::Normal, "", &ok);
    if (!ok || phoneNumber.isEmpty()) return;

    double totalOrderPrice = 0.0;
    QVector<QPair<QString, QPair<double, int>>> orderItems;  //we Stored the items for insertion later, including quantity

    // Calculating total order price and collect order items (name, price, quantity)
    for (int i = 0; i < cartList->count(); ++i) {
        QString cartText = cartList->item(i)->text();
        QStringList parts = cartText.split(" - Total: $");
        if (parts.size() == 2) {
            QString itemName = parts[0].split(" x").first();  // Dish name without quantity
            int quantity = parts[0].split(" x").last().toInt();  // Extract quantity
            double itemPrice = parts[1].toDouble();  // Total price for that item
            double singleItemPrice = itemPrice / quantity;  // Calculate price per single item
            totalOrderPrice += itemPrice;

            orderItems.append(qMakePair(itemName, qMakePair(singleItemPrice, quantity)));  // Add the item to the order items list with quantity
        }
    }

    // sql queries to Insert order into the orders table in our database 
    QSqlQuery query;
    query.prepare("INSERT INTO orders (customer_name, phone_number, total_price, order_date) VALUES (:customer_name, :phone_number, :total_price, current_date)");
    query.bindValue(":customer_name", customerName);
    query.bindValue(":phone_number", phoneNumber);
    query.bindValue(":total_price", totalOrderPrice);

    if (query.exec()) {
        // Order placed successfully, retrieve the orderId
        int orderId = query.lastInsertId().toInt();

        // Then we use sql quesries to  Insert order items into the order_items table
        QSqlQuery itemQuery;
        itemQuery.prepare("INSERT INTO order_items (order_id, recipe_name, price, quantity, total_price) VALUES (:order_id, :recipe_name, :price, :quantity, :total_price)");

        for (const auto &item : orderItems) {
            itemQuery.bindValue(":order_id", orderId);           // we used the orderId from the 'orders' table
            itemQuery.bindValue(":recipe_name", item.first);     // we binded the item name (recipe name)
            itemQuery.bindValue(":price", item.second.first);    // we binded the price per single item
            itemQuery.bindValue(":quantity", item.second.second); // we binded the quantity
            itemQuery.bindValue(":total_price", item.second.first * item.second.second);    // get total price

            if (!itemQuery.exec()) {
                qDebug() << "Error inserting order item: " << itemQuery.lastError().text();
                return;  // Stop execution if an error occurs while inserting order items for tracing if we have errors 
            }
        }

        QMessageBox::information(this, "Order Placed", "Your order has been placed successfully!\nTotal Amount: $" + QString::number(totalOrderPrice, 'f', 2));

        // Clearing the cart after placing the order
        cartList->clear();

        // Updating the revenue label
        revenueLabel->setText("Total Revenue: $" + QString::number(totalOrderPrice, 'f', 2));

        // Fetching the average order price from the database and update the label
        fetchAverageOrderPrice(avgOrderLabel);

        // Updating the most popular dish label
        updateMostPopularDish(mostPopularDishLabel);

        // Updating the order analysis table
        updateOrderAnalysis();

    } else {
        // If there was an error placing the order
        QMessageBox::critical(this, "Database Error", query.lastError().text());
    }
}

//queries to fetch menu items form database 
void RestaurantApp::fetchMenuItems(GridWidget *menuGrid) {
    QSqlQuery query;
    if (!query.exec("SELECT recipe_name, price FROM recipes")) {
        QMessageBox::critical(this, "Database Error", query.lastError().text());
        return;
    }

    while (query.next()) {
        QString recipeName = query.value(0).toString();
        QString price = "$" + query.value(1).toString();
        menuGrid->addItem(recipeName, price, "C:/Users/wmj/Desktop/RestaurantOrderingSystem/images/" + recipeName.toLower() + ".jpg");
    }
}

//  We populated a grid with ingredients and their associated prices, along with an image for each ingredient.
void RestaurantApp::fetchIngredients(GridWidget *ingredientGrid) {
    QStringList ingredients = {"Tomato", "Cheese", "Chicken", "Basil"};
    double prices[] = {1.50, 2.00, 5.00, 0.50};

    for (int i = 0; i < ingredients.size(); ++i) {
        ingredientGrid->addItem(
            ingredients[i],
            "$" + QString::number(prices[i], 'f', 2),
            "C:/Users/wmj/Desktop/RestaurantOrderingSystem/images/" + ingredients[i].toLower() + ".jpg"
        );
    }
}

void RestaurantApp::fetchAverageOrderPrice(QLabel *avgOrderLabel) {
    // Fetch average order price from the database
    QSqlQuery query("SELECT AVG(total_price) FROM orders WHERE DATE(order_date) = current_date");
    if (query.next()) {
        double avgPrice = query.value(0).toDouble();
        avgOrderLabel->setText("Average Order Price: $" + QString::number(avgPrice, 'f', 2));
    } else {
        avgOrderLabel->setText("Average Order Price: N/A");
    }
}

void RestaurantApp::updateMostPopularDish(QLabel *mostPopularDishLabel) {
    // Fetching  the most popular dish from the database
    QSqlQuery query("SELECT recipe_name, COUNT(*) AS count FROM order_items GROUP BY recipe_name ORDER BY COUNT(*) DESC LIMIT 1");
    if (query.next()) {
        QString mostPopularDish = query.value(0).toString();
        mostPopularDishLabel->setText("Most Popular Dish: " + mostPopularDish);
    } else {
        mostPopularDishLabel->setText("Most Popular Dish: N/A");
    }
}

void RestaurantApp::updateOrderAnalysis() {
    QSqlQuery query;

    // Calculating  total orders and total revenue
    query.prepare("SELECT COUNT(*) AS total_orders, SUM(total_price) AS total_revenue FROM orders WHERE DATE(order_date) = current_date");
    if (!query.exec() || !query.next()) {
        qDebug() << "Error calculating total orders and revenue: " << query.lastError().text();
        return;
    }

    int totalOrders = query.value(0).toInt();
    double totalRevenue = query.value(1).toDouble();

    // Calculating average order price
    query.prepare("SELECT AVG(total_price) AS average_order_price FROM orders WHERE DATE(order_date) = current_date");
    if (!query.exec() || !query.next()) {
        qDebug() << "Error calculating average order price: " << query.lastError().text();
        return;
    }

    double averageOrderPrice = query.value(0).toDouble();

    // Getting the most popular dish
    query.prepare("SELECT recipe_name FROM order_items GROUP BY recipe_name ORDER BY COUNT(*) DESC LIMIT 1");
    if (!query.exec() || !query.next()) {
        qDebug() << "Error calculating most popular dish: " << query.lastError().text();
        return;
    }

    QString mostPopularDish = query.value(0).toString();

    //  we tried to Insert the analysis data into the order_analysis table with the current date
    query.prepare("INSERT INTO order_analysis (analysis_date, total_orders, total_revenue, average_order_price, most_popular_dish) "
                  "VALUES (current_date, :total_orders, :total_revenue, :average_order_price, :most_popular_dish) "
                  "ON CONFLICT (analysis_date) DO UPDATE SET "
                  "total_orders = EXCLUDED.total_orders, "
                  "total_revenue = EXCLUDED.total_revenue, "
                  "average_order_price = EXCLUDED.average_order_price, "
                  "most_popular_dish = EXCLUDED.most_popular_dish");

    query.bindValue(":total_orders", totalOrders);
    query.bindValue(":total_revenue", totalRevenue);
    query.bindValue(":average_order_price", averageOrderPrice);
    query.bindValue(":most_popular_dish", mostPopularDish);

    if (!query.exec()) {
        qDebug() << "Error inserting/updating order analysis: " << query.lastError().text();
    } else {
        qDebug() << "Order analysis updated successfully!";
    }
}

void RestaurantApp::showProfile() {
    QDialog profileDialog(this);
    profileDialog.setWindowTitle("My Profile");
    QVBoxLayout *profileLayout = new QVBoxLayout(&profileDialog);

    // Fetching customers orders from database 
    QSqlQuery query;
    query.prepare("SELECT order_id, customer_name, phone_number, total_price, order_date FROM orders WHERE customer_name = :customer_name");
    query.bindValue(":customer_name", currentCustomerName);  // here we used the stored customer name

    if (query.exec()) {
        while (query.next()) {
            int orderId = query.value(0).toInt();
            QString customerName = query.value(1).toString();
            QString phoneNumber = query.value(2).toString();
            double totalPrice = query.value(3).toDouble();
            QString orderDate = query.value(4).toString();

            QLabel *orderDetails = new QLabel(
                QString("Order ID: %1\nCustomer Name: %2\nPhone Number: %3\nTotal Price: $%4\nOrder Date: %5\n")
                .arg(orderId).arg(customerName).arg(phoneNumber).arg(totalPrice, 0, 'f', 2).arg(orderDate), 
                &profileDialog
            );
            orderDetails->setStyleSheet("padding: 10px; border: 1px solid #BDC3C7; margin-bottom: 10px; border-radius: 5px; background-color: #ECF0F1;");
            profileLayout->addWidget(orderDetails);
        }
    } else {
        QLabel *errorLabel = new QLabel("Error fetching order details: " + query.lastError().text(), &profileDialog);
        profileLayout->addWidget(errorLabel);
    }

    QPushButton *closeButton = new QPushButton("Close", &profileDialog);
    QString buttonStyle = 
        "QPushButton {"
        "   font-size: 16px;"
        "   padding: 10px 20px;"
        "   border-radius: 5px;"
        "   color: white;"
        "   background-color: #27AE60;"  
        "   margin: 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #229954;"
        "}";
    closeButton->setStyleSheet(buttonStyle);
    profileLayout->addWidget(closeButton);
    connect(closeButton, &QPushButton::clicked, &profileDialog, &QDialog::accept);

    profileDialog.exec();
}

bool RestaurantApp::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QWidget *widget = qobject_cast<QWidget*>(obj);
        if (widget) {
            QLayout *layout = widget->layout();
            if (layout) {
                QLabel *nameLabel = qobject_cast<QLabel*>(layout->itemAt(1)->widget());
                QLabel *priceLabel = qobject_cast<QLabel*>(layout->itemAt(2)->widget());
                if (nameLabel && priceLabel) {
                    handleItemClick(nameLabel->text(), priceLabel->text());
                }
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void RestaurantApp::handleItemClick(const QString &name, const QString &price) {
    bool ok;
    int quantity = QInputDialog::getInt(this, "Quantity", 
                                        "Enter quantity for " + name + ":", 
                                        1, 1, 100, 1, &ok);
    if (ok) {
        double itemPrice = price.mid(1).toDouble(); 
        double totalPrice = itemPrice * quantity;
        
        QListWidget *cartList = findChild<QListWidget*>();  // Finding the cart list widget
        if (cartList) {
            QString cartText = QString("%1 x%2 - Total: $%3")
                             .arg(name)
                             .arg(quantity)
                             .arg(totalPrice, 0, 'f', 2);
            QListWidgetItem *newItem = new QListWidgetItem(cartText);
            cartList->addItem(newItem);
        }
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Connection to the database
    if (!connectToDatabase()) {
        return -1;  // Exit if database connection fails
    }

    // Created Welcome Dialog and Main Application Instance
    WelcomeDialog welcomeDialog;

    // We Used a pointer for dynamic allocation to ensure proper lifetime
    RestaurantApp *restaurantApp = nullptr;

    // Showing our  Welcome Dialog
    if (welcomeDialog.exec() == QDialog::Accepted) {
        // Proceed to main restaurant app
        restaurantApp = new RestaurantApp();
        restaurantApp->show();
    }

    return app.exec();
}


//Thank you.