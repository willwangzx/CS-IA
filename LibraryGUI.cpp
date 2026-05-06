#include "LibraryGUI.h"
#include <sstream>
#include <iomanip>

// Button Implementation
Button::Button(const sf::Vector2f& position, const sf::Vector2f& size, 
               const std::string& label, const sf::Font& font)
    : normalColor(sf::Color(70, 130, 180)), 
      hoverColor(sf::Color(100, 149, 237)),
      activeColor(sf::Color(65, 105, 225)),
      text(font),
      isHovered(false), isActive(false) {
    
    shape.setSize(size);
    shape.setPosition(position);
    shape.setFillColor(normalColor);
    shape.setOutlineThickness(2.f);
    shape.setOutlineColor(sf::Color(50, 50, 50));
    
    text.setString(label);
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::White);
    
    // Center text in button
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin(sf::Vector2f{textBounds.size.x / 2.f, textBounds.size.y / 2.f});
    text.setPosition(sf::Vector2f(position.x + size.x / 2.f, position.y + size.y / 2.f - 5.f));
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(text);
}

void Button::update(const sf::Vector2f& mousePos) {
    isHovered = shape.getGlobalBounds().contains(mousePos);
    
    if (isActive) {
        shape.setFillColor(activeColor);
    } else if (isHovered) {
        shape.setFillColor(hoverColor);
    } else {
        shape.setFillColor(normalColor);
    }
}

bool Button::isClicked(const sf::Vector2f& mousePos) {
    return shape.getGlobalBounds().contains(mousePos);
}

void Button::setActive(bool active) {
    isActive = active;
}

// InputBox Implementation
InputBox::InputBox(const sf::Vector2f& position, const sf::Vector2f& size, 
                   const std::string& placeholderText, const sf::Font& font)
    : text(font),
      placeholder(font),
      isFocused(false), showCursor(false) {
    
    shape.setSize(size);
    shape.setPosition(position);
    shape.setFillColor(sf::Color::White);
    shape.setOutlineThickness(2.f);
    shape.setOutlineColor(sf::Color(100, 100, 100));
    
    text.setCharacterSize(16);
    text.setFillColor(sf::Color::Black);
    text.setPosition(sf::Vector2f(position.x + 10.f, position.y + 10.f));
    
    placeholder.setString(placeholderText);
    placeholder.setCharacterSize(16);
    placeholder.setFillColor(sf::Color(150, 150, 150));
    placeholder.setPosition(sf::Vector2f(position.x + 10.f, position.y + 10.f));
}

void InputBox::draw(sf::RenderWindow& window) {
    window.draw(shape);
    
    if (content.empty() && !isFocused) {
        window.draw(placeholder);
    } else {
        std::string displayText = content;
        if (isFocused && showCursor) {
            displayText += "|";
        }
        text.setString(displayText);
        window.draw(text);
    }
}

void InputBox::handleEvent(const sf::Event& event) {
    if (!isFocused) return;
    
    if (event.is<sf::Event::TextEntered>()) {
        if (event.text.code == 8) { // Backspace
            if (!content.empty()) {
                content.pop_back();
            }
        } else if (event.text.code == 13) { // Enter
            // Enter handled by parent
        } else if (event.text.code < 128 && event.text.code >= 32) {
            content += static_cast<char>(event.text.code);
        }
    }
    
    // Cursor blink
    if (cursorClock.getElapsedTime().asSeconds() > 0.5f) {
        showCursor = !showCursor;
        cursorClock.restart();
    }
}

void InputBox::setFocus(bool focus) {
    isFocused = focus;
    if (focus) {
        shape.setOutlineColor(sf::Color(70, 130, 180));
        cursorClock.restart();
        showCursor = true;
    } else {
        shape.setOutlineColor(sf::Color(100, 100, 100));
        showCursor = false;
    }
}

bool InputBox::isClicked(const sf::Vector2f& mousePos) {
    return shape.getGlobalBounds().contains(mousePos);
}

void InputBox::clear() {
    content.clear();
}

void InputBox::setPosition(const sf::Vector2f& position) {
    shape.setPosition(position);
    text.setPosition(sf::Vector2f(position.x + 10.f, position.y + 10.f));
    placeholder.setPosition(sf::Vector2f(position.x + 10.f, position.y + 10.f));
}

