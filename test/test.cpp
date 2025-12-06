#include <catch2/catch_test_macros.hpp>
#include <iostream>

// change if you choose to use a different header name
#include "CampusCompass.h"



// the syntax for defining a test is below. It is important for the name to be
// unique, but you can group multiple tests with [tags]. A test can have
// [multiple][tags] using that syntax.
TEST_CASE("Example Test Name - Change me!", "[tag]") {
  // instantiate any class members that you need to test here
  int one = 1;

  // anything that evaluates to false in a REQUIRE block will result in a
  // failing test
  REQUIRE(one == 0); // fix me!

  // all REQUIRE blocks must evaluate to true for the whole test to pass
  REQUIRE(false); // also fix me!
}

TEST_CASE("Test 2", "[tag]") {
  // you can also use "sections" to share setup code between tests, for example:
  int one = 1;

  SECTION("num is 2") {
    int num = one + 1;
    REQUIRE(num == 2);
  };

  SECTION("num is 3") {
    int num = one + 2;
    REQUIRE(num == 3);
  };

  // each section runs the setup code independently to ensure that they don't
  // affect each other
}

// You must write 5 unique, meaningful tests for credit on the testing section
// of this project!

// See the following for an example of how to easily test your output.
// Note that while this works, I recommend also creating plenty of unit tests for particular functions within your code.
// This pattern should only be used for final, end-to-end testing.

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
