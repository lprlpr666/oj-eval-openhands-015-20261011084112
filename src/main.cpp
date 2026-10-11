#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <unordered_map>

const std::string DATA_FILE = "data.idx";

// Simple database class that directly reads/writes to file for each operation
class FileStorageDB {
private:
    // Cache for recently accessed data to improve performance
    std::unordered_map<std::string, std::set<int>> cache;
    
    // Read all data from file
    void readAll() {
        std::ifstream file(DATA_FILE);
        if (!file) return; // File doesn't exist yet
        
        cache.clear();
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            // Parse line: index:value
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                std::string index = line.substr(0, pos);
                int value = std::stoi(line.substr(pos + 1));
                cache[index].insert(value);
            }
        }
        file.close();
    }
    
    // Write all data to file
    void writeAll() {
        std::ofstream file(DATA_FILE);
        for (const auto& pair : cache) {
            const std::string& index = pair.first;
            for (int value : pair.second) {
                file << index << ":" << value << "\n";
            }
        }
        file.close();
    }
    
public:
    FileStorageDB() {
        readAll();
    }
    
    void insert(const std::string& index, int value) {
        cache[index].insert(value);
        writeAll(); // Write immediately to ensure persistence
    }
    
    void remove(const std::string& index, int value) {
        auto it = cache.find(index);
        if (it != cache.end()) {
            it->second.erase(value);
            if (it->second.empty()) {
                cache.erase(it);
            }
            writeAll(); // Write immediately to ensure persistence
        } else {
            // Still need to write to ensure file is up to date
            writeAll();
        }
    }
    
    std::vector<int> find(const std::string& index) {
        auto it = cache.find(index);
        std::vector<int> result;
        if (it != cache.end()) {
            result.assign(it->second.begin(), it->second.end());
        }
        return result;
    }
};

int main() {
    FileStorageDB db;
    
    int n;
    std::cin >> n;
    std::cin.ignore(); // Ignore the newline after n
    
    for (int i = 0; i < n; i++) {
        std::string command;
        std::getline(std::cin, command);
        
        std::istringstream iss(command);
        std::string op;
        iss >> op;
        
        if (op == "insert") {
            std::string index;
            int value;
            iss >> index >> value;
            db.insert(index, value);
        } else if (op == "delete") {
            std::string index;
            int value;
            iss >> index >> value;
            db.remove(index, value);
        } else if (op == "find") {
            std::string index;
            iss >> index;
            std::vector<int> values = db.find(index);
            
            if (values.empty()) {
                std::cout << "null\n";
            } else {
                for (size_t j = 0; j < values.size(); j++) {
                    if (j > 0) std::cout << " ";
                    std::cout << values[j];
                }
                std::cout << "\n";
            }
        }
    }
    
    return 0;
}