// ScrollableList Implementation
ScrollableList::ScrollableList(const sf::Vector2f& pos, const sf::Vector2f& sz, 
                               const sf::Font& f)
    : position(pos), size(sz), scrollOffset(0), maxVisibleItems(10), font(&f) {
    
    background.setSize(size);
    background.setPosition(position);
    background.setFillColor(sf::Color::White);
    background.setOutlineThickness(2.f);
    background.setOutlineColor(sf::Color(100, 100, 100));
}

void ScrollableList::draw(sf::RenderWindow& window) {
    window.draw(background);
    
    int startIndex = scrollOffset;
    int endIndex = std::min(startIndex + maxVisibleItems, static_cast<int>(items.size()));
    
    for (int i = startIndex; i < endIndex; ++i) {
        sf::Text itemText = items[i];
        itemText.setPosition(sf::Vector2f(position.x + 10.f, 
                            position.y + 10.f + (i - startIndex) * 25.f));
        window.draw(itemText);
    }
}

void ScrollableList::addItem(const std::string& item) {
    sf::Text newItem(*font);
    newItem.setString(item);
    newItem.setCharacterSize(14);
    newItem.setFillColor(sf::Color::Black);
    items.push_back(newItem);
}

void ScrollableList::clear() {
    items.clear();
    scrollOffset = 0;
}

void ScrollableList::scroll(int delta) {
    scrollOffset += delta;
    scrollOffset = std::max(0, scrollOffset);
    scrollOffset = std::min(scrollOffset, 
                           std::max(0, static_cast<int>(items.size()) - maxVisibleItems));
}

void ScrollableList::update(const sf::Vector2f& mousePos) {
    // Can add hover effects here if needed
}

// MessageBox Implementation
MessageBox::MessageBox(const sf::Font& font)
    : message(font),
      visible(false), displayDuration(3.0f) {
    
    background.setSize(sf::Vector2f(400.f, 80.f));
    background.setFillColor(sf::Color(240, 240, 240, 230));
    background.setPosition(sf::Vector2f(250.f, 500.f));
    
    border.setSize(sf::Vector2f(400.f, 80.f));
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineThickness(2.f);
    border.setOutlineColor(sf::Color(70, 130, 180));
    border.setPosition(sf::Vector2f(250.f, 500.f));
    
    message.setCharacterSize(16);
    message.setFillColor(sf::Color::Black);
    message.setPosition(sf::Vector2f(270.f, 520.f));
}

void MessageBox::show(const std::string& msg, float duration) {
    message.setString(msg);
    displayDuration = duration;
    visible = true;
    displayClock.restart();
}

void MessageBox::draw(sf::RenderWindow& window) {
    if (visible) {
        window.draw(background);
        window.draw(border);
        window.draw(message);
    }
}

void MessageBox::update() {
    if (visible && displayClock.getElapsedTime().asSeconds() > displayDuration) {
        visible = false;
    }
}

// LibraryGUI Implementation
LibraryGUI::LibraryGUI()
    : window(sf::VideoMode(900u, 700u), "Library Management System", sf::Style::Close),
      currentScreen(Screen::MAIN_MENU) {
    
    window.setFramerateLimit(60);
    
    // Initialize background
    background.setSize(sf::Vector2f(900.f, 700.f));
    background.setFillColor(sf::Color(245, 245, 245));
    
    // Initialize header bar
    headerBar.setSize(sf::Vector2f(900.f, 80.f));
    headerBar.setFillColor(sf::Color(70, 130, 180));
}

LibraryGUI::~LibraryGUI() {
}

bool LibraryGUI::loadFont(const std::string& fontPath) {
    try {
        font = sf::Font(fontPath);
    } catch (...) {
        return false;
    }
    
    // Initialize title text
    titleText.setFont(font);
    titleText.setString("Library Management System");
    titleText.setCharacterSize(32);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition(sf::Vector2f(200.f, 20.f));
    
    // Initialize screen title
    screenTitleText.setFont(font);
    screenTitleText.setCharacterSize(24);
    screenTitleText.setFillColor(sf::Color::Black);
    screenTitleText.setStyle(sf::Text::Bold);
    
    // Initialize message box
    messageBox = std::make_unique<MessageBox>(font);
    
    // Initialize main menu
    initializeMainMenu();
    
    return true;
}

