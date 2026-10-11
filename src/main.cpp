#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>

const std::string DATA_FILE = "database.dat";

// Structure to store index-value pairs
struct Record {
    std::string index;
    int value;
    
    bool operator<(const Record& other) const {
        if (index != other.index) return index < other.index;
        return value < other.value;
    }
};

class FileStorageDB {
private:
    std::map<std::string, std::set<int>> data;
    
public:
    void load() {
        std::ifstream file(DATA_FILE, std::ios::binary);
        if (!file) return; // File doesn't exist yet
        
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            std::istringstream iss(line);
            std::string index;
            int value;
            char delimiter;
            
            if (iss >> index >> delimiter >> value && delimiter == ':') {
                data[index].insert(value);
            }
        }
        file.close();
    }
    
    void save() {
        std::ofstream file(DATA_FILE, std::ios::binary | std::ios::trunc);
        for (const auto& pair : data) {
            const std::string& index = pair.first;
            for (int value : pair.second) {
                file << index << ":" << value << "\n";
            }
        }
        file.close();
    }
    
    void insert(const std::string& index, int value) {
        data[index].insert(value);
    }
    
    void remove(const std::string& index, int value) {
        auto it = data.find(index);
        if (it != data.end()) {
            it->second.erase(value);
            if (it->second.empty()) {
                data.erase(it);
            }
        }
    }
    
    std::vector<int> find(const std::string& index) {
        std::vector<int> result;
        auto it = data.find(index);
        if (it != data.end()) {
            result.assign(it->second.begin(), it->second.end());
        }
        return result;
    }
};

int main() {
    FileStorageDB db;
    db.load();
    
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
    
    db.save();
    return 0;
}