#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <set>
#include <map>

const std::string DATA_FILE = "data.idx";

// Database class that uses in-memory index with incremental file updates
class FileStorageDB {
private:
    // In-memory index for fast access
    std::map<std::string, std::set<int>> index;
    
    // Load index from file at startup
    void load() {
        std::ifstream file(DATA_FILE);
        if (!file) return; // File doesn't exist yet
        
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            // Parse line: index:value
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                std::string idx = line.substr(0, pos);
                int value = std::stoi(line.substr(pos + 1));
                index[idx].insert(value);
            }
        }
        file.close();
    }
    
    // Append a single record to file
    void appendToFile(const std::string& index, int value) {
        std::ofstream file(DATA_FILE, std::ios::app);
        file << index << ":" << value << "\n";
        file.close();
    }
    
    // Rewrite entire file (used when deleting)
    void rewriteFile() {
        std::ofstream file(DATA_FILE);
        for (const auto& pair : index) {
            const std::string& idx = pair.first;
            for (int value : pair.second) {
                file << idx << ":" << value << "\n";
            }
        }
        file.close();
    }
    
public:
    FileStorageDB() {
        load();
    }
    
    void insert(const std::string& idx, int value) {
        // Check if this exact (index, value) pair already exists
        if (index.find(idx) != index.end() && index[idx].find(value) != index[idx].end()) {
            return; // Already exists, do nothing
        }
        
        // Add to in-memory index
        index[idx].insert(value);
        
        // Append to file
        appendToFile(idx, value);
    }
    
    void remove(const std::string& idx, int value) {
        auto it = index.find(idx);
        if (it != index.end()) {
            it->second.erase(value);
            if (it->second.empty()) {
                index.erase(it);
            }
            
            // Rewrite entire file with current state
            rewriteFile();
        }
    }
    
    std::vector<int> find(const std::string& idx) {
        std::vector<int> result;
        auto it = index.find(idx);
        if (it != index.end()) {
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