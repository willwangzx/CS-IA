#ifndef RBTREEVISUALIZER_H
#define RBTREEVISUALIZER_H

#include "LibraryManagementSystem.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <string>
#include <unordered_map>
#include <vector>

class RBTreeVisualizer {
public:
    explicit RBTreeVisualizer(const sf::Font& font);

    void refresh(const LibraryManagementSystem& library);
    void handleEvent(const sf::Event& event, const sf::Vector2f& mousePos,
                     LibraryManagementSystem& library);
    void update(const sf::Vector2f& mousePos);
    void draw(sf::RenderWindow& window);
    bool consumeBackRequested();

private:
    struct VisualButton {
        sf::RectangleShape shape;
        sf::Text label;
        sf::Color normalColor;
        sf::Color hoverColor;
        bool hovered = false;

        VisualButton(const sf::Font& font, const sf::Vector2f& position,
                     const sf::Vector2f& size, const std::string& text,
                     const sf::Color& normal, const sf::Color& hover);
        void update(const sf::Vector2f& mousePos);
        void draw(sf::RenderWindow& window);
        bool contains(const sf::Vector2f& mousePos) const;
    };

    struct VisualInput {
        sf::RectangleShape shape;
        sf::Text valueText;
        sf::Text placeholderText;
        std::string placeholder;
        std::string content;
        bool focused = false;

        VisualInput(const sf::Font& font, const sf::Vector2f& position,
                    const sf::Vector2f& size, const std::string& placeholderLabel);
        void draw(sf::RenderWindow& window);
        void handleText(const sf::Event& event);
        bool contains(const sf::Vector2f& mousePos) const;
        void setFocus(bool value);
        void clear();
    };

    using NodePositionMap = std::unordered_map<int, sf::Vector2f>;

    const sf::Font& font;
    RBTreeVisualSnapshot<Book> snapshot;
    std::vector<RBTreeTraceStep<Book>> trace;
    std::size_t currentStep = 0;
    bool playing = false;
    bool backRequested = false;
    int selectedNodeId = -1;
    std::string statusMessage;
    sf::Clock animationClock;
    float secondsPerStep = 0.8f;
    float pausedProgress = 0.f;

    std::vector<VisualInput> inputs;
    std::vector<VisualButton> buttons;
    NodePositionMap nodePositions;

    void layoutControls();
    void runAdd(LibraryManagementSystem& library);
    void runRemove(LibraryManagementSystem& library);
    void startTrace(const LibraryManagementSystem::TreeOperationResult& result,
                    const LibraryManagementSystem& library);
    void rebuildNodePositions(const RBTreeVisualSnapshot<Book>& activeSnapshot);
    NodePositionMap buildNodePositions(const RBTreeVisualSnapshot<Book>& activeSnapshot) const;
    int assignInorderPositions(const RBTreeVisualSnapshot<Book>& activeSnapshot,
                               int nodeId, int& nextColumn, int maxDepth,
                               const sf::FloatRect& canvasBounds,
                               NodePositionMap& positions) const;
    const RBTreeVisualSnapshot<Book>& activeSnapshot() const;
    const RBTreeTraceStep<Book>* activeStep() const;
    const RBTreeTraceStep<Book>* nextStep() const;
    float transitionRawProgress() const;
    float transitionProgress() const;
    std::string visualNodeKey(const RBTreeVisualNode<Book>& node) const;
    void drawToolbar(sf::RenderWindow& window);
    void drawCanvas(sf::RenderWindow& window, const RBTreeVisualSnapshot<Book>& activeSnapshot);
    void drawInspector(sf::RenderWindow& window, const RBTreeVisualSnapshot<Book>& activeSnapshot);
    void drawTimeline(sf::RenderWindow& window);
    void drawText(sf::RenderWindow& window, const std::string& text,
                  unsigned int size, const sf::Vector2f& position,
                  const sf::Color& color, bool bold = false);
    std::string stepKindLabel(RBTreeStepKind kind) const;
};

#endif
