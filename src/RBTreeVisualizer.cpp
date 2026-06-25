#include "RBTreeVisualizer.h"

#include <algorithm>
#include <cmath>
#include <cctype>
#include <cstdint>
#include <sstream>

namespace {
const sf::FloatRect CanvasBounds({25.f, 155.f}, {590.f, 430.f});
const sf::FloatRect InspectorBounds({630.f, 155.f}, {245.f, 430.f});
const sf::FloatRect TimelineBounds({25.f, 610.f}, {590.f, 55.f});

sf::Color headerBlue() { return sf::Color(70, 130, 180); }
sf::Color borderGray() { return sf::Color(140, 150, 160); }
sf::Color canvasWhite() { return sf::Color(252, 253, 255); }
sf::Color mutedText() { return sf::Color(92, 100, 112); }

bool isBlank(const std::string& value) {
    return std::all_of(value.begin(), value.end(), [](unsigned char ch) {
        return std::isspace(ch) != 0;
    });
}

std::string trimForPanel(const std::string& value, std::size_t maxLength) {
    if (value.size() <= maxLength) {
        return value;
    }
    return value.substr(0, maxLength > 3 ? maxLength - 3 : maxLength) + "...";
}

void centerText(sf::Text& text, const sf::Vector2f& center) {
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x / 2.f,
                                bounds.position.y + bounds.size.y / 2.f));
    text.setPosition(center);
}

void drawLine(sf::RenderWindow& window, const sf::Vector2f& from,
              const sf::Vector2f& to, const sf::Color& color, float thickness) {
    const sf::Vector2f delta(to.x - from.x, to.y - from.y);
    const float length = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    if (length <= 0.01f) {
        return;
    }

    sf::RectangleShape line(sf::Vector2f(length, thickness));
    line.setFillColor(color);
    line.setOrigin(sf::Vector2f(0.f, thickness / 2.f));
    line.setPosition(from);
    line.setRotation(sf::radians(std::atan2(delta.y, delta.x)));
    window.draw(line);
}

float clamp01(float value) {
    return std::max(0.f, std::min(1.f, value));
}

float easeInOut(float value) {
    const float t = clamp01(value);
    return t * t * (3.f - 2.f * t);
}

sf::Vector2f lerp(const sf::Vector2f& from, const sf::Vector2f& to, float t) {
    return sf::Vector2f(from.x + (to.x - from.x) * t,
                        from.y + (to.y - from.y) * t);
}

sf::Color withOpacity(sf::Color color, float opacity) {
    color.a = static_cast<std::uint8_t>(
        std::round(static_cast<float>(color.a) * clamp01(opacity)));
    return color;
}

sf::Color blendColor(const sf::Color& from, const sf::Color& to, float t) {
    const float eased = clamp01(t);
    auto blendChannel = [eased](std::uint8_t a, std::uint8_t b) {
        return static_cast<std::uint8_t>(
            std::round(static_cast<float>(a) +
                       (static_cast<float>(b) - static_cast<float>(a)) * eased));
    };
    return sf::Color(blendChannel(from.r, to.r),
                     blendChannel(from.g, to.g),
                     blendChannel(from.b, to.b),
                     blendChannel(from.a, to.a));
}

sf::Color nodeFillColor(Color color) {
    return color == RED ? sf::Color(190, 47, 55) : sf::Color(30, 35, 42);
}
}

RBTreeVisualizer::VisualButton::VisualButton(
    const sf::Font& font, const sf::Vector2f& position, const sf::Vector2f& size,
    const std::string& text, const sf::Color& normal, const sf::Color& hover)
    : label(font), normalColor(normal), hoverColor(hover) {
    shape.setPosition(position);
    shape.setSize(size);
    shape.setFillColor(normalColor);
    shape.setOutlineThickness(1.f);
    shape.setOutlineColor(sf::Color(55, 72, 92));

    label.setString(text);
    label.setCharacterSize(13);
    label.setFillColor(sf::Color::White);
    label.setStyle(sf::Text::Bold);
    centerText(label, sf::Vector2f(position.x + size.x / 2.f,
                                   position.y + size.y / 2.f));
}

void RBTreeVisualizer::VisualButton::update(const sf::Vector2f& mousePos) {
    hovered = contains(mousePos);
    shape.setFillColor(hovered ? hoverColor : normalColor);
}

void RBTreeVisualizer::VisualButton::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(label);
}

bool RBTreeVisualizer::VisualButton::contains(const sf::Vector2f& mousePos) const {
    return shape.getGlobalBounds().contains(mousePos);
}

