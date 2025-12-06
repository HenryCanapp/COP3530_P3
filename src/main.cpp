#include <iostream>

#include "CampusCompass.h"


int main() {
    // initialize your main project object
    CampusCompass compass("../data/edges.csv", "../data/classes.csv");

    // the below is example code for parsing commandline input
    //I am using the example code
    int no_of_lines;
    std::string command;
    std::cin >> no_of_lines;
    std::cin.ignore(); // ignore newline that first cin left over
    for (int i = 0; i < no_of_lines; i++) {
        std::getline(std::cin, command);

        // parse commands
        bool success = compass.parseCommand(command);
        if (!success) {
            std::cout << "unsuccessful" << std::endl;
        }
    }
    return 0;
}
