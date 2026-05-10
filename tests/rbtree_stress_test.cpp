#include "RedBlackTree.h"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct Options {
    std::size_t count = 10000000;
    std::string pattern = "permuted";
    std::size_t removeEvery = 0;
};

Options parseOptions(int argc, char** argv) {
    Options options;

    if (argc >= 2) {
        std::string input = argv[1];
        std::size_t parsedCharacters = 0;
        unsigned long long value = std::stoull(input, &parsedCharacters);
        if (parsedCharacters != input.size() || value == 0) {
            throw std::invalid_argument("count must be a positive integer");
        }
        options.count = static_cast<std::size_t>(value);
    }

    if (argc >= 3) {
        options.pattern = argv[2];
        if (options.pattern != "ordered" &&
            options.pattern != "reverse" &&
            options.pattern != "permuted") {
            throw std::invalid_argument("pattern must be ordered, reverse, or permuted");
        }
    }

    if (argc >= 4) {
        std::string input = argv[3];
        std::size_t parsedCharacters = 0;
        unsigned long long value = std::stoull(input, &parsedCharacters);
        if (parsedCharacters != input.size()) {
            throw std::invalid_argument("removeEvery must be a non-negative integer");
        }
        options.removeEvery = static_cast<std::size_t>(value);
    }

    if (options.count > static_cast<std::size_t>(std::numeric_limits<int>::max()) - 1) {
        throw std::out_of_range("count is too large for this int-based stress test");
    }

    return options;
}

std::vector<int> makeInput(const Options& options) {
    std::vector<int> values;
    values.reserve(options.count);

    if (options.pattern == "ordered") {
        for (std::size_t i = 0; i < options.count; ++i) {
            values.push_back(static_cast<int>(i + 1));
        }
        return values;
    }

    if (options.pattern == "reverse") {
        for (std::size_t i = options.count; i > 0; --i) {
            values.push_back(static_cast<int>(i));
        }
        return values;
    }

    std::size_t stride = 104729;
    while (std::gcd(stride, options.count) != 1) {
        stride += 2;
    }

    std::size_t index = 0;
    for (std::size_t i = 0; i < options.count; ++i) {
        values.push_back(static_cast<int>(index + 1));
        index = (index + stride) % options.count;
    }

    return values;
}

template <typename ClockTime>
double secondsSince(ClockTime start, ClockTime end) {
    return std::chrono::duration<double>(end - start).count();
}

std::uint64_t expectedSum(std::size_t count) {
    return static_cast<std::uint64_t>(count) *
           static_cast<std::uint64_t>(count + 1) / 2;
}

std::uint64_t removedSum(std::size_t count, std::size_t removeEvery) {
    if (removeEvery == 0) {
        return 0;
    }

    const std::size_t removedCount = count / removeEvery;
    return static_cast<std::uint64_t>(removeEvery) *
           static_cast<std::uint64_t>(removedCount) *
           static_cast<std::uint64_t>(removedCount + 1) / 2;
}

bool validateTraversal(RedBlackTree<int>& tree,
                       std::size_t expectedCount,
                       std::uint64_t expectedValueSum) {
    std::size_t traversed = 0;
    std::uint64_t actualSum = 0;
    int previous = 0;
    bool sorted = true;

    tree.inorderTraversal([&](int value) {
        if (traversed > 0 && value <= previous) {
            sorted = false;
        }
        previous = value;
        actualSum += static_cast<std::uint64_t>(value);
        ++traversed;
    });

    if (!sorted || traversed != expectedCount || actualSum != expectedValueSum) {
        std::cerr << "Traversal validation failed. sorted=" << sorted
                  << ", traversed=" << traversed
                  << ", expectedCount=" << expectedCount
                  << ", sum=" << actualSum
                  << ", expectedSum=" << expectedValueSum << std::endl;
        return false;
    }

    return true;
}

bool validateSearches(RedBlackTree<int>& tree, std::size_t count) {
    std::vector<int> presentProbes{
        1,
        static_cast<int>(std::max<std::size_t>(1, count / 4)),
        static_cast<int>(std::max<std::size_t>(1, count / 2)),
        static_cast<int>(std::max<std::size_t>(1, (count * 3) / 4)),
        static_cast<int>(count)
    };

    for (int value : presentProbes) {
        if (tree.search(value) == nullptr) {
            std::cerr << "Search validation failed for present value " << value << std::endl;
            return false;
        }
    }

    if (tree.search(0) != nullptr || tree.search(static_cast<int>(count + 1)) != nullptr) {
        std::cerr << "Search validation failed for missing boundary values." << std::endl;
        return false;
    }

    return true;
}

} // namespace

int main(int argc, char** argv) {
    try {
        const Options options = parseOptions(argc, argv);
        std::cout << "RedBlackTree stress test" << std::endl;
        std::cout << "  count: " << options.count << std::endl;
        std::cout << "  pattern: " << options.pattern << std::endl;
        std::cout << "  removeEvery: " << options.removeEvery << std::endl;

        auto setupStart = std::chrono::steady_clock::now();
        std::vector<int> values = makeInput(options);
        auto setupEnd = std::chrono::steady_clock::now();
        std::cout << "Input prepared in "
                  << secondsSince(setupStart, setupEnd) << "s" << std::endl;

        RedBlackTree<int> tree;

        auto insertStart = std::chrono::steady_clock::now();
        for (int value : values) {
            tree.insert(value);
        }
        auto insertEnd = std::chrono::steady_clock::now();
        std::cout << "Inserted " << options.count << " values in "
                  << secondsSince(insertStart, insertEnd) << "s" << std::endl;

        auto searchStart = std::chrono::steady_clock::now();
        if (!validateSearches(tree, options.count)) {
            return EXIT_FAILURE;
        }
        auto searchEnd = std::chrono::steady_clock::now();
        std::cout << "Search probes completed in "
                  << secondsSince(searchStart, searchEnd) << "s" << std::endl;

        auto traversalStart = std::chrono::steady_clock::now();
        if (!validateTraversal(tree, options.count, expectedSum(options.count))) {
            return EXIT_FAILURE;
        }
        auto traversalEnd = std::chrono::steady_clock::now();
        std::cout << "Validated full traversal in "
                  << secondsSince(traversalStart, traversalEnd) << "s" << std::endl;

        if (options.removeEvery > 0) {
            auto removeStart = std::chrono::steady_clock::now();
            for (std::size_t value = options.removeEvery;
                 value <= options.count;
                 value += options.removeEvery) {
                tree.remove(static_cast<int>(value));
            }
            auto removeEnd = std::chrono::steady_clock::now();

            const std::size_t removedCount = options.count / options.removeEvery;
            std::cout << "Removed " << removedCount << " sampled values in "
                      << secondsSince(removeStart, removeEnd) << "s" << std::endl;

            auto postRemoveStart = std::chrono::steady_clock::now();
            if (!validateTraversal(tree,
                                   options.count - removedCount,
                                   expectedSum(options.count) -
                                       removedSum(options.count, options.removeEvery))) {
                return EXIT_FAILURE;
            }
            auto postRemoveEnd = std::chrono::steady_clock::now();
            std::cout << "Validated post-removal traversal in "
                      << secondsSince(postRemoveStart, postRemoveEnd) << "s" << std::endl;
        }

        std::cout << "Stress test passed." << std::endl;
        return EXIT_SUCCESS;
    } catch (const std::exception& ex) {
        std::cerr << "Stress test error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}
