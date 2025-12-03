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

    /*todo
     * Insert the student into the list given a specific name and ufid
     * add each cladd into the students classes
     * call update student after the classes have been added which will update
     *  all the shortest distance data
     * Data validation at each step
     * Student has classes from 1-6
     * prints successful
     */
    //checks if correct args then adds student to students list
    bool insertStudent(std::vector<std::string>& args);

    /*todo
     * simply removes the student from the list
     * prints successful
     */
    //checks if correct args then removes student from the list
    bool removeStudent(std::vector<std::string>& args);

    /*todo
     * removes class from a student
     * if a student has no classes left, remove student
     * must also call update student
     * prints successful
     */
    //checks if correct args then drops the sepcified class for that student
    bool dropClass(std::vector<std::string>& args);

    /*todo
     * swaps class around
     * calls update student
     * prints successful
     */
    //checks if correct args then replaces classes as specified for that student
    bool replaceClass(std::vector<std::string>& args);

    /*todo
     * repeatedly call drop class on every student with specified class
     * prints number of students affected
     */
    //checks args and removes class from every student, outputs how many times removed
    bool removeClass(std::vector<std::string>& args);

    /*todo
     * toggles the closure bool of each edge specified
     * must call update student on every student
     * prints successful
     */
    //checks args and toggles edges closure
    bool toggleEdgesClosure(std::vector<std::string>& args);

    /*todo
     * prints whether an edge is open, closes, or DNE
     */
    //checks args and checks edges status
    bool checkEdgeStatus(std::vector<std::string>& args);

    /*todo
     * checks if there is a path between two points
     * best method is prolly just a breadth first search
     * prints successful
     */
    //checks args then checks if connected
    bool isConnected(std::vector<std::string>& args);

    /*todo
     * simple access/reading of the already made shortest pasths data
     * prints each class code and the total time
     * interpret INT_MAX as unreachable and print a time of -1
     */
    //checks args then prints student's shortest path
    bool printShortestEdges(std::vector<std::string>& args);

    /*todo
     * operates on the built in data structure
     * prints the total cost of the built MST
     * ignore unreachable classes?
     */
    //checks args then prints the studnets zone
    bool printStudentZone(std::vector<std::string>& args);

    //Dijkstra's alg for shortest distance to every node from start
    std::unordered_map<int, std::pair<int, int>> shortestDistance(int start);

    /*todo
     * finds the shortest distance to each class from the place of residence
     * updates the classes list with appropriate time values
     * updates the paths list on how to get to each class
     * how to handle unreachable classes?
     */
    //updates students shortest distance data
    void updateStudent(std::string id);

    //following functions are all for data validation
    bool validUFID(std::string id);
    bool validName(std::string name);
    bool validClassCode(std::string code);
};
