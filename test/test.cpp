#include <catch2/catch_test_macros.hpp>
#include <iostream>

// change if you choose to use a different header name
#include "CampusCompass.h"




// This uses C++ "raw strings" and assumes your CampusCompass outputs a string with
//   the same thing you print.
TEST_CASE("Default test cases", "[default]") {
  std::string input;
  std::string expectedOutput;

  SECTION("gradescope 1") {
    input = R"(6
insert "Student A" 10000001 1 1 COP3502
insert "Student B" 10000002 1 1 COP3502
insert "Student C" 10000003 1 2 COP3502 MAC2311
dropClass 10000001 COP3502
remove 10000001
removeClass COP3502
)";

    expectedOutput = R"(successful
successful
successful
successful
unsuccessful
2
)";
  };


  SECTION("gradescope 2") {
    input = R"(6
checkEdgeStatus 1 2
toggleEdgesClosure 1 1 2
checkEdgeStatus 1 2
checkEdgeStatus 2 1
checkEdgeStatus 1 3
checkEdgeStatus 1 99
)";

    expectedOutput = R"(open
successful
closed
closed
DNE
DNE
)";
  };

  SECTION("gradescope 3") {
    input = R"(5
isConnected 33 52
toggleEdgesClosure 1 33 52
isConnected 33 52
toggleEdgesClosure 1 33 52
isConnected 33 52
)";

    expectedOutput = R"(successful
successful
unsuccessful
successful
successful
)";
  };

  SECTION("gradescope 4") {
    input = R"(5
insert "Jane Doe" 12121212 1 1 IDS2935
printShortestEdges 12121212
toggleEdgesClosure 1 1 4
printShortestEdges 12121212
toggleEdgesClosure 1 1 4
)";

    expectedOutput = R"(successful
Name: Jane Doe
IDS2935 | Total Time: 13
successful
Name: Jane Doe
IDS2935 | Total Time: 17
successful
)";
  };

  SECTION("gradescope 5") {
    input = R"(2
insert "John Doe" 88887777 5 4 IDS2935 CEN3031 CIS4301 CNT4007
printStudentZone 88887777
)";

    expectedOutput = R"(successful
Student Zone Cost For John Doe: 20
)";
  };

  CampusCompass compass("data/edges.csv", "data/classes.csv");
  std::string actualOutput;

  // somehow pass your input into your CampusCompass and parse it to call the
  // correct functions, for example:

  std::stringstream inp(input);
  int no_of_lines;
  std::string command;
  inp >> no_of_lines;
  inp.ignore(); // ignore newline that first cin left over

  std::stringstream output;
  for (int i = 0; i < no_of_lines; i++) {
    while (!command.compare("")) {
      std::getline(inp, command);
    }
    // parse commands and add to the output
    compass.parseCommand(command, output);
    command = "";
  }

  actualOutput = output.str();

  REQUIRE(actualOutput == expectedOutput);
}

TEST_CASE("Incorrect Commands", "[unit tests]") {
  std::string input;
  std::string expectedOutput;

  SECTION("All incorrect") {
    input = R"(5
foeifokeok kokf defefe
that last one was straight gibberish just for fun
insert "Henry Canapp" 01 1 2 COP3504 EEL3111
remove 2031
cropClass 00000001 COP3504
)";

    expectedOutput = R"(unsuccessful
unsuccessful
unsuccessful
unsuccessful
unsuccessful
)";
  };

  SECTION("Half incorrect") {
    input = R"(4
insert "H3nry" 92813206 23 1 COP3504
insert "Joe Manma" 01201096 3 2 PHY2048 COP3504
removeClass COP3504
toggleEdgesClosure 1 34
)";
    expectedOutput = R"(unsuccessful
successful
1
unsuccessful
)";
  };

  SECTION("More incorrect") {
    input = R"(3
printShortestEdges "Henry"
printStudentZone 1029194827192
isConnected 18
)";
    expectedOutput = R"(unsuccessful
unsuccessful
unsuccessful
)";
  };

  CampusCompass compass("data/edges.csv", "data/classes.csv");
  std::string actualOutput;

  // somehow pass your input into your CampusCompass and parse it to call the
  // correct functions, for example:

  std::stringstream inp(input);
  int no_of_lines;
  std::string command;
  inp >> no_of_lines;
  inp.ignore(); // ignore newline that first cin left over

  std::stringstream output;
  for (int i = 0; i < no_of_lines; i++) {
    while (!command.compare("")) {
      std::getline(inp, command);
    }
    // parse commands and add to the output
    compass.parseCommand(command, output);
    command = "";
  }

  actualOutput = output.str();

  REQUIRE(actualOutput == expectedOutput);
}

