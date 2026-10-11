#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <set>
#include <filesystem>

const std::string INDEX_FILE = "index.dat";

// Structure to store index-value pairs
struct Record {
    std::string index;
    int value;
    
    bool operator<(const Record& other) const {
        if (index != other.index) return index < other.index;
        return value < other.value;
    }
    
    bool operator==(const Record& other) const {
        return index == other.index && value == other.value;
    }
};

class FileStorageDB {
private:
    // In-memory index mapping index string to sorted values
    std::unordered_map<std::string, std::set<int>> index_map;
    
    // Load index from file
    void loadIndex() {
        std::ifstream indexFile(INDEX_FILE, std::ios::binary);
        if (!indexFile) return; // File doesn't exist yet
        
        std::string line;
        while (std::getline(indexFile, line)) {
            if (line.empty()) continue;
            
            std::istringstream iss(line);
            std::string index;
            int value;
            char delimiter;
            
            if (iss >> index >> delimiter >> value && delimiter == ':') {
                index_map[index].insert(value);
            }
        }
        indexFile.close();
    }
    
    // Save index to file
    void saveIndex() {
        std::ofstream indexFile(INDEX_FILE, std::ios::binary | std::ios::trunc);
        for (const auto& pair : index_map) {
            const std::string& index = pair.first;
            for (int value : pair.second) {
                indexFile << index << ":" << value << "\n";
            }
        }
        indexFile.close();
    }
    
public:
    FileStorageDB() {
        loadIndex();
    }
    
    ~FileStorageDB() {
        saveIndex();
    }
    
    void insert(const std::string& index, int value) {
        index_map[index].insert(value);
    }
    
    void remove(const std::string& index, int value) {
        auto it = index_map.find(index);
        if (it != index_map.end()) {
            it->second.erase(value);
            if (it->second.empty()) {
                index_map.erase(it);
            }
        }
    }
    
    std::vector<int> find(const std::string& index) {
        std::vector<int> result;
        auto it = index_map.find(index);
        if (it != index_map.end()) {
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