RBTreeVisualizer::VisualInput::VisualInput(
    const sf::Font& font, const sf::Vector2f& position, const sf::Vector2f& size,
    const std::string& placeholderLabel)
    : valueText(font), placeholderText(font), placeholder(placeholderLabel) {
    shape.setPosition(position);
    shape.setSize(size);
    shape.setFillColor(sf::Color::White);
    shape.setOutlineThickness(1.f);
    shape.setOutlineColor(borderGray());

    valueText.setCharacterSize(13);
    valueText.setFillColor(sf::Color(25, 30, 36));
    valueText.setPosition(sf::Vector2f(position.x + 8.f, position.y + 8.f));

    placeholderText.setString(placeholder);
    placeholderText.setCharacterSize(13);
    placeholderText.setFillColor(sf::Color(132, 140, 150));
    placeholderText.setPosition(sf::Vector2f(position.x + 8.f, position.y + 8.f));
}

void RBTreeVisualizer::VisualInput::draw(sf::RenderWindow& window) {
    window.draw(shape);
    if (content.empty()) {
        window.draw(placeholderText);
    } else {
        valueText.setString(trimForPanel(content, 18));
        window.draw(valueText);
    }
}

void RBTreeVisualizer::VisualInput::handleText(const sf::Event& event) {
    if (!focused) {
        return;
    }
    if (const auto* textEvent = event.getIf<sf::Event::TextEntered>()) {
        const char32_t unicode = textEvent->unicode;
        if (unicode == 8) {
            if (!content.empty()) {
                content.pop_back();
            }
        } else if (unicode >= 32 && unicode < 128) {
            content += static_cast<char>(unicode);
        }
    }
}

bool RBTreeVisualizer::VisualInput::contains(const sf::Vector2f& mousePos) const {
    return shape.getGlobalBounds().contains(mousePos);
}

void RBTreeVisualizer::VisualInput::setFocus(bool value) {
    focused = value;
    shape.setOutlineColor(focused ? headerBlue() : borderGray());
    shape.setOutlineThickness(focused ? 2.f : 1.f);
}

void RBTreeVisualizer::VisualInput::clear() {
    content.clear();
}

RBTreeVisualizer::RBTreeVisualizer(const sf::Font& fontRef) : font(fontRef) {
    layoutControls();
    statusMessage = "Ready. Add or remove a book to animate the real tree.";
}

void RBTreeVisualizer::layoutControls() {
    inputs.clear();
    buttons.clear();

    inputs.emplace_back(font, sf::Vector2f(25.f, 98.f), sf::Vector2f(88.f, 34.f), "ISBN");
    inputs.emplace_back(font, sf::Vector2f(121.f, 98.f), sf::Vector2f(170.f, 34.f), "Title");
    inputs.emplace_back(font, sf::Vector2f(299.f, 98.f), sf::Vector2f(155.f, 34.f), "Author");
    inputs.emplace_back(font, sf::Vector2f(462.f, 98.f), sf::Vector2f(70.f, 34.f), "Year");

    const sf::Color normal = headerBlue();
    const sf::Color hover(95, 153, 216);
    buttons.emplace_back(font, sf::Vector2f(542.f, 98.f), sf::Vector2f(120.f, 34.f),
                         "Add & Animate", normal, hover);
    buttons.emplace_back(font, sf::Vector2f(670.f, 98.f), sf::Vector2f(135.f, 34.f),
                         "Remove & Animate", normal, hover);
    buttons.emplace_back(font, sf::Vector2f(815.f, 98.f), sf::Vector2f(60.f, 34.f),
                         "Back", sf::Color(90, 98, 108), sf::Color(112, 122, 135));

    buttons.emplace_back(font, sf::Vector2f(25.f, 134.f), sf::Vector2f(92.f, 28.f),
                         "Play/Pause", sf::Color(48, 111, 176), hover);
    buttons.emplace_back(font, sf::Vector2f(125.f, 134.f), sf::Vector2f(72.f, 28.f),
                         "Step", sf::Color(48, 111, 176), hover);
    buttons.emplace_back(font, sf::Vector2f(205.f, 134.f), sf::Vector2f(72.f, 28.f),
                         "Reset", sf::Color(90, 98, 108), sf::Color(112, 122, 135));
    buttons.emplace_back(font, sf::Vector2f(285.f, 134.f), sf::Vector2f(58.f, 28.f),
                         "Fit", sf::Color(90, 98, 108), sf::Color(112, 122, 135));
    buttons.emplace_back(font, sf::Vector2f(351.f, 134.f), sf::Vector2f(78.f, 28.f),
                         "Refresh", sf::Color(90, 98, 108), sf::Color(112, 122, 135));
}

