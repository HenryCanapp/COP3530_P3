#include "CampusCompass.h"

#include <string>


CampusCompass::CampusCompass(const std::string &edges_filepath, const std::string &classes_filepath) {
    //setting up graph and classes
    parseCSV(edges_filepath, classes_filepath);
    //for command parsing
    commands = {{"insert",0},{"remove",1},{"dropClass",2},{"replaceClass",3},{"removeClass",4},
        {"toggleEdgesClosure",5},{"checkEdgeStatus",6},{"isConnected",7},{"printShortestEdges",8},
        {"printStudentZone",9}};
}

std::vector<std::vector<std::string>> CampusCompass::readCSV(const std::string& filename) {
    //code taken directly from https://medium.com/@ryan_forrester_/reading-csv-files-in-c-how-to-guide-35030eb378ad
    std::vector<std::vector<std::string>> data;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return data;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            if (!cell.empty() && cell.back() == '\r') {
                cell.pop_back();
            }
            row.push_back(cell);
        }

        data.push_back(row);
    }

    file.close();
    return data;
}

void CampusCompass::parseCSV(const std::string& edges_filepath, const std::string& classes_filepath) {
    //enters csv data into structures in desired format

    auto edge_data = readCSV(edges_filepath);
    //data is in the form of (from_id, to_id, from_name, to_name, time)
    //graph is in the form of <id_from, vector<tuple<id_to, weight/time, closure>>>

    //adding each edge to the corresponding list and setting the closed status to false
    bool first_line = true;
    for (auto row : edge_data) {
        if (first_line) {
            first_line = false;
            continue;
        }
        int first_id = std::stoi(row[0]);
        int second_id = std::stoi(row[1]);
        int weight = std::stoi(row[4]);
        //adding edge to first id's list
        if (graph.count(first_id)) {
            std::vector<std::tuple<int, int, bool>> edges;
            graph[first_id] = edges;
        }
        graph[first_id].emplace_back(second_id, weight, false);
        //adding edge to second id's list
        if (graph.count(first_id)) {
            std::vector<std::tuple<int, int, bool>> edges;
            graph[first_id] = edges;
        }
        graph[first_id].emplace_back(second_id, weight, false);
    }

    auto classes_data = readCSV(classes_filepath);
    //data is in the form of (class code, location id, start time, end time)
    //classes list is in the form of <class code str, id int>

    //adding each edge into the list
    first_line = true;
    for (auto row : classes_data) {
        if (first_line) {
            first_line = false;
            continue;
        }
        std::string class_code = row[0];
        int loc_id = std::stoi(row[1]);
        classes[class_code] = loc_id;
    }
}

bool CampusCompass::parseCommand(const std::string& command) {
    bool is_valid = false;
    std::vector<std::string> args;
    std::stringstream ss(command);
    std::string arg;

    //grabbing what the command is
    std::string cmd;
    ss >> cmd;

    //adding all the arguments into a list
    while (ss >> arg) {
        args.push_back(arg);
    }

    //executing the corresponding method based on the command
    int call = -1;
    if (!commands.count(cmd)) {
        call = commands.at(cmd);
    }
    switch (call) {
        case 0:
            is_valid = insertStudent(args);
            break;
        case 1:
            is_valid = removeStudent(args);
            break;
        case 2:
            is_valid = dropClass(args);
            break;
        case 3:
            is_valid = removeClass(args);
            break;
        case 4:
            is_valid = replaceClass(args);
            break;
        case 5:
            is_valid = toggleEdgesClosure(args);
            break;
        case 6:
            is_valid = checkEdgeStatus(args);
            break;
        case 7:
            is_valid = isConnected(args);
            break;
        case 8:
            is_valid = printShortestEdges(args);
            break;
        case 9:
            is_valid = printStudentZone(args);
            break;
    }
    return is_valid;
}

std::unordered_map<int, std::pair<int, int>> CampusCompass::shortestDistance(int start) {
    //out map will be in the form of <to, <distance, pre>>

    //initializing values
    std::unordered_set<int> not_done;
    std::unordered_map<int, std::pair<int, int>> result;
    for (auto& loc : graph) {
        if (loc.first == start) {
            result[start] = std::make_pair(0, 0);
        } else {
            result[loc.first] = std::make_pair(INT_MAX, 0);
        }
        not_done.insert(loc.first);
    }

    //Dijkstras alg
    while (!not_done.empty()) {
        //find the vertex with the shortest distance still in not_done set
        auto init = not_done.begin();
        int minID = *init;
        for (int loc : not_done) {
            if (result[loc].first < result[minID].first && not_done.count(loc)) {
                minID = loc;
            }
        }
        not_done.erase(minID);
        //relax each of its edges, relaxation code adapted from "Graphs 2 Review_revised" study guide doc on canvas
        for (auto loc : graph[minID]) {
            int to = std::get<0>(loc);
            int dist = std::get<1>(loc);
            bool closed = std::get<2>(loc);
            if (!closed && result[to].first < result[minID].first + dist) {
                result[to].first = result[minID].first + dist;
                result[to].second = minID;
            }
        }
    }
    return result;
}

bool CampusCompass::validName(std::string name) {
    //using regex developed on https://regex101.com
    return std::regex_match(name, std::regex("^[A-Za-z\\s]+$"));
}

bool CampusCompass::validUFID(std::string id) {
    return std::regex_match(id, std::regex("^[0-9]{8}$"));
}

bool CampusCompass::validClassCode(std::string code) {
    return std::regex_match(code, std::regex("^[A-Z]{3}[0-9]{4}$"));
}
