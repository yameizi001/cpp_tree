#include <iostream>
#include <filesystem>
#include <stack>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

class pathinfo {
public:
    pathinfo(const fs::path &path, int depth, const std::string &pre_way, bool has_next = true) {
        this->path = path;
        this->depth = depth;
        if (depth > 0) {
            std::string tag = has_next ? "1" : "0";
            way = pre_way + tag;
        }
    }

    ~pathinfo() = default;

    fs::path path;
    int depth;
    std::string way;
};

std::string prefix_tag(const std::string &way) {
    std::string prefix;
    for (int i = 0; i < way.length(); ++i) {
        if (i == way.length() - 1) {
            prefix += way[i] == '1' ? " \u251C\u2500" : " \u2514\u2500";
        } else if (way[i] == '1') {
            prefix += " \u2502";
        } else {
            prefix += "  ";
        }
    }
    return prefix;
}

int main() {
    std::stack<pathinfo> stack;
    std::vector<fs::path> dirs;
    std::vector<fs::path> files;
    std::vector<fs::path> composite;

    stack.push(pathinfo(".", -1, "", false));

    while (!stack.empty()) {
        pathinfo path_info = stack.top();
        const fs::path &path = path_info.path;
        const int depth = path_info.depth;
        const std::string &way = path_info.way;
        if (fs::is_directory(path)) {
            if (depth != -1) {
                std::string prefix = prefix_tag(way);
                std::cout << prefix + path.filename().string() << std::endl;
            }

            fs::directory_iterator iterator = fs::directory_iterator(path);

            stack.pop();
            dirs.clear();
            files.clear();
            composite.clear();

            for (const auto &entry: iterator) {
                const fs::path &inner_path = entry.path();
                if (fs::is_directory(inner_path)) {
                    dirs.push_back(inner_path);
                } else if (fs::is_regular_file(inner_path)) {
                    files.push_back(inner_path);
                }
            }

            std::sort(dirs.begin(), dirs.end(), std::greater<>());
            std::sort(files.begin(), files.end(), std::greater<>());

            for (const auto &file: files) {
                composite.push_back(file);
            }
            for (const auto &dir: dirs) {
                composite.push_back(dir);
            }
            for (int i = 0; i < composite.size(); ++i) {
                if (i != 0) {
                    stack.push(pathinfo(composite.at(i), depth + 1, way, true));
                } else {
                    stack.push(pathinfo(composite.at(i), depth + 1, way, false));
                }
            }
        } else if (fs::is_regular_file(path)) {
            std::string prefix = prefix_tag(way);
            std::cout << prefix + path.filename().string() << std::endl;
            stack.pop();
        }
    }

    return 0;
}
