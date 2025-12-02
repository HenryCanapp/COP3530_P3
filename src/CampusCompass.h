#pragma once

#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <climits>
#include <unordered_set>
#include <regex>


class CampusCompass {
private:

    //student struct for students data
    struct Student {
        std::string id;
        std::string name;
        //location id of their residence
        std::string residence;
        //list of their classes and the shortest time from their residence
        std::map<std::string, int> classes;
        //list of shortest path edges to desired class in order dijkstra's alg (from, list of proceeding edges)
        std::unordered_map<int, std::vector<int>> paths;
    };

    //an adjacency list in the form of <id_from, vector<tuple<id_to, weight/time, closure>>>
    std::unordered_map<int, std::vector<std::tuple<int, int, bool>>> graph;

    //list of classes
    std::unordered_map<std::string, int> classes;

    //list of students accessible via their id
    std::map<std::string, Student> students;

    //simple list for available commands
    std::unordered_map<std::string, int> commands;
public:
    // Think about what helper functions you will need in the algorithm
    CampusCompass(const std::string& edges_filepath, const std::string& classes_filepath); // constructor

    //reads the CSV file and converts it into 2D array, code taken from guide on canvas
    static std::vector<std::vector<std::string>> readCSV(const std::string& filename);

    //parses through data and fills in graph and classes list
    void parseCSV(const std::string& edges_filepath, const std::string& classes_filepath);

    //checks command validity then executes function
    bool parseCommand(const std::string& command);

    //checks if correct args then adds student to students list
    bool insertStudent(std::vector<std::string>& args);

    //checks if correct args then removes student from the list
    bool removeStudent(std::vector<std::string>& args);

    //checks if correct args then drops the sepcified class for that student
    bool dropClass(std::vector<std::string>& args);

    //checks if correct args then replaces classes as specified for that student
    bool replaceClass(std::vector<std::string>& args);

    //checks args and removes class from every student, outputs how many times removed
    bool removeClass(std::vector<std::string>& args);

    //checks args and toggles edges closure
    bool toggleEdgesClosure(std::vector<std::string>& args);

    //checks args and checks edges status
    bool checkEdgeStatus(std::vector<std::string>& args);

    //checks args then checks if connected
    bool isConnected(std::vector<std::string>& args);

    //checks args then prints student's shortest path
    bool printShortestEdges(std::vector<std::string>& args);

    //checks args then prints the studnets zone
    bool printStudentZone(std::vector<std::string>& args);

    //Dijkstra's alg for shortest distance to every node from start
    std::unordered_map<int, std::pair<int, int>> shortestDistance(int start);

    //updates students shortest distance data
    void updateStudent(std::string id);

    //following functions are all for data validation
    bool validUFID(std::string id);
    bool validName(std::string name);
    bool validClassCode(std::string code);
};
