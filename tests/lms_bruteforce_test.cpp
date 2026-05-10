#include "LibraryManagementSystem.h"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

class NullBuffer : public std::streambuf {
public:
    int overflow(int c) override {
        return c;
    }
};

struct Metric {
    std::string name;
    std::size_t operations;
    double seconds;
    std::string note;
};

struct Summary {
    std::size_t count = 0;
    std::size_t uniqueIsbns = 0;
    std::size_t finalBooks = 0;
    std::size_t availableBooks = 0;
    std::size_t unavailableBooks = 0;
    std::uintmax_t libraryDatBytes = 0;
    std::uintmax_t explicitSaveBytes = 0;
    std::vector<Metric> metrics;
};

std::size_t parseCount(int argc, char** argv) {
    if (argc < 2) {
        return 1000;
    }

    std::string input = argv[1];
    std::size_t parsedCharacters = 0;
    unsigned long long value = std::stoull(input, &parsedCharacters);
    if (parsedCharacters != input.size() || value == 0) {
        throw std::invalid_argument("count must be a positive integer");
    }
    return static_cast<std::size_t>(value);
}

std::filesystem::path parseOutputDirectory(int argc, char** argv) {
    if (argc < 3) {
        return std::filesystem::current_path() / "benchmark-results";
    }
    return std::filesystem::absolute(argv[2]);
}

template <typename Func>
double measure(Func func) {
    auto start = std::chrono::steady_clock::now();
    func();
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(end - start).count();
}

std::string csvEscape(const std::string& value) {
    if (value.find_first_of(",\"\n") == std::string::npos) {
        return value;
    }

    std::string escaped = "\"";
    for (char ch : value) {
        if (ch == '"') {
            escaped += "\"\"";
        } else {
            escaped += ch;
        }
    }
    escaped += '"';
    return escaped;
}

void appendMetric(Summary& summary,
                  const std::string& name,
                  std::size_t operations,
                  double seconds,
                  const std::string& note) {
    summary.metrics.push_back(Metric{name, operations, seconds, note});
}

void writeCsv(const std::filesystem::path& csvPath,
              const Summary& summary,
              bool includeHeader) {
    std::ofstream out(csvPath, std::ios::app);
    if (!out.is_open()) {
        throw std::runtime_error("could not open CSV output file");
    }

    if (includeHeader) {
        out << "count,unique_isbns,metric,operations,total_seconds,ops_per_second,note,"
               "final_books,available_books,unavailable_books,library_dat_bytes,explicit_save_bytes\n";
    }

    for (const Metric& metric : summary.metrics) {
        const double opsPerSecond = metric.seconds > 0.0
                                        ? static_cast<double>(metric.operations) / metric.seconds
                                        : 0.0;
        out << summary.count << ','
            << summary.uniqueIsbns << ','
            << csvEscape(metric.name) << ','
            << metric.operations << ','
            << std::fixed << std::setprecision(6) << metric.seconds << ','
            << std::fixed << std::setprecision(2) << opsPerSecond << ','
            << csvEscape(metric.note) << ','
            << summary.finalBooks << ','
            << summary.availableBooks << ','
            << summary.unavailableBooks << ','
            << summary.libraryDatBytes << ','
            << summary.explicitSaveBytes << '\n';
    }
}

void printSummary(const Summary& summary) {
    std::cout << "LibraryManagementSystem brute-force test" << std::endl;
    std::cout << "  requested addBook count: " << summary.count << std::endl;
    std::cout << "  unique ISBN groups: " << summary.uniqueIsbns << std::endl;
    std::cout << "  final books: " << summary.finalBooks << std::endl;
    std::cout << "  available books: " << summary.availableBooks << std::endl;
    std::cout << "  unavailable books: " << summary.unavailableBooks << std::endl;
    std::cout << "  library.dat bytes: " << summary.libraryDatBytes << std::endl;
    std::cout << "  explicit save bytes: " << summary.explicitSaveBytes << std::endl;

    for (const Metric& metric : summary.metrics) {
        const double opsPerSecond = metric.seconds > 0.0
                                        ? static_cast<double>(metric.operations) / metric.seconds
                                        : 0.0;
        std::cout << "  " << metric.name
                  << ": ops=" << metric.operations
                  << ", seconds=" << std::fixed << std::setprecision(6) << metric.seconds
                  << ", ops/s=" << std::fixed << std::setprecision(2) << opsPerSecond
                  << ", note=" << metric.note << std::endl;
    }
}

