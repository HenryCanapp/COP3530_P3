#include "CampusCompass.h"

#include <string>

CampusCompass::Student::Student(std::string& name, std::string& id, int residence) {
    this->name = name;
    this->id = id;
    this->residence = residence;
}

CampusCompass::~CampusCompass() {
    for (auto& student : students) {
        delete student.second;
        student.second = nullptr;
    }
}

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
        std::cout << "Failed to open file: " << filename << std::endl;
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
        if (!graph.count(first_id)) {
            std::vector<std::tuple<int, int, bool>> edges;
            graph[first_id] = edges;
        }
        graph[first_id].emplace_back(second_id, weight, false);
        //adding edge to second id's list
        if (!graph.count(second_id)) {
            std::vector<std::tuple<int, int, bool>> edges;
            graph[second_id] = edges;
        }
        graph[second_id].emplace_back(first_id, weight, false);
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

bool CampusCompass::parseCommand(const std::string& command, std::stringstream& out) {

    std::vector<std::string> args;
    std::stringstream ss(command);
    std::string arg;

    //grabbing what the command is
    std::string cmd;
    ss >> cmd;

    //adding all the arguments into a list
    //making sure to add quoted arg as one arg in quotes
    bool composite = false;
    std::string full = "";
    while (ss >> arg) {
        if (composite) {
            full += " " + arg;
            if (arg.back() == '\"') {
                composite = false;
                args.push_back(full);
            }
            continue;
        }
        if (arg.front() == '\"') {
            full = arg;
            composite = true;
            continue;
        }
        args.push_back(arg);
    }

    //executing the corresponding method based on the command
    int call = -1;
    if (commands.count(cmd)) {
        call = commands.at(cmd);
    }
    bool is_valid = false;
    switch (call) {
        case 0:
            is_valid = insertStudent(args, out);
            break;
        case 1:
            is_valid = removeStudent(args, out);
            break;
        case 2:
            is_valid = dropClass(args, out);
            break;
        case 3:
            is_valid = replaceClass(args, out);
            break;
        case 4:
            is_valid = removeClass(args, out);
            break;
        case 5:
            is_valid = toggleEdgesClosure(args, out);
            break;
        case 6:
            is_valid = checkEdgeStatus(args, out);
            break;
        case 7:
            is_valid = isConnected(args, out);
            break;
        case 8:
            is_valid = printShortestEdges(args, out);
            break;
        case 9:
            is_valid = printStudentZone(args, out);
            break;
        default:

            break;
    }
    if (!is_valid) {out << "unsuccessful" << std::endl;}
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
            //second clause is overflow prevention
            if (!closed && result[minID].first != INT_MAX && result[to].first > result[minID].first + dist) {
                result[to].first = result[minID].first + dist;
                result[to].second = minID;
            }
        }
    }
    return result;
}

bool CampusCompass::validName(std::string& name) {
    //using regex developed on https://regex101.com
    return std::regex_match(name, std::regex(R"(["][A-Za-z\s]+["])"));
}

bool CampusCompass::validUFID(std::string& id) {
    return std::regex_match(id, std::regex("^[0-9]{8}$"));
}

bool CampusCompass::validClassCode(std::string& code) {
    if (std::regex_match(code, std::regex("^[A-Z]{3}[0-9]{4}$"))) {
        if (classes.count(code)) {
            return true;
        }
    }
    return false;
}

bool CampusCompass::validLocID(int id) {
    if (id > 0 && id < static_cast<int>(graph.size())) {
        return true;
    }
    return false;
}

bool CampusCompass::insertStudent(std::vector<std::string>& args, std::stringstream& out) {
    //checks if name is formatted correctly
    if (!validName(args[0])) {
        return false;
    }
    std::string name = args[0].substr(1, args[0].size() - 2);

    //checks if id is formatted correctly
    if (!validUFID(args[1])) {
        return false;
    }
    std::string ufid = args[1];

    //checks if resisdence location is real
    int residence = std::stoi(args[2]);
    if (!validLocID(residence)) {
        return false;
    }

    //creates the student
    Student* newStudent = new Student(name, ufid, residence);

    //checks if an appropriate number of classes is listed
    int n = stoi(args[3]);
    if (n < 1 || n > 6) {
        return false;
    }

    //checks if there are actually that many classes given
    if (static_cast<int>(args.size()) - 4 != n) {
        return false;
    }

    //adds each class
    for (int i = 4; i < n+4; i++) {
        //validates class format+in the list of classes
        if (!validClassCode(args[i])) {
            return false;
        }
        //default value to be overwritten
        newStudent->classes[args[i]] = -1;
    }

    //add student and then update their shortest path variables
    students[ufid] = newStudent;
    updateStudent(ufid);

    out << "successful" << std::endl;
    return true;
}

