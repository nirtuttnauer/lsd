#include <iostream>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <getopt.h>

namespace fs = std::filesystem;

// Helper function to check if a file is hidden
bool isHidden(const fs::path &path) {
    return path.filename().string().rfind(".", 0) == 0;
}

// Helper function to determine file type from extension
std::string getFileType(const fs::path &path) {
    static const std::unordered_map<std::string, std::string> fileTypes = {
        {".py", "Python"}, {".ts", "TypeScript"}, {".js", "JavaScript"},
        {".cpp", "C++ Source File"}, {".h", "C/C++ Header File"},
        {".txt", "Text File"}, {".pdf", "PDF Document"},
        {".png", "PNG Image"}, {".jpg", "JPEG Image"},
        {".mp3", "MP3 Audio"}, {".mp4", "MP4 Video"},
        {".zip", "ZIP Archive"}, {".tar", "TAR Archive"},
        {".gz", "GZIP Archive"}, {".exe", "Executable"},
        {".sh", "Shell Script"}, {".md", "Markdown Document"},
        {".json", "JSON File"}, {".xml", "XML File"},
        {".html", "HTML Document"}, {".css", "CSS Stylesheet"},
        {".scss", "SCSS Stylesheet"}, {".less", "LESS Stylesheet"},
        {".java", "Java Source File"}, {".class", "Java Class File"},
        {".jar", "Java Archive"}, {".rb", "Ruby Script"},
        {".php", "PHP Script"}, {".sql", "SQL Script"},
        {".c", "C Source File"}, {".cs", "C# Source File"},
        {".swift", "Swift Source File"}, {".kt", "Kotlin Source File"},
        {".go", "Go Source File"}, {".rs", "Rust Source File"},
        {".lua", "Lua Script"}, {".pl", "Perl Script"},
        {".r", "R Script"}, {".m", "MATLAB Script"},
        {".jl", "Julia Script"}, {".ipynb", "Jupyter Notebook"},
        {".yml", "YAML File"}, {".toml", "TOML File"},
        {".ini", "INI File"}, {".conf", "Configuration File"},
        {".log", "Log File"}, {".csv", "CSV File"},
        {".tsv", "TSV File"}, {".xls", "Excel File"},
        {".xlsx", "Excel File"}, {".doc", "Word Document"},
        {".docx", "Word Document"}, {".ppt", "PowerPoint Document"},
        {".pptx", "PowerPoint Document"}, {".key", "Keynote Document"},
        {".pages", "Pages Document"}, {".numbers", "Numbers Document"},
        {".svg", "SVG Image"}, {".gif", "GIF Image"},
        // Add more file types as needed
    };

    auto ext = path.extension().string();
    if (fileTypes.count(ext)) {
        return fileTypes.at(ext);
    }
    return "Unknown File Type";
}

// Helper function to get file size in human-readable format
std::string getFileSize(const fs::path &path) {
    try {
        auto size = fs::file_size(path);
        std::ostringstream oss;

        if (size < 1024) {
            oss << size << " B";
        } else if (size < 1024 * 1024) {
            oss << std::fixed << std::setprecision(2) << (size / 1024.0) << " KB";
        } else if (size < 1024 * 1024 * 1024) {
            oss << std::fixed << std::setprecision(2) << (size / (1024.0 * 1024.0)) << " MB";
        } else {
            oss << std::fixed << std::setprecision(2) << (size / (1024.0 * 1024.0 * 1024.0)) << " GB";
        }

        return oss.str();
    } catch (const std::exception &) {
        return "Unknown Size";
    }
}

// Function to generate folder structure
std::string getFolderStructure(const fs::path &dirPath, int level = 0, int maxDepth = -1, bool showHidden = false, bool verbose = false) {
    std::ostringstream oss;

    if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
        oss << "Invalid directory path: " << dirPath << std::endl;
        return oss.str();
    }

    for (const auto &entry : fs::directory_iterator(dirPath)) {
        if (!showHidden && isHidden(entry.path())) {
            continue;
        }

        // Use tabs for hierarchy
        oss << std::string(level, '\t');
        oss << entry.path().filename().string();

        if (verbose) {
            oss << " [";
            if (fs::is_directory(entry)) {
                oss << "Directory";
            } else {
                oss << getFileType(entry.path()) << ", " << getFileSize(entry.path());
            }
            oss << "]";
        }
        oss << std::endl;

        // Recursively process subdirectories
        if (fs::is_directory(entry) && (maxDepth == -1 || level < maxDepth)) {
            oss << getFolderStructure(entry, level + 1, maxDepth, showHidden, verbose);
        }
    }

    return oss.str();
}

// Function to copy text to clipboard
void copyToClipboard(const std::string &text) {
#ifdef __APPLE__
    FILE *pipe = popen("pbcopy", "w");
#elif __linux__
    FILE *pipe = popen("xclip -selection clipboard", "w");
#else
    FILE *pipe = nullptr;
    std::cerr << "Clipboard functionality is not supported on this platform." << std::endl;
#endif
    if (pipe) {
        fwrite(text.c_str(), sizeof(char), text.size(), pipe);
        pclose(pipe);
    } else {
        std::cerr << "Error: Unable to access clipboard utility." << std::endl;
    }
}

void printUsage() {
    std::cout << "Usage: lsd [options] [directory]\n"
              << "Options:\n"
              << "  /, --depth <n>      Limit recursion depth\n"
              << "  -, --hidden         Include hidden files\n"
              << "  +, --verbose        Show detailed information\n"
              << "  =, --clipboard      Copy output to clipboard\n"
              << "  h               Show this help message\n";
}

void parseArguments(int argc, char *argv[], fs::path &dirPath, int &maxDepth, bool &showHidden, bool &verbose, bool &copyClipboard) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "/") {
            try {
                if (i + 1 < argc) {
                    maxDepth = std::stoi(argv[++i]);
                } else {
                    throw std::invalid_argument("Missing depth value after '/'");
                }
            } catch (const std::exception &) {
                std::cerr << "Invalid depth value. Using default (-1).\n";
                maxDepth = -1;
            }
        } else if (arg == "+") {
            verbose = true;
        } else if (arg == "-") {
            showHidden = true;
        } else if (arg == "=") {
            copyClipboard = true;
        } else if (arg == "h") {
            printUsage();
            exit(EXIT_SUCCESS);
        } else if (dirPath.empty()) {
            dirPath = arg; // Assume it's the directory path
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            printUsage();
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[]) {
    fs::path dirPath = fs::current_path();
    int maxDepth = -1;
    bool showHidden = false;
    bool verbose = false;
    bool copyClipboard = false;

    parseArguments(argc, argv, dirPath, maxDepth, showHidden, verbose, copyClipboard);

    std::cout << "Folder structure of: " << dirPath << std::endl;
    std::string output = getFolderStructure(dirPath, 0, maxDepth, showHidden, verbose);
    std::cout << output;

    if (copyClipboard) {
        copyToClipboard(output);
        std::cout << "Copied folder structure to clipboard." << std::endl;
    }

    return 0;
}