void LibraryGUI::initializeMainMenu() {
    mainMenuButtons.clear();
    
    float startY = 150.f;
    float spacing = 70.f;
    
    mainMenuButtons.push_back(std::make_unique<Button>(
        sf::Vector2f(300.f, startY), sf::Vector2f(300.f, 50.f), 
        "Add Book", font));
    
    mainMenuButtons.push_back(std::make_unique<Button>(
        sf::Vector2f(300.f, startY + spacing), sf::Vector2f(300.f, 50.f), 
        "Remove Book", font));
    
    mainMenuButtons.push_back(std::make_unique<Button>(
        sf::Vector2f(300.f, startY + spacing * 2), sf::Vector2f(300.f, 50.f), 
        "Search Book", font));
    
    mainMenuButtons.push_back(std::make_unique<Button>(
        sf::Vector2f(300.f, startY + spacing * 3), sf::Vector2f(300.f, 50.f), 
        "Checkout Book", font));
    
    mainMenuButtons.push_back(std::make_unique<Button>(
        sf::Vector2f(300.f, startY + spacing * 4), sf::Vector2f(300.f, 50.f), 
        "Return Book", font));
    
    mainMenuButtons.push_back(std::make_unique<Button>(
        sf::Vector2f(300.f, startY + spacing * 5), sf::Vector2f(300.f, 50.f), 
        "View All Books", font));
}

void LibraryGUI::initializeAddBookScreen() {
    inputBoxes.clear();
    labels.clear();
    
    screenTitleText.setString("Add New Book");
    screenTitleText.setPosition(sf::Vector2f(350.f, 100.f));
    
    // Create labels
    std::vector<std::string> labelTexts = {"ISBN:", "Title:", "Author:", "Year:"};
    float startY = 180.f;
    float spacing = 80.f;
    
    for (size_t i = 0; i < labelTexts.size(); ++i) {
        sf::Text label(font);
        label.setString(labelTexts[i]);
        label.setCharacterSize(18);
        label.setFillColor(sf::Color::Black);
        label.setPosition(sf::Vector2f(200.f, startY + i * spacing));
        labels.push_back(label);
    }
    
    // Create input boxes
    std::vector<std::string> placeholders = {"Enter ISBN", "Enter Title", "Enter Author", "Enter Year"};
    
    for (size_t i = 0; i < placeholders.size(); ++i) {
        inputBoxes.push_back(std::make_unique<InputBox>(
            sf::Vector2f(300.f, startY + i * spacing), 
            sf::Vector2f(400.f, 40.f), 
            placeholders[i], font));
    }
    
    // Create buttons
    backButton = std::make_unique<Button>(
        sf::Vector2f(200.f, 600.f), sf::Vector2f(150.f, 50.f), 
        "Back", font);
    
    submitButton = std::make_unique<Button>(
        sf::Vector2f(550.f, 600.f), sf::Vector2f(150.f, 50.f), 
        "Add Book", font);
}

void LibraryGUI::initializeRemoveBookScreen() {
    inputBoxes.clear();
    labels.clear();
    
    screenTitleText.setString("Remove Book");
    screenTitleText.setPosition(sf::Vector2f(350.f, 100.f));
    
    // Create label
    sf::Text label(font);
    label.setString("ISBN:");
    label.setCharacterSize(18);
    label.setFillColor(sf::Color::Black);
    label.setPosition(sf::Vector2f(200.f, 250.f));
    labels.push_back(label);
    
    // Create input box
    inputBoxes.push_back(std::make_unique<InputBox>(
        sf::Vector2f(300.f, 250.f), sf::Vector2f(400.f, 40.f), 
        "Enter ISBN to remove", font));
    
    // Create buttons
    backButton = std::make_unique<Button>(
        sf::Vector2f(200.f, 600.f), sf::Vector2f(150.f, 50.f), 
        "Back", font);
    
    submitButton = std::make_unique<Button>(
        sf::Vector2f(550.f, 600.f), sf::Vector2f(150.f, 50.f), 
        "Remove", font);
}