bool CampusCompass::removeStudent(std::vector<std::string>& args, std::stringstream& out) {
    //checks format
    if (!validUFID(args[0])) {
        return false;
    }
    if (students.count(args[0])) {
        students.erase(args[0]);
        out << "successful" << std::endl;
        return true;
    }
    return false;
}

bool CampusCompass::dropClass(std::vector<std::string>& args, std::stringstream& out) {
    if (!validUFID(args[0])) {
        return false;
    }

    if (!students.count(args[0])) {
        return false;
    }

    if (!validClassCode(args[1])) {
        return false;
    }

    //first checks if student is enrolled in class
    Student& stu = *students[args[0]];
    if (stu.classes.count(args[1])) {
        stu.classes.erase(args[1]);
        //checks if student needs to be removed or updated
        if (stu.classes.empty()) {
            students.erase(args[0]);
        } else {
            updateStudent(args[0]);
        }
        out << "successful" << std::endl;
        return true;
    }
    return false;
}

bool CampusCompass::replaceClass(std::vector<std::string>& args, std::stringstream& out) {
    if (!validUFID(args[0])) {
        return false;
    }

    if (!validClassCode(args[1]) || !validUFID(args[2])) {
        return false;
    }

    //checks if student is already enrolled in new class
    if (!students[args[0]]->classes.count(args[2])) {
        //drops the old class and adds the new class
        std::vector drop_args(args.begin(), args.begin() + 2);
        //so dropclass doesnt affect output string
        std::stringstream dumm;
        if (dropClass(drop_args, dumm)) {
            students[args[0]]->classes[args[2]] = -1;
            updateStudent(args[0]);
            out << "successful" << std::endl;
            return true;
        }
    }
    return false;
}

bool CampusCompass::removeClass(std::vector<std::string>& args, std::stringstream& out) {
    if (!validClassCode(args[0])) {
        return false;
    }

    //so dropClass doesn't affect output string
    std::stringstream dumm;
    //checks if each student has a class, if they do, then drop the class
    int amnt = 0;
    for (auto&[id, obj] : students) {
        if (std::vector drop_args = {id, args[0]}; dropClass(drop_args, dumm)) {
            amnt++;
        }
    }
    out << amnt << std::endl;
    return true;
}

bool CampusCompass::checkEdgeStatus(std::vector<std::string>& args, std::stringstream& out) {
    int from = std::stoi(args[0]);
    int to = std::stoi(args[1]);
    if (!validLocID(from) || !validLocID(to)) {
        out << "DNE" << std::endl;
        return true;
    }
    //finds the edge and then reads it's status
    std::string result = "DNE";
    for (auto [id, dist, closed] : graph[from]) {
        if (id == to) {
             result = closed ? "closed" : "open";
        }
    }
    out << result << std::endl;
    return true;
}

bool CampusCompass::toggleEdgesClosure(std::vector<std::string>& args, std::stringstream& out) {
    //checks there are the correct number of arguments
    int n = std::stoi(args[0]);
    if (static_cast<int>(args.size()) != 2*n + 1) {
        return false;
    }
    //goes through each edge pair
    for (int i = 1; i < 2*n + 1; i += 2) {
        int from = std::stoi(args[i]);
        int to = std::stoi(args[i+1]);
        if (!validLocID(from) || !validLocID(to)) {
            return false;
        }
        //flips the closed status in both vertices edge lists
        bool inList = false;
        for (auto& edge : graph[from]) {
            if (std::get<0>(edge) == to) {
                std::get<2>(edge) = !std::get<2>(edge);
                inList = true;
                break;
            }
        }
        if (!inList) {
            return false;
        }
        inList = false;
        for (auto& edge : graph[to]) {
            if (std::get<0>(edge) == from) {
                std::get<2>(edge) = !std::get<2>(edge);
                inList = true;
                break;
            }
        }
        if (!inList) {
            return false;
        }
    }

    //update the students
    for (auto& [name, obj] : students) {
        updateStudent(obj->id);
    }

    out << "successful" << std::endl;
    return true;
}