Summary runScenario(std::size_t count,
                    const std::filesystem::path& outputDirectory,
                    bool appendHeader) {
    Summary summary;
    summary.count = count;
    summary.uniqueIsbns = std::max<std::size_t>(1, count / 10);

    const std::filesystem::path originalCwd = std::filesystem::current_path();
    const std::filesystem::path workDir =
        std::filesystem::temp_directory_path() /
        ("lms-bruteforce-" + std::to_string(count) + "-" +
         std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));

    std::filesystem::create_directories(workDir);
    std::filesystem::create_directories(outputDirectory);

    NullBuffer nullBuffer;
    std::ostream nullStream(&nullBuffer);
    std::streambuf* originalCout = std::cout.rdbuf(nullStream.rdbuf());

    try {
        std::filesystem::current_path(workDir);
        LibraryManagementSystem library;
        const int baseIsbn = 100000;

        appendMetric(summary,
                     "addBook",
                     count,
                     measure([&]() {
                         for (std::size_t i = 0; i < count; ++i) {
                             const int isbn = baseIsbn + static_cast<int>(i % summary.uniqueIsbns);
                             library.addBook(isbn,
                                             "Title " + std::to_string(i),
                                             "Author " + std::to_string(i % 97),
                                             1950 + static_cast<int>(i % 75));
                         }
                     }),
                     "public addBook; each call scans copy IDs and appends to journal");

        const std::size_t findOps = count;
        std::size_t findHits = 0;
        const double findSeconds = measure([&]() {
            for (std::size_t i = 0; i < findOps; ++i) {
                const bool hit = (i % 4) != 0;
                const int isbn = hit
                                     ? baseIsbn + static_cast<int>(i % summary.uniqueIsbns)
                                     : baseIsbn + static_cast<int>(summary.uniqueIsbns + i);
                if (library.findBook(isbn) != nullptr) {
                    ++findHits;
                }
            }
        });
        appendMetric(summary,
                     "findBook",
                     findOps,
                     findSeconds,
                     "75% hit / 25% miss lookup mix; hits=" + std::to_string(findHits));

        const std::size_t checkoutOps = count / 2;
        appendMetric(summary,
                     "checkoutBook",
                     checkoutOps,
                     measure([&]() {
                         for (std::size_t i = 0; i < checkoutOps; ++i) {
                             const int isbn = baseIsbn + static_cast<int>(i % summary.uniqueIsbns);
                             library.checkoutBook(isbn);
                         }
                     }),
                     "public checkoutBook; successful calls append to journal");

        const std::size_t returnOps = count / 4;
        appendMetric(summary,
                     "returnBook",
                     returnOps,
                     measure([&]() {
                         for (std::size_t i = 0; i < returnOps; ++i) {
                             const int isbn = baseIsbn + static_cast<int>(i % summary.uniqueIsbns);
                             library.returnBook(isbn);
                         }
                     }),
                     "public returnBook; successful calls append to journal");

        std::size_t traversed = 0;
        std::size_t available = 0;
        appendMetric(summary,
                     "forEachBook",
                     1,
                     measure([&]() {
                         library.forEachBook([&](const Book& book) {
                             ++traversed;
                             if (book.getAvailability()) {
                                 ++available;
                             }
                         });
                     }),
                     "single full catalog traversal");

        const std::filesystem::path explicitSave = workDir / "explicit_save.dat";
        appendMetric(summary,
                     "saveToFile",
                     1,
                     measure([&]() {
                         library.saveToFile(explicitSave.string());
                     }),
                     "single explicit full catalog save");

        const std::size_t removeOps = summary.uniqueIsbns;
        appendMetric(summary,
                     "removeBook",
                     removeOps,
                     measure([&]() {
                         for (std::size_t i = 0; i < removeOps; ++i) {
                             const int isbn = baseIsbn + static_cast<int>(i);
                             library.removeBook(isbn);
                         }
                     }),
                     "one public removeBook call per ISBN group; successful calls append to journal");

        LibraryManagementSystem loadedLibrary;
        appendMetric(summary,
                     "loadFromFile",
                     1,
                     measure([&]() {
                         loadedLibrary.loadFromFile(explicitSave.string());
                     }),
                     "reloads the pre-removal explicit save snapshot");

        summary.finalBooks = 0;
        summary.availableBooks = 0;
        summary.unavailableBooks = 0;
        library.forEachBook([&](const Book& book) {
            ++summary.finalBooks;
            if (book.getAvailability()) {
                ++summary.availableBooks;
            } else {
                ++summary.unavailableBooks;
            }
        });

        const std::filesystem::path libraryDat = workDir / "library.dat";
        if (std::filesystem::exists(libraryDat)) {
            summary.libraryDatBytes = std::filesystem::file_size(libraryDat);
        }
        if (std::filesystem::exists(explicitSave)) {
            summary.explicitSaveBytes = std::filesystem::file_size(explicitSave);
        }

        std::cout.rdbuf(originalCout);
        printSummary(summary);
        writeCsv(outputDirectory / "lms_bruteforce_results.csv", summary, appendHeader);
    } catch (...) {
        std::cout.rdbuf(originalCout);
        std::filesystem::current_path(originalCwd);
        std::filesystem::remove_all(workDir);
        throw;
    }

    std::filesystem::current_path(originalCwd);
    std::filesystem::remove_all(workDir);
    return summary;
}

} // namespace

int main(int argc, char** argv) {
    try {
        const std::size_t count = parseCount(argc, argv);
        const std::filesystem::path outputDirectory = parseOutputDirectory(argc, argv);
        const bool includeHeader = argc < 4 || std::string(argv[3]) != "--append";
        runScenario(count, outputDirectory, includeHeader);
        return EXIT_SUCCESS;
    } catch (const std::exception& ex) {
        std::cerr << "LMS brute-force test error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}