void LibraryGUI::initializeSearchBookScreen() {
    inputBoxes.clear();
    labels.clear();
    
    screenTitleText.setString("Search Book");
    screenTitleText.setPosition(sf::Vector2f(350.f, 100.f));
    
    // Create label
    sf::Text label(font);
    label.setString("ISBN:");
    label.setCharacterSize(18);
    label.setFillColor(sf::Color::Black);
    label.setPosition(sf::Vector2f(200.f, 200.f));
    labels.push_back(label);
    
    // Create input box
    inputBoxes.push_back(std::make_unique<InputBox>(
        sf::Vector2f(300.f, 200.f), sf::Vector2f(400.f, 40.f), 
        "Enter ISBN to search", font));
    
    // Create result display area
    bookList = std::make_unique<ScrollableList>(
        sf::Vector2f(150.f, 280.f), sf::Vector2f(600.f, 250.f), font);
    
    // Create buttons
    backButton = std::make_unique<Button>(
        sf::Vector2f(200.f, 600.f), sf::Vector2f(150.f, 50.f), 
        "Back", font);
    
    submitButton = std::make_unique<Button>(
        sf::Vector2f(550.f, 600.f), sf::Vector2f(150.f, 50.f), 
        "Search", font);
}

void LibraryGUI::initializeCheckoutBookScreen() {
    inputBoxes.clear();
    labels.clear();
    
    screenTitleText.setString("Checkout Book");
    screenTitleText.setPosition(sf::Vector2f(330.f, 100.f));
    
    // Create label
    sf::Text label(font);
    label.setString("ISBN:");
    label.setCharacterSize(18);
    label.setFillColor(sf::Color::Black);
    label.setPosition(sf::Vector2f(200.f, 250.f));
    labels.push_back(label);
    
    // Create input box
    inputBoxes.push_back(std::make_unique<InputBox>(
        sf::Vector2f(300.f, 250.f), sf::Vector2f(400.f, 40.f), 
        "Enter ISBN to checkout", font));
    
    // Create buttons
    backButton = std::make_unique<Button>(
        sf::Vector2f(200.f, 600.f), sf::Vector2f(150.f, 50.f), 
        "Back", font);
    
    submitButton = std::make_unique<Button>(
        sf::Vector2f(550.f, 600.f), sf::Vector2f(150.f, 50.f), 
        "Checkout", font);
}

void LibraryGUI::initializeReturnBookScreen() {
    inputBoxes.clear();
    labels.clear();
    
    screenTitleText.setString("Return Book");
    screenTitleText.setPosition(sf::Vector2f(350.f, 100.f));
    
    // Create label
    sf::Text label(font);
    label.setString("ISBN:");
    label.setCharacterSize(18);
    label.setFillColor(sf::Color::Black);
    label.setPosition(sf::Vector2f(200.f, 250.f));
    labels.push_back(label);
    
    // Create input box
    inputBoxes.push_back(std::make_unique<InputBox>(
        sf::Vector2f(300.f, 250.f), sf::Vector2f(400.f, 40.f), 
        "Enter ISBN to return", font));
    
    // Create buttons
    backButton = std::make_unique<Button>(
        sf::Vector2f(200.f, 600.f), sf::Vector2f(150.f, 50.f), 
        "Back", font);
    
    submitButton = std::make_unique<Button>(
        sf::Vector2f(550.f, 600.f), sf::Vector2f(150.f, 50.f), 
        "Return", font);
}

void LibraryGUI::initializeViewAllScreen() {
    screenTitleText.setString("All Books in Library");
    screenTitleText.setPosition(sf::Vector2f(300.f, 100.f));

    // Create scrollable list
    bookList = std::make_unique<ScrollableList>(
        sf::Vector2f(50.f, 160.f), sf::Vector2f(800.f, 450.f), font);

    bookList->clear();
    // Get all books from library and add to list
    library.forEachBook([this](const Book& book) {
        std::cout << "Adding to list: " << book.getTitle() << std::endl;
        std::stringstream ss;
        ss << "ISBN: " << book.getISBN()
           << " | " << book.getTitle()
           << " | " << book.getAuthor()
           << " | " << book.getYear()
           << " | " << (book.getAvailability() ? "Available" : "Checked Out");
        bookList->addItem(ss.str());
    });

    // Create back button
    backButton = std::make_unique<Button>(
        sf::Vector2f(375.f, 630.f), sf::Vector2f(150.f, 50.f),
        "Back", font);
}

