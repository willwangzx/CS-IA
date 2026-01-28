#ifndef LIBRARYGUI_H
#define LIBRARYGUI_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include "LibraryManagementSystem.h"
#include <vector>
#include <string>
#include <memory>

// UI Component Classes
class Button {
private:
    sf::RectangleShape shape;
    sf::Text text;
    sf::Color normalColor;
    sf::Color hoverColor;
    sf::Color activeColor;
    bool isHovered;
    bool isActive;

public:
    Button(const sf::Vector2f& position, const sf::Vector2f& size, 
           const std::string& label, const sf::Font& font);
    
    void draw(sf::RenderWindow& window);
    void update(const sf::Vector2f& mousePos);
    bool isClicked(const sf::Vector2f& mousePos);
    void setActive(bool active);
    bool getActive() const { return isActive; }
};

class InputBox {
private:
    sf::RectangleShape shape;
    sf::Text text;
    sf::Text placeholder;
    std::string content;
    bool isFocused;
    sf::Clock cursorClock;
    bool showCursor;

public:
    InputBox(const sf::Vector2f& position, const sf::Vector2f& size, 
             const std::string& placeholderText, const sf::Font& font);
    
    void draw(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event);
    void setFocus(bool focus);
    bool getFocus() const { return isFocused; }
    std::string getContent() const { return content; }
    void clear();
    void setPosition(const sf::Vector2f& position);
    bool isClicked(const sf::Vector2f& mousePos);
};

class ScrollableList {
private:
    sf::RectangleShape background;
    std::vector<sf::Text> items;
    sf::Vector2f position;
    sf::Vector2f size;
    int scrollOffset;
    int maxVisibleItems;
    const sf::Font* font;

public:
    ScrollableList(const sf::Vector2f& position, const sf::Vector2f& size, 
                   const sf::Font& font);
    
    void draw(sf::RenderWindow& window);
    void addItem(const std::string& item);
    void clear();
    void scroll(int delta);
    void update(const sf::Vector2f& mousePos);
};

class MessageBox {
private:
    sf::RectangleShape background;
    sf::RectangleShape border;
    sf::Text message;
    bool visible;
    sf::Clock displayClock;
    float displayDuration;

public:
    MessageBox(const sf::Font& font);
    
    void show(const std::string& msg, float duration = 3.0f);
    void draw(sf::RenderWindow& window);
    void update();
    bool isVisible() const { return visible; }
};

// Main GUI Class
class LibraryGUI {
private:
    sf::RenderWindow window;
    LibraryManagementSystem library;
    sf::Font font;
    
    // UI State
    enum class Screen { MAIN_MENU, ADD_BOOK, REMOVE_BOOK, SEARCH_BOOK, 
                       CHECKOUT_BOOK, RETURN_BOOK, VIEW_ALL };
    Screen currentScreen;
    
    // UI Components
    std::vector<std::unique_ptr<Button>> mainMenuButtons;
    std::vector<std::unique_ptr<InputBox>> inputBoxes;
    std::unique_ptr<ScrollableList> bookList;
    std::unique_ptr<MessageBox> messageBox;
    std::unique_ptr<Button> backButton;
    std::unique_ptr<Button> submitButton;
    
    // Labels
    std::vector<sf::Text> labels;
    sf::Text titleText;
    sf::Text screenTitleText;
    
    // Background
    sf::RectangleShape background;
    sf::RectangleShape headerBar;
    
    // Helper methods
    void initializeMainMenu();
    void initializeAddBookScreen();
    void initializeRemoveBookScreen();
    void initializeSearchBookScreen();
    void initializeCheckoutBookScreen();
    void initializeReturnBookScreen();
    void initializeViewAllScreen();
    
    void handleMainMenuEvents(const sf::Event& event);
    void handleAddBookEvents(const sf::Event& event);
    void handleRemoveBookEvents(const sf::Event& event);
    void handleSearchBookEvents(const sf::Event& event);
    void handleCheckoutBookEvents(const sf::Event& event);
    void handleReturnBookEvents(const sf::Event& event);
    void handleViewAllEvents(const sf::Event& event);
    
    void renderMainMenu();
    void renderAddBookScreen();
    void renderRemoveBookScreen();
    void renderSearchBookScreen();
    void renderCheckoutBookScreen();
    void renderReturnBookScreen();
    void renderViewAllScreen();
    
    void clearInputBoxes();
    void setScreen(Screen screen);
    
    sf::Vector2f getMousePosition();
    void updateHoverStates();

public:
    LibraryGUI();
    ~LibraryGUI();
    
    void run();
    bool loadFont(const std::string& fontPath);
};

#endif