void RBTreeVisualizer::refresh(const LibraryManagementSystem& library) {
    snapshot = library.getTreeVisualizationSnapshot();
    if (trace.empty()) {
        currentStep = 0;
    }
}

void RBTreeVisualizer::handleEvent(const sf::Event& event, const sf::Vector2f& mousePos,
                                   LibraryManagementSystem& library) {
    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseEvent->button == sf::Mouse::Button::Left) {
            for (auto& input : inputs) {
                input.setFocus(input.contains(mousePos));
            }

            if (buttons[0].contains(mousePos)) {
                runAdd(library);
            } else if (buttons[1].contains(mousePos)) {
                runRemove(library);
            } else if (buttons[2].contains(mousePos)) {
                backRequested = true;
            } else if (buttons[3].contains(mousePos)) {
                if (!trace.empty() && currentStep + 1 < trace.size()) {
                    if (playing) {
                        pausedProgress = transitionRawProgress();
                        playing = false;
                    } else {
                        playing = true;
                        animationClock.restart();
                    }
                } else {
                    playing = false;
                    pausedProgress = 0.f;
                }
            } else if (buttons[4].contains(mousePos)) {
                if (!trace.empty() && currentStep + 1 < trace.size()) {
                    ++currentStep;
                    playing = false;
                    pausedProgress = 0.f;
                }
            } else if (buttons[5].contains(mousePos)) {
                currentStep = 0;
                playing = false;
                pausedProgress = 0.f;
            } else if (buttons[6].contains(mousePos)) {
                rebuildNodePositions(activeSnapshot());
                statusMessage = "Tree fitted to canvas.";
            } else if (buttons[7].contains(mousePos)) {
                trace.clear();
                playing = false;
                pausedProgress = 0.f;
                refresh(library);
                statusMessage = "Tree refreshed from the real catalog.";
            } else if (CanvasBounds.contains(mousePos)) {
                rebuildNodePositions(activeSnapshot());
                selectedNodeId = -1;
                const float radius = std::max(10.f, 22.f -
                    std::max(0, activeSnapshot().nodeCount - 12) * 0.45f);
                for (const auto& item : nodePositions) {
                    const int id = item.first;
                    const auto& node = activeSnapshot().nodes[id];
                    if (node.isNil) {
                        continue;
                    }
                    const sf::Vector2f delta(mousePos.x - item.second.x, mousePos.y - item.second.y);
                    if (std::sqrt(delta.x * delta.x + delta.y * delta.y) <= radius) {
                        selectedNodeId = id;
                        break;
                    }
                }
            }
        }
    }

    for (auto& input : inputs) {
        input.handleText(event);
    }
}

void RBTreeVisualizer::update(const sf::Vector2f& mousePos) {
    for (auto& button : buttons) {
        button.update(mousePos);
    }

    if (playing && !trace.empty()) {
        if (currentStep + 1 >= trace.size()) {
            playing = false;
            pausedProgress = 0.f;
        } else if (transitionRawProgress() >= 1.f) {
            ++currentStep;
            pausedProgress = 0.f;
            animationClock.restart();
            if (currentStep + 1 >= trace.size()) {
                playing = false;
            }
        }
    }
}

void RBTreeVisualizer::draw(sf::RenderWindow& window) {
    const RBTreeVisualSnapshot<Book>& active = activeSnapshot();
    rebuildNodePositions(active);

    drawToolbar(window);
    drawCanvas(window, active);
    drawInspector(window, active);
    drawTimeline(window);
}

bool RBTreeVisualizer::consumeBackRequested() {
    const bool requested = backRequested;
    backRequested = false;
    return requested;
}

void RBTreeVisualizer::runAdd(LibraryManagementSystem& library) {
    try {
        if (inputs.size() < 4 || isBlank(inputs[0].content) ||
            isBlank(inputs[1].content) || isBlank(inputs[2].content) ||
            isBlank(inputs[3].content)) {
            statusMessage = "ISBN, title, author, and year are required.";
            return;
        }

        const int isbn = std::stoi(inputs[0].content);
        const int year = std::stoi(inputs[3].content);
        LibraryManagementSystem::TreeOperationResult result =
            library.addBookWithTreeTrace(isbn, inputs[1].content, inputs[2].content, year);
        startTrace(result, library);
        if (result.success) {
            for (auto& input : inputs) {
                input.clear();
            }
        }
    } catch (...) {
        statusMessage = "Invalid ISBN or year.";
    }
}