void LibraryGUI::handleMainMenuEvents(const sf::Event& event) {
    if (event.is<sf::Event::MouseButtonPressed>()) {
        const auto& mouseEvent = event.get<sf::Event::MouseButtonPressed>();
        if (mouseEvent.button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos = getMousePosition();
            
            for (size_t i = 0; i < mainMenuButtons.size(); ++i) {
                if (mainMenuButtons[i]->isClicked(mousePos)) {
                    switch (i) {
                        case 0: setScreen(Screen::ADD_BOOK); break;
                        case 1: setScreen(Screen::REMOVE_BOOK); break;
                        case 2: setScreen(Screen::SEARCH_BOOK); break;
                        case 3: setScreen(Screen::CHECKOUT_BOOK); break;
                        case 4: setScreen(Screen::RETURN_BOOK); break;
                        case 5: setScreen(Screen::VIEW_ALL); break;
                    }
                }
            }
        }
    }
}

void LibraryGUI::handleAddBookEvents(const sf::Event& event) {
    // Handle input box focus
    if (event.is<sf::Event::MouseButtonPressed>()) {
        const auto& mouseEvent = event.get<sf::Event::MouseButtonPressed>();
        if (mouseEvent.button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos = getMousePosition();
            
            // Check input boxes
            for (auto& inputBox : inputBoxes) {
                if (inputBox->getFocus()) {
                    inputBox->setFocus(false);
                }
            }
            
            for (auto& inputBox : inputBoxes) {
                if (inputBox->isClicked(mousePos)) {
                    inputBox->setFocus(true);
                    break;
                }
            }
            
            // Check back button
            if (backButton->isClicked(mousePos)) {
                setScreen(Screen::MAIN_MENU);
            }
            
            // Check submit button
            if (submitButton->isClicked(mousePos)) {
                if (inputBoxes.size() >= 4) {
                    try {
                        int isbn = std::stoi(inputBoxes[0]->getContent());
                        std::string title = inputBoxes[1]->getContent();
                        std::string author = inputBoxes[2]->getContent();
                        int year = std::stoi(inputBoxes[3]->getContent());
                        
                        library.addBook(isbn, title, author, year);
                        messageBox->show("Book added successfully!");
                        clearInputBoxes();
                    } catch (...) {
                        messageBox->show("Invalid input! Please check all fields.");
                    }
                }
            }
        }
    }
    
    // Handle text input
    for (auto& inputBox : inputBoxes) {
        inputBox->handleEvent(event);
    }
}

void LibraryGUI::handleRemoveBookEvents(const sf::Event& event) {
    if (event.is<sf::Event::MouseButtonPressed>()) {
        const auto& mouseEvent = event.get<sf::Event::MouseButtonPressed>();
        if (mouseEvent.button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos = getMousePosition();
            
            // Handle input focus
            if (inputBoxes[0]->getFocus()) {
                inputBoxes[0]->setFocus(false);
            }
            
            if (inputBoxes[0]->isClicked(mousePos)) {
                inputBoxes[0]->setFocus(true);
            }
            
            if (backButton->isClicked(mousePos)) {
                setScreen(Screen::MAIN_MENU);
            }
            
            if (submitButton->isClicked(mousePos)) {
                try {
                    int isbn = std::stoi(inputBoxes[0]->getContent());
                    library.removeBook(isbn);
                    messageBox->show("Book removed successfully!");
                    clearInputBoxes();
                } catch (...) {
                    messageBox->show("Invalid ISBN!");
                }
            }
        }
    }
    
    inputBoxes[0]->handleEvent(event);
}

void LibraryGUI::handleSearchBookEvents(const sf::Event& event) {
    if (event.is<sf::Event::MouseButtonPressed>()) {
        const auto& mouseEvent = event.get<sf::Event::MouseButtonPressed>();
        if (mouseEvent.button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos = getMousePosition();
            
            if (inputBoxes[0]->getFocus()) {
                inputBoxes[0]->setFocus(false);
            }
            
            if (inputBoxes[0]->isClicked(mousePos)) {
                inputBoxes[0]->setFocus(true);
            }
            
            if (backButton->isClicked(mousePos)) {
                setScreen(Screen::MAIN_MENU);
            }
            
            if (submitButton->isClicked(mousePos)) {
                try {
                    int isbn = std::stoi(inputBoxes[0]->getContent());
                    Book* book = library.findBook(isbn);
                    
                    bookList->clear();
                    if (book != nullptr) {
                        std::stringstream ss;
                        ss << "ISBN: " << book->getISBN() 
                           << " | Title: " << book->getTitle()
                           << " | Author: " << book->getAuthor()
                           << " | Year: " << book->getYear()
                           << " | Status: " << (book->getAvailability() ? "Available" : "Checked Out");
                        bookList->addItem(ss.str());
                        messageBox->show("Book found!");
                    } else {
                        bookList->addItem("Book not found.");
                        messageBox->show("Book not found!");
                    }
                } catch (...) {
                    messageBox->show("Invalid ISBN!");
                }
            }
        }
    }
    
    inputBoxes[0]->handleEvent(event);
}

