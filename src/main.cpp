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

    std::stringstream output;
    for (int i = 0; i < no_of_lines; i++) {
        std::getline(std::cin, command);

        // parse commands and add to the output
        compass.parseCommand(command, output);

    }
    std::cout << output.str() << std::endl;
    return 0;
}