void RBTreeVisualizer::runRemove(LibraryManagementSystem& library) {
    try {
        if (inputs.empty() || isBlank(inputs[0].content)) {
            statusMessage = "Enter an ISBN to remove.";
            return;
        }

        const int isbn = std::stoi(inputs[0].content);
        LibraryManagementSystem::TreeOperationResult result =
            library.removeBookWithTreeTrace(isbn);
        startTrace(result, library);
    } catch (...) {
        statusMessage = "Invalid ISBN.";
    }
}

void RBTreeVisualizer::startTrace(const LibraryManagementSystem::TreeOperationResult& result,
                                  const LibraryManagementSystem& library) {
    statusMessage = result.message;
    refresh(library);

    if (!result.success) {
        trace.clear();
        playing = false;
        currentStep = 0;
        pausedProgress = 0.f;
        return;
    }

    trace = result.trace;
    currentStep = 0;
    playing = trace.size() > 1;
    selectedNodeId = -1;
    pausedProgress = 0.f;
    animationClock.restart();
}

const RBTreeVisualSnapshot<Book>& RBTreeVisualizer::activeSnapshot() const {
    if (!trace.empty() && currentStep < trace.size()) {
        return trace[currentStep].snapshot;
    }
    return snapshot;
}

const RBTreeTraceStep<Book>* RBTreeVisualizer::activeStep() const {
    if (!trace.empty() && currentStep < trace.size()) {
        return &trace[currentStep];
    }
    return nullptr;
}

const RBTreeTraceStep<Book>* RBTreeVisualizer::nextStep() const {
    if (!trace.empty() && currentStep + 1 < trace.size()) {
        return &trace[currentStep + 1];
    }
    return nullptr;
}

float RBTreeVisualizer::transitionRawProgress() const {
    if (nextStep() == nullptr || secondsPerStep <= 0.f) {
        return 0.f;
    }

    const float elapsedProgress =
        playing ? animationClock.getElapsedTime().asSeconds() / secondsPerStep : 0.f;
    return clamp01(pausedProgress + elapsedProgress);
}

float RBTreeVisualizer::transitionProgress() const {
    return easeInOut(transitionRawProgress());
}

std::string RBTreeVisualizer::visualNodeKey(const RBTreeVisualNode<Book>& node) const {
    if (node.isNil) {
        return "";
    }

    std::ostringstream key;
    key << node.data.getISBN() << ':' << node.data.getCopyId();
    return key.str();
}

void RBTreeVisualizer::rebuildNodePositions(const RBTreeVisualSnapshot<Book>& active) {
    nodePositions = buildNodePositions(active);
}

RBTreeVisualizer::NodePositionMap RBTreeVisualizer::buildNodePositions(
    const RBTreeVisualSnapshot<Book>& active) const {
    NodePositionMap positions;
    if (active.rootId < 0 || active.nodes.empty()) {
        return positions;
    }

    int nextColumn = 0;
    assignInorderPositions(active, active.rootId, nextColumn,
                           std::max(1, active.height), CanvasBounds, positions);
    return positions;
}

int RBTreeVisualizer::assignInorderPositions(
    const RBTreeVisualSnapshot<Book>& active, int nodeId, int& nextColumn,
    int maxDepth, const sf::FloatRect& canvasBounds, NodePositionMap& positions) const {
    if (nodeId < 0 || nodeId >= static_cast<int>(active.nodes.size())) {
        return -1;
    }

    const RBTreeVisualNode<Book>& node = active.nodes[nodeId];
    assignInorderPositions(active, node.leftId, nextColumn, maxDepth, canvasBounds, positions);

    const int column = nextColumn++;
    const int visibleCount = std::max(1, static_cast<int>(active.nodes.size()));
    const float horizontal =
        visibleCount == 1 ? 0.5f : static_cast<float>(column) / static_cast<float>(visibleCount - 1);
    const float x = canvasBounds.position.x + 32.f +
                    horizontal * std::max(1.f, canvasBounds.size.x - 64.f);
    const float levelGap = (canvasBounds.size.y - 58.f) / static_cast<float>(std::max(1, maxDepth));
    const float y = canvasBounds.position.y + 32.f + node.depth * levelGap;
    positions[nodeId] = sf::Vector2f(x, y);

    assignInorderPositions(active, node.rightId, nextColumn, maxDepth, canvasBounds, positions);
    return column;
}