void LibraryGUI::handleCheckoutBookEvents(const sf::Event& event) {
    if (event.is<sf::Event::MouseButtonPressed>()) {
        const auto& mouseEvent = event.get<sf::Event::MouseButtonPressed>();
        if (mouseEvent.button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos = getMousePosition();
            
            if (inputBoxes[0]->getFocus()) {
                inputBoxes[0]->setFocus(false);
            }
            
            if (inputBoxes[0]->isClicked(mousePos)) {
                inputBoxes[0]->setFocus(true);
            }
            
            if (backButton->isClicked(mousePos)) {
                setScreen(Screen::MAIN_MENU);
            }
            
            if (submitButton->isClicked(mousePos)) {
                try {
                    int isbn = std::stoi(inputBoxes[0]->getContent());
                    library.checkoutBook(isbn);
                    messageBox->show("Book checked out successfully!");
                    clearInputBoxes();
                } catch (...) {
                    messageBox->show("Invalid ISBN!");
                }
            }
        }
    }
    
    inputBoxes[0]->handleEvent(event);
}

void LibraryGUI::handleReturnBookEvents(const sf::Event& event) {
    if (event.is<sf::Event::MouseButtonPressed>()) {
        const auto& mouseEvent = event.get<sf::Event::MouseButtonPressed>();
        if (mouseEvent.button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos = getMousePosition();
            
            if (inputBoxes[0]->getFocus()) {
                inputBoxes[0]->setFocus(false);
            }
            
            if (inputBoxes[0]->isClicked(mousePos)) {
                inputBoxes[0]->setFocus(true);
            }
            
            if (backButton->isClicked(mousePos)) {
                setScreen(Screen::MAIN_MENU);
            }
            
            if (submitButton->isClicked(mousePos)) {
                try {
                    int isbn = std::stoi(inputBoxes[0]->getContent());
                    library.returnBook(isbn);
                    messageBox->show("Book returned successfully!");
                    clearInputBoxes();
                } catch (...) {
                    messageBox->show("Invalid ISBN!");
                }
            }
        }
    }
    
    inputBoxes[0]->handleEvent(event);
}

void LibraryGUI::handleViewAllEvents(const sf::Event& event) {
    if (event.is<sf::Event::MouseButtonPressed>()) {
        const auto& mouseEvent = event.get<sf::Event::MouseButtonPressed>();
        if (mouseEvent.button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos = getMousePosition();
            
            if (backButton->isClicked(mousePos)) {
                setScreen(Screen::MAIN_MENU);
            }
        }
    }
    
    // Handle scrolling
    if (event.is<sf::Event::MouseWheelMoved>()) {
        const auto& wheelEvent = event.get<sf::Event::MouseWheelMoved>();
        bookList->scroll(-static_cast<int>(wheelEvent.delta));
    }
}

void LibraryGUI::renderMainMenu() {
    for (auto& button : mainMenuButtons) {
        button->draw(window);
    }
}

void LibraryGUI::renderAddBookScreen() {
    window.draw(screenTitleText);
    
    for (auto& label : labels) {
        window.draw(label);
    }
    
    for (auto& inputBox : inputBoxes) {
        inputBox->draw(window);
    }
    
    backButton->draw(window);
    submitButton->draw(window);
}

void LibraryGUI::renderRemoveBookScreen() {
    window.draw(screenTitleText);
    
    for (auto& label : labels) {
        window.draw(label);
    }
    
    for (auto& inputBox : inputBoxes) {
        inputBox->draw(window);
    }
    
    backButton->draw(window);
    submitButton->draw(window);
}