bool CampusCompass::isConnected(std::vector<std::string>& args, std::stringstream& out) {
    int from = std::stoi(args[0]);
    int to = std::stoi(args[1]);
    if (!validLocID(from) || !validLocID(to)) {
        return false;
    }
    //breadth first search using a queue
    std::queue<int> qu;
    qu.push(from);
    std::set<int> checked;
    while (!qu.empty()) {
        int top = qu.front();
        qu.pop();
        checked.insert(top);
        for (auto [id, dist, closed] : graph[top]) {
            if (closed) {continue;}
            if (id == to) {
                out << "successful" << std::endl;
                return true;
            }
            if (!checked.count(id)) {
                qu.push(id);
                checked.insert(id);
            }
        }
    }
    return false;
}

void CampusCompass::updateStudent(std::string& id) {
    Student& stu = *students[id];
    int start = stu.residence;
    //get the dijkstra's list of shortest distances
    auto dj_list = shortestDistance(start);
    //go through each class a student has and update the distance values and path
    for (auto& dest : stu.classes) {
        int loc_id = classes[dest.first];
        if (dj_list[loc_id].first != INT_MAX) {
            //sets the distance to correct value
            dest.second = dj_list[loc_id].first;
            //create the path from dest to residence
            std::vector<int> path;
            int from = dj_list[loc_id].second;
            while (from != start) {
                path.push_back(from);
                from = dj_list[from].second;
            }
            stu.paths[loc_id] = path;
        }
    }
}

bool CampusCompass::printShortestEdges(std::vector<std::string>& args, std::stringstream& out) {
    if (!validUFID(args[0])) {
        return false;
    }
    Student& stu = *students[args[0]];
    out << "Name: " << stu.name << std::endl;
    //prints the time distance to each class
    for (const auto&[class_name, time] : stu.classes) {
        out << class_name << " | Total Time: " << time << std::endl;
    }
    return true;
}

std::unordered_map<int, std::vector<std::pair<int, int>>> CampusCompass::findMST(std::set<int>& locations) {
    std::unordered_map<int, std::vector<std::pair<int, int>>> mst;
    //start with an arbitrary location
    int first = *locations.begin();
    mst[first] = std::vector<std::pair<int, int>>();

    while (mst.size() != locations.size()) {
        //find the abs_shortest edges among all available edges in form of <from, <to, dist>>
        std::pair<int, std::pair<int, int>> abs_shortest = std::make_pair(0, std::make_pair(0, INT_MAX));
        for (auto it = mst.begin(); it != mst.end(); it++) {
            int curr = it->first;
            //find it's shortest edge to a vertex in the subgraph NOT already added
            for (auto [loc, dist, closed] : graph[curr]) {
                //only adds edges that meet mst criteria
                if (!closed && locations.count(loc) && !mst.count(loc)) {
                    if (dist < abs_shortest.second.second) {
                        abs_shortest.second.first = loc;
                        abs_shortest.second.second = dist;
                        abs_shortest.first = curr;
                    }
                }
            }
        }
        int from = abs_shortest.first;
        int to = abs_shortest.second.first;
        //add the edge to the first vertex's list
        mst[from].push_back(abs_shortest.second);
        //add the vertex if it doesn't exist
        if (!mst.count(to)) {
            mst[to] = std::vector<std::pair<int, int>>();
        }
        //add the edge to the second vertex's list
        mst[to].push_back(std::make_pair(from, abs_shortest.second.second));
    }
    return mst;
}

bool CampusCompass::printStudentZone(std::vector<std::string> &args, std::stringstream& out) {
    if (!validUFID(args[0])) {
        return false;
    }
    //create a set of all the locations traveled to to get to classes
    auto& paths = students[args[0]]->paths;
    std::set<int> locations;
    locations.insert(students[args[0]]->residence);
    for (auto it = paths.begin(); it != paths.end(); ++it) {
        locations.insert(it->first);
        for (int fol : it->second) {
            locations.insert(fol);
        }
    }
    auto mst = findMST(locations);
    std::set<std::pair<int, int>> seen;
    int total_cost = 0;
    //sum the cost of all the edges in
    for (auto it = mst.begin(); it != mst.end(); ++it) {
        for (auto [loc, dist] : it->second) {
            std::pair<int, int> edge1 = std::make_pair(it->first, loc);
            std::pair<int, int> edge2 = std::make_pair(loc, it->first);
            //ensuring that this is a unique edge not already counted
            if (!seen.count(edge1) && !seen.count(edge2)) {
                total_cost += dist;
                seen.insert(edge1);
            }
        }
    }
    out << "Student Zone Cost For " << students[args[0]]->name << ": " << total_cost << std::endl;
    return true;
}