void RBTreeVisualizer::drawToolbar(sf::RenderWindow& window) {
    sf::RectangleShape toolbar(sf::Vector2f(850.f, 70.f));
    toolbar.setPosition(sf::Vector2f(25.f, 90.f));
    toolbar.setFillColor(sf::Color(236, 240, 245));
    toolbar.setOutlineThickness(1.f);
    toolbar.setOutlineColor(sf::Color(190, 198, 208));
    window.draw(toolbar);

    drawText(window, "Red-Black Tree View", 18, sf::Vector2f(450.f, 71.f),
             sf::Color::White, true);

    for (auto& input : inputs) {
        input.draw(window);
    }
    for (auto& button : buttons) {
        button.draw(window);
    }

    drawText(window, statusMessage, 13, sf::Vector2f(448.f, 139.f),
             mutedText(), false);
}

void RBTreeVisualizer::drawCanvas(sf::RenderWindow& window,
                                  const RBTreeVisualSnapshot<Book>& active) {
    sf::RectangleShape canvas(CanvasBounds.size);
    canvas.setPosition(CanvasBounds.position);
    canvas.setFillColor(canvasWhite());
    canvas.setOutlineThickness(1.f);
    canvas.setOutlineColor(borderGray());
    window.draw(canvas);

    const RBTreeTraceStep<Book>* step = activeStep();
    const RBTreeTraceStep<Book>* targetStep = nextStep();
    const float progress = transitionProgress();
    const bool transitioning = targetStep != nullptr && transitionRawProgress() > 0.f;
    const RBTreeVisualSnapshot<Book>* target =
        transitioning ? &targetStep->snapshot : nullptr;

    if ((active.rootId < 0 || active.nodeCount == 0) &&
        (target == nullptr || target->nodeCount == 0)) {
        drawText(window, "The tree is empty.", 22,
                 sf::Vector2f(CanvasBounds.position.x + 205.f, CanvasBounds.position.y + 175.f),
                 sf::Color(45, 54, 65), true);
        drawText(window, "Add a book above to animate the first red-black insertion.", 14,
                 sf::Vector2f(CanvasBounds.position.x + 118.f, CanvasBounds.position.y + 212.f),
                 mutedText());
        return;
    }

    const int primaryId = step != nullptr ? step->primaryNodeId : selectedNodeId;
    const int secondaryId = step != nullptr ? step->secondaryNodeId : -1;

    const NodePositionMap currentPositions = nodePositions;
    const NodePositionMap targetPositions =
        target != nullptr ? buildNodePositions(*target) : NodePositionMap{};

    std::unordered_map<std::string, int> currentByKey;
    std::unordered_map<std::string, int> targetByKey;
    std::vector<std::string> renderKeys;

    for (const auto& node : active.nodes) {
        if (node.isNil) {
            continue;
        }
        const std::string key = visualNodeKey(node);
        currentByKey[key] = node.id;
        renderKeys.push_back(key);
    }

    if (target != nullptr) {
        for (const auto& node : target->nodes) {
            if (node.isNil) {
                continue;
            }
            const std::string key = visualNodeKey(node);
            targetByKey[key] = node.id;
            if (currentByKey.find(key) == currentByKey.end()) {
                renderKeys.push_back(key);
            }
        }
    }

    auto positionOrDefault = [](const NodePositionMap& positions, int nodeId) {
        const auto it = positions.find(nodeId);
        return it != positions.end() ? it->second : sf::Vector2f();
    };

    std::unordered_map<std::string, sf::Vector2f> renderedPositions;
    for (const std::string& key : renderKeys) {
        const auto currentIt = currentByKey.find(key);
        const auto targetIt = targetByKey.find(key);
        const bool hasCurrent = currentIt != currentByKey.end();
        const bool hasTarget = targetIt != targetByKey.end();

        if (hasCurrent && hasTarget && target != nullptr) {
            renderedPositions[key] =
                lerp(positionOrDefault(currentPositions, currentIt->second),
                     positionOrDefault(targetPositions, targetIt->second),
                     progress);
        } else if (hasTarget && target != nullptr) {
            const RBTreeVisualNode<Book>& targetNode = target->nodes[targetIt->second];
            sf::Vector2f start =
                positionOrDefault(targetPositions, targetIt->second);
            if (targetNode.parentId >= 0 &&
                targetNode.parentId < static_cast<int>(target->nodes.size())) {
                const std::string parentKey = visualNodeKey(target->nodes[targetNode.parentId]);
                const auto parentInCurrent = currentByKey.find(parentKey);
                if (parentInCurrent != currentByKey.end()) {
                    start = positionOrDefault(currentPositions, parentInCurrent->second);
                } else {
                    start = positionOrDefault(targetPositions, targetNode.parentId);
                }
            }
            renderedPositions[key] =
                lerp(start, positionOrDefault(targetPositions, targetIt->second), progress);
        } else if (hasCurrent) {
            renderedPositions[key] = positionOrDefault(currentPositions, currentIt->second);
        }
    }

    auto positionForNode = [&](const RBTreeVisualNode<Book>& node,
                               const NodePositionMap& positions) {
        if (!node.isNil) {
            const auto renderedIt = renderedPositions.find(visualNodeKey(node));
            if (renderedIt != renderedPositions.end()) {
                return renderedIt->second;
            }
        }
        return positionOrDefault(positions, node.id);
    };

    auto keyForId = [this](const RBTreeVisualSnapshot<Book>& snapshot, int nodeId) {
        if (nodeId >= 0 && nodeId < static_cast<int>(snapshot.nodes.size()) &&
            !snapshot.nodes[nodeId].isNil) {
            return visualNodeKey(snapshot.nodes[nodeId]);
        }
        return std::string();
    };

    const std::string primaryKey = keyForId(active, primaryId);
    const std::string secondaryKey = keyForId(active, secondaryId);
    const std::string selectedKey = keyForId(active, selectedNodeId);

    auto drawEdges = [&](const RBTreeVisualSnapshot<Book>& snapshot,
                         const NodePositionMap& positions, float opacity,
                         bool useActiveHighlights) {
        if (opacity <= 0.01f) {
            return;
        }
        for (const auto& node : snapshot.nodes) {
            const sf::Vector2f from = positionForNode(node, positions);
            for (int childId : {node.leftId, node.rightId}) {
                if (childId < 0 ||
                    childId >= static_cast<int>(snapshot.nodes.size())) {
                    continue;
                }
                const RBTreeVisualNode<Book>& child = snapshot.nodes[childId];
                const bool highlighted =
                    useActiveHighlights &&
                    (node.id == primaryId || childId == primaryId ||
                     node.id == secondaryId || childId == secondaryId);
                drawLine(window, from, positionForNode(child, positions),
                         withOpacity(highlighted ? sf::Color(38, 128, 214)
                                                 : sf::Color(168, 176, 186),
                                     opacity),
                         highlighted ? 3.f : 1.5f);
            }
        }
    };

    auto drawNilLeaves = [&](const RBTreeVisualSnapshot<Book>& snapshot,
                             const NodePositionMap& positions, float opacity) {
        if (opacity <= 0.01f) {
            return;
        }
        for (const auto& node : snapshot.nodes) {
            if (!node.isNil) {
                continue;
            }
            sf::CircleShape dot(4.f);
            dot.setOrigin(sf::Vector2f(4.f, 4.f));
            dot.setPosition(positionForNode(node, positions));
            dot.setFillColor(withOpacity(sf::Color(190, 196, 204), opacity));
            window.draw(dot);
        }
    };

    drawEdges(active, currentPositions, transitioning ? 1.f - progress : 1.f, true);
    if (target != nullptr) {
        drawEdges(*target, targetPositions, progress, false);
    }

    drawNilLeaves(active, currentPositions, transitioning ? 1.f - progress : 1.f);
    if (target != nullptr) {
        drawNilLeaves(*target, targetPositions, progress);
    }

    const int radiusNodeCount =
        std::max(active.nodeCount, target != nullptr ? target->nodeCount : 0);
    const float baseRadius = std::max(10.f, 22.f -
        std::max(0, radiusNodeCount - 12) * 0.45f);

    for (const std::string& key : renderKeys) {
        const auto currentIt = currentByKey.find(key);
        const auto targetIt = targetByKey.find(key);
        const bool hasCurrent = currentIt != currentByKey.end();
        const bool hasTarget = targetIt != targetByKey.end() && target != nullptr;
        if (!hasCurrent && !hasTarget) {
            continue;
        }

        const RBTreeVisualNode<Book>& node =
            hasTarget ? target->nodes[targetIt->second] : active.nodes[currentIt->second];
        const sf::Vector2f center = renderedPositions[key];
        const float opacity = hasCurrent && hasTarget ? 1.f :
            (hasTarget ? progress : (transitioning ? 1.f - progress : 1.f));
        if (opacity <= 0.01f) {
            continue;
        }

        const float scale = hasCurrent && hasTarget ? 1.f :
            (hasTarget ? 0.72f + 0.28f * progress :
                         (transitioning ? 1.f - 0.25f * progress : 1.f));
        const float radius = baseRadius * scale;
        const sf::Color startColor = hasCurrent ?
            nodeFillColor(active.nodes[currentIt->second].color) : nodeFillColor(node.color);
        const sf::Color endColor = hasTarget ?
            nodeFillColor(target->nodes[targetIt->second].color) : startColor;

        sf::CircleShape circle(radius);
        circle.setOrigin(sf::Vector2f(radius, radius));
        circle.setPosition(center);
        circle.setFillColor(withOpacity(blendColor(startColor, endColor, progress), opacity));
        circle.setOutlineThickness((key == primaryKey || key == selectedKey) ? 4.f : 2.f);
        circle.setOutlineColor(withOpacity(key == primaryKey ? sf::Color(42, 157, 143) :
                                           key == secondaryKey ? sf::Color(255, 183, 77) :
                                           sf::Color(245, 247, 250),
                                           opacity));
        window.draw(circle);

        std::ostringstream label;
        label << node.data.getISBN();
        if (radius >= 15.f) {
            label << "\n#" << node.data.getCopyId();
        }
        sf::Text nodeText(font);
        nodeText.setString(label.str());
        nodeText.setCharacterSize(radius >= 15.f ? 10 : 9);
        nodeText.setFillColor(withOpacity(sf::Color::White, opacity));
        centerText(nodeText, center);
        window.draw(nodeText);
    }
}