TEST_CASE("Edge Cases", "[unit tests]") {
  std::string input;
  std::string expectedOutput;
  input = R"(7
insert "person one" 00000001 2 2 COP3504 PHY2049
remove 00000002
dropClass 00000001 NUL0000
replaceClass 00000001 PHY2049 COP3504
toggleEdgesClosure 4 2 54 2 1 2 4 2 50
printShortestEdges 00000001
printStudentZone 00000001
)";
  expectedOutput = R"(successful
unsuccessful
unsuccessful
unsuccessful
successful
Name: person one
COP3504 | Total Time: -1
PHY2049 | Total Time: -1
Student Zone Cost For person one: 0
)";

  CampusCompass compass("data/edges.csv", "data/classes.csv");
  std::string actualOutput;

  // somehow pass your input into your CampusCompass and parse it to call the
  // correct functions, for example:

  std::stringstream inp(input);
  int no_of_lines;
  std::string command;
  inp >> no_of_lines;
  inp.ignore(); // ignore newline that first cin left over

  std::stringstream output;
  for (int i = 0; i < no_of_lines; i++) {
    while (!command.compare("")) {
      std::getline(inp, command);
    }
    // parse commands and add to the output
    compass.parseCommand(command, output);
    command = "";
  }

  actualOutput = output.str();

  REQUIRE(actualOutput == expectedOutput);
}

TEST_CASE("Benchmarks", "[unit tests]") {
  std::string input;
  std::string expectedOutput;
  SECTION("Remove class functions"){
    input = R"(9
insert "stu uno" 00000001 1 3 PHY2048 COP3504 MAC2313
insert "stu dos" 00000002 36 1 PHY2048
insert "stu tres" 00000003 18 4 PHY2049 COP3504 MAC2313 CDA3101
dropClass 00000003 MAC2313
removeClass PHY2048
replaceClass 00000003 PHY2049 PHY2048
removeClass PHY2048
remove 00000002
remove 00000003
)";
    expectedOutput = R"(successful
successful
successful
successful
2
successful
1
unsuccessful
successful
)";
  };

  SECTION("trapping a student (me)"){
    input = R"(4
insert "Henry Canapp" 92813206 3 4 PHY2049 COT3100 COP3530 EEL3111
printShortestEdges 92813206
toggleEdgesClosure 1 49 56
printShortestEdges 92813206
)";
    expectedOutput = R"(successful
Name: Henry Canapp
COP3530 | Total Time: 17
COT3100 | Total Time: 19
EEL3111 | Total Time: 16
PHY2049 | Total Time: 17
successful
Name: Henry Canapp
COP3530 | Total Time: 17
COT3100 | Total Time: 19
EEL3111 | Total Time: 16
PHY2049 | Total Time: -1
)";
  };
  CampusCompass compass("data/edges.csv", "data/classes.csv");
  std::string actualOutput;

  // somehow pass your input into your CampusCompass and parse it to call the
  // correct functions, for example:

  std::stringstream inp(input);
  int no_of_lines;
  std::string command;
  inp >> no_of_lines;
  inp.ignore(); // ignore newline that first cin left over

  std::stringstream output;
  for (int i = 0; i < no_of_lines; i++) {
    while (!command.compare("")) {
      std::getline(inp, command);
    }
    // parse commands and add to the output
    compass.parseCommand(command, output);
    command = "";
  }

  actualOutput = output.str();

  REQUIRE(actualOutput == expectedOutput);
}

/*
TEST_CASE("Template Test", "[template]") {
  std::string input;
  std::string expectedOutput;
  SECTION(""){
  input = R"()";
  expectedOutput = R"()";
};

CampusCompass compass("../data/edges.csv", "../data/classes.csv");
std::string actualOutput;

// somehow pass your input into your CampusCompass and parse it to call the
// correct functions, for example:

std::stringstream inp(input);
int no_of_lines;
std::string command;
inp >> no_of_lines;
inp.ignore(); // ignore newline that first cin left over

std::stringstream output;
for (int i = 0; i < no_of_lines; i++) {
  while (!command.compare("")) {
    std::getline(inp, command);
  }
  // parse commands and add to the output
  compass.parseCommand(command, output);
  command = "";
}

actualOutput = output.str();

REQUIRE(actualOutput == expectedOutput);
}
*/