void LibraryGUI::renderSearchBookScreen() {
    window.draw(screenTitleText);
    
    for (auto& label : labels) {
        window.draw(label);
    }
    
    for (auto& inputBox : inputBoxes) {
        inputBox->draw(window);
    }
    
    if (bookList) {
        bookList->draw(window);
    }
    
    backButton->draw(window);
    submitButton->draw(window);
}

void LibraryGUI::renderCheckoutBookScreen() {
    window.draw(screenTitleText);
    
    for (auto& label : labels) {
        window.draw(label);
    }
    
    for (auto& inputBox : inputBoxes) {
        inputBox->draw(window);
    }
    
    backButton->draw(window);
    submitButton->draw(window);
}

void LibraryGUI::renderReturnBookScreen() {
    window.draw(screenTitleText);
    
    for (auto& label : labels) {
        window.draw(label);
    }
    
    for (auto& inputBox : inputBoxes) {
        inputBox->draw(window);
    }
    
    backButton->draw(window);
    submitButton->draw(window);
}

void LibraryGUI::renderViewAllScreen() {
    window.draw(screenTitleText);
    
    if (bookList) {
        bookList->draw(window);
    }
    
    backButton->draw(window);
}

void LibraryGUI::clearInputBoxes() {
    for (auto& inputBox : inputBoxes) {
        inputBox->clear();
    }
}

void LibraryGUI::setScreen(Screen screen) {
    currentScreen = screen;
    
    switch (screen) {
        case Screen::MAIN_MENU:
            initializeMainMenu();
            break;
        case Screen::ADD_BOOK:
            initializeAddBookScreen();
            break;
        case Screen::REMOVE_BOOK:
            initializeRemoveBookScreen();
            break;
        case Screen::SEARCH_BOOK:
            initializeSearchBookScreen();
            break;
        case Screen::CHECKOUT_BOOK:
            initializeCheckoutBookScreen();
            break;
        case Screen::RETURN_BOOK:
            initializeReturnBookScreen();
            break;
        case Screen::VIEW_ALL:
            initializeViewAllScreen();
            break;
    }
}

sf::Vector2f LibraryGUI::getMousePosition() {
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    return window.mapPixelToCoords(pixelPos);
}

void LibraryGUI::updateHoverStates() {
    sf::Vector2f mousePos = getMousePosition();
    
    // Update main menu buttons
    for (auto& button : mainMenuButtons) {
        button->update(mousePos);
    }
    
    // Update other buttons
    if (backButton) {
        backButton->update(mousePos);
    }
    
    if (submitButton) {
        submitButton->update(mousePos);
    }
}

void LibraryGUI::run() {
    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            
            // Handle events based on current screen
            switch (currentScreen) {
                case Screen::MAIN_MENU:
                    handleMainMenuEvents(*event);
                    break;
                case Screen::ADD_BOOK:
                    handleAddBookEvents(*event);
                    break;
                case Screen::REMOVE_BOOK:
                    handleRemoveBookEvents(*event);
                    break;
                case Screen::SEARCH_BOOK:
                    handleSearchBookEvents(*event);
                    break;
                case Screen::CHECKOUT_BOOK:
                    handleCheckoutBookEvents(*event);
                    break;
                case Screen::RETURN_BOOK:
                    handleReturnBookEvents(*event);
                    break;
                case Screen::VIEW_ALL:
                    handleViewAllEvents(*event);
                    break;
            }
        }
        
        // Update
        updateHoverStates();
        messageBox->update();
        
        // Clear window
        window.clear(sf::Color::White);
        
        // Draw background
        window.draw(background);
        window.draw(headerBar);
        window.draw(titleText);
        
        // Render current screen
        switch (currentScreen) {
            case Screen::MAIN_MENU:
                renderMainMenu();
                break;
            case Screen::ADD_BOOK:
                renderAddBookScreen();
                break;
            case Screen::REMOVE_BOOK:
                renderRemoveBookScreen();
                break;
            case Screen::SEARCH_BOOK:
                renderSearchBookScreen();
                break;
            case Screen::CHECKOUT_BOOK:
                renderCheckoutBookScreen();
                break;
            case Screen::RETURN_BOOK:
                renderReturnBookScreen();
                break;
            case Screen::VIEW_ALL:
                renderViewAllScreen();
                break;
        }
        
        // Draw message box on top
        messageBox->draw(window);
        
        window.display();
    }
}