void RBTreeVisualizer::drawInspector(sf::RenderWindow& window,
                                     const RBTreeVisualSnapshot<Book>& active) {
    sf::RectangleShape panel(InspectorBounds.size);
    panel.setPosition(InspectorBounds.position);
    panel.setFillColor(sf::Color(248, 250, 252));
    panel.setOutlineThickness(1.f);
    panel.setOutlineColor(borderGray());
    window.draw(panel);

    float y = InspectorBounds.position.y + 14.f;
    drawText(window, "Algorithm Step", 16, sf::Vector2f(InspectorBounds.position.x + 16.f, y),
             sf::Color(35, 43, 54), true);
    y += 28.f;

    if (const RBTreeTraceStep<Book>* step = activeStep()) {
        drawText(window, stepKindLabel(step->kind), 13,
                 sf::Vector2f(InspectorBounds.position.x + 16.f, y),
                 sf::Color(38, 128, 214), true);
        y += 21.f;
        drawText(window, trimForPanel(step->title, 28), 13,
                 sf::Vector2f(InspectorBounds.position.x + 16.f, y),
                 sf::Color(44, 52, 63));
        y += 20.f;
        drawText(window, trimForPanel(step->detail, 34), 12,
                 sf::Vector2f(InspectorBounds.position.x + 16.f, y),
                 mutedText());
    } else {
        drawText(window, "No active animation", 13,
                 sf::Vector2f(InspectorBounds.position.x + 16.f, y), mutedText());
    }

    y = InspectorBounds.position.y + 140.f;
    drawText(window, "Legend", 16, sf::Vector2f(InspectorBounds.position.x + 16.f, y),
             sf::Color(35, 43, 54), true);
    y += 30.f;

    struct LegendItem {
        sf::Color color;
        std::string label;
    };
    const std::vector<LegendItem> legend{
        {sf::Color(190, 47, 55), "Red node"},
        {sf::Color(30, 35, 42), "Black node"},
        {sf::Color(190, 196, 204), "Nil leaf"},
        {sf::Color(42, 157, 143), "Selected/active"}
    };
    for (const auto& item : legend) {
        sf::CircleShape marker(7.f);
        marker.setOrigin(sf::Vector2f(7.f, 7.f));
        marker.setPosition(sf::Vector2f(InspectorBounds.position.x + 24.f, y + 7.f));
        marker.setFillColor(item.color);
        window.draw(marker);
        drawText(window, item.label, 12, sf::Vector2f(InspectorBounds.position.x + 42.f, y),
                 sf::Color(45, 54, 65));
        y += 22.f;
    }

    y += 12.f;
    drawText(window, "Invariants", 16, sf::Vector2f(InspectorBounds.position.x + 16.f, y),
             sf::Color(35, 43, 54), true);
    y += 28.f;

    const std::vector<std::pair<std::string, bool>> checks{
        {"Root black", active.rootBlack},
        {"No red-red", active.noRedRed},
        {"Black-height", active.uniformBlackHeight}
    };
    for (const auto& check : checks) {
        drawText(window, check.second ? "OK" : "FAIL", 12,
                 sf::Vector2f(InspectorBounds.position.x + 16.f, y),
                 check.second ? sf::Color(32, 134, 87) : sf::Color(190, 47, 55), true);
        drawText(window, check.first, 12,
                 sf::Vector2f(InspectorBounds.position.x + 58.f, y),
                 sf::Color(45, 54, 65));
        y += 21.f;
    }

    y += 10.f;
    std::ostringstream stats;
    stats << "Nodes: " << active.nodeCount << "   Height: " << active.height;
    drawText(window, stats.str(), 12, sf::Vector2f(InspectorBounds.position.x + 16.f, y),
             mutedText());
    y += 20.f;
    drawText(window, "Black height: " + std::to_string(active.blackHeight), 12,
             sf::Vector2f(InspectorBounds.position.x + 16.f, y), mutedText());

    y += 35.f;
    drawText(window, "Selected Book", 16, sf::Vector2f(InspectorBounds.position.x + 16.f, y),
             sf::Color(35, 43, 54), true);
    y += 27.f;
    if (selectedNodeId >= 0 && selectedNodeId < static_cast<int>(active.nodes.size()) &&
        !active.nodes[selectedNodeId].isNil) {
        const Book& book = active.nodes[selectedNodeId].data;
        drawText(window, "ISBN " + std::to_string(book.getISBN()) +
                         " / copy " + std::to_string(book.getCopyId()),
                 12, sf::Vector2f(InspectorBounds.position.x + 16.f, y),
                 sf::Color(45, 54, 65), true);
        y += 20.f;
        drawText(window, trimForPanel(book.getTitle(), 28), 12,
                 sf::Vector2f(InspectorBounds.position.x + 16.f, y), mutedText());
        y += 20.f;
        drawText(window, book.getAvailability() ? "Available" : "Checked Out", 12,
                 sf::Vector2f(InspectorBounds.position.x + 16.f, y),
                 book.getAvailability() ? sf::Color(32, 134, 87) : sf::Color(180, 112, 32));
    } else {
        drawText(window, "Click a node to inspect it.", 12,
                 sf::Vector2f(InspectorBounds.position.x + 16.f, y), mutedText());
    }
}

