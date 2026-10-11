#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <set>

const std::string DATA_FILE = "data.idx";

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

// Database class that uses a sorted vector of records for memory efficiency
class FileStorageDB {
private:
    std::vector<Record> records;
    
    // Load records from file at startup
    void load() {
        std::ifstream file(DATA_FILE);
        if (!file) return; // File doesn't exist yet
        
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            // Parse line: index:value
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                Record record;
                record.index = line.substr(0, pos);
                record.value = std::stoi(line.substr(pos + 1));
                records.push_back(record);
            }
        }
        file.close();
        
        // Sort records for efficient searching
        std::sort(records.begin(), records.end());
    }
    
    // Append a single record to file
    void appendToFile(const std::string& index, int value) {
        std::ofstream file(DATA_FILE, std::ios::app);
        file << index << ":" << value << "\n";
        file.close();
    }
    
    // Rewrite entire file with current records
    void rewriteFile() {
        std::ofstream file(DATA_FILE);
        for (const auto& record : records) {
            file << record.index << ":" << record.value << "\n";
        }
        file.close();
    }
    
    // Find the range of records with the given index
    std::pair<std::vector<Record>::iterator, std::vector<Record>::iterator> findRange(const std::string& idx) {
        Record lower = {idx, 0};
        Record upper = {idx, INT_MAX};
        
        auto lower_it = std::lower_bound(records.begin(), records.end(), lower);
        auto upper_it = std::upper_bound(records.begin(), records.end(), upper);
        
        return std::make_pair(lower_it, upper_it);
    }
    
public:
    FileStorageDB() {
        load();
    }
    
    void insert(const std::string& idx, int value) {
        // Check if this exact (index, value) pair already exists
        Record record = {idx, value};
        auto it = std::lower_bound(records.begin(), records.end(), record);
        if (it != records.end() && *it == record) {
            return; // Already exists, do nothing
        }
        
        // Insert in sorted order
        records.insert(it, record);
        
        // Append to file
        appendToFile(idx, value);
    }
    
    void remove(const std::string& idx, int value) {
        Record record = {idx, value};
        auto it = std::lower_bound(records.begin(), records.end(), record);
        if (it != records.end() && *it == record) {
            records.erase(it);
            
            // Rewrite entire file with current state
            rewriteFile();
        }
    }
    
    std::vector<int> find(const std::string& idx) {
        std::vector<int> result;
        
        // Find range of records with this index
        Record lower = {idx, 0};
        Record upper = {idx, INT_MAX};
        
        auto lower_it = std::lower_bound(records.begin(), records.end(), lower);
        auto upper_it = std::upper_bound(records.begin(), records.end(), upper);
        
        // Extract values
        for (auto it = lower_it; it != upper_it; ++it) {
            result.push_back(it->value);
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