void RBTreeVisualizer::drawTimeline(sf::RenderWindow& window) {
    sf::RectangleShape timeline(TimelineBounds.size);
    timeline.setPosition(TimelineBounds.position);
    timeline.setFillColor(sf::Color(248, 250, 252));
    timeline.setOutlineThickness(1.f);
    timeline.setOutlineColor(borderGray());
    window.draw(timeline);

    drawText(window, "Timeline", 13,
             sf::Vector2f(TimelineBounds.position.x + 12.f, TimelineBounds.position.y + 8.f),
             sf::Color(35, 43, 54), true);

    if (trace.empty()) {
        drawText(window, "No animation loaded.", 12,
                 sf::Vector2f(TimelineBounds.position.x + 90.f, TimelineBounds.position.y + 9.f),
                 mutedText());
        return;
    }

    const float startX = TimelineBounds.position.x + 16.f;
    const float width = TimelineBounds.size.x - 32.f;
    const float y = TimelineBounds.position.y + 35.f;
    const std::size_t count = trace.size();
    for (std::size_t i = 0; i < count; ++i) {
        const float t = count <= 1 ? 0.5f : static_cast<float>(i) / static_cast<float>(count - 1);
        const sf::Vector2f center(startX + t * width, y);
        sf::CircleShape dot(i == currentStep ? 6.f : 4.f);
        dot.setOrigin(sf::Vector2f(dot.getRadius(), dot.getRadius()));
        dot.setPosition(center);
        dot.setFillColor(i == currentStep ? sf::Color(38, 128, 214) : sf::Color(165, 174, 185));
        window.draw(dot);
    }
}

void RBTreeVisualizer::drawText(sf::RenderWindow& window, const std::string& text,
                                unsigned int size, const sf::Vector2f& position,
                                const sf::Color& color, bool bold) {
    sf::Text sfText(font);
    sfText.setString(text);
    sfText.setCharacterSize(size);
    sfText.setFillColor(color);
    if (bold) {
        sfText.setStyle(sf::Text::Bold);
    }
    sfText.setPosition(position);
    window.draw(sfText);
}

std::string RBTreeVisualizer::stepKindLabel(RBTreeStepKind kind) const {
    switch (kind) {
        case RBTreeStepKind::Compare: return "Compare";
        case RBTreeStepKind::InsertRed: return "Insert Red";
        case RBTreeStepKind::DeleteTarget: return "Delete Target";
        case RBTreeStepKind::Successor: return "Successor";
        case RBTreeStepKind::Transplant: return "Transplant";
        case RBTreeStepKind::RotateLeft: return "Rotate Left";
        case RBTreeStepKind::RotateRight: return "Rotate Right";
        case RBTreeStepKind::Recolor: return "Recolor";
        case RBTreeStepKind::RootBlack: return "Root Black";
        case RBTreeStepKind::Complete: return "Complete";
    }
    return "Step";
}
