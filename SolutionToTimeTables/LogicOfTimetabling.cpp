#include <C:\Users\User\Downloads\json.hpp>
#include <iostream>
// Replaced <bits/stdc++.h> with specific headers
#include <vector>
#include <string>
#include <map>
#include <utility> // For std::move
#include <algorithm> // For std::sort, std::min
#include <chrono>    // For timing (though not used in this snippet)
#include <fstream>   // For file operations
#include <stdexcept> // For std::runtime_error
#include <optional>  // Still useful for other optional types if needed later, but not for Module exams
#include <functional> // For std::function
using namespace std;

enum class WeekDay {MON, TUE, WED, THU, FRI, SAT, SUN};
enum class SlotType {LEC, TUT, REC, SEC, LAB, OTH};

struct TimeSlot {
    WeekDay day;
    int startTime;
    int endTime;
    string location;
    TimeSlot(string d, int s, int e, string loc) : startTime(s), endTime(e), location(move(loc)) {
        static const map<string, WeekDay> strToWeekDayConvert = {
            {"Monday", WeekDay::MON},
            {"Tuesday", WeekDay::TUE},
            {"Wednesday", WeekDay::WED},
            {"Thursday", WeekDay::THU},
            {"Friday", WeekDay::FRI},
            {"Saturday", WeekDay::SAT},
            {"Sunday", WeekDay::SUN}
        };
        auto it = strToWeekDayConvert.find(d);
        if (it != strToWeekDayConvert.end()) {
            day = it->second;
        } else {
            throw runtime_error("Somewhere in JSON, there's an invalid date! It looks like: " + d);
        }
    }

    bool overlap(const TimeSlot& t2) const {
        if (day != t2.day) {
            return false;
        } else {
            return !(startTime >= t2.endTime || endTime <= t2.startTime);
        }
    }
};

struct ModRequire { //Requirements for the module (Lectures, Tutorials Etc.) and available time slots
    SlotType lesson;
    vector<vector<TimeSlot>> options;
    ModRequire(string l, vector<vector<TimeSlot>> o) : options(o) {
        static const map<string, SlotType> strToSlotTypeConvert = {
            {"Lecture", SlotType::LEC},
            {"Tutorial", SlotType::TUT},
            {"Recitation", SlotType::REC},
            {"Sectional Teaching", SlotType::SEC},
            {"Laboratory", SlotType::LAB}
        };
        auto it = strToSlotTypeConvert.find(l);
        if (it != strToSlotTypeConvert.end()) {
            lesson = it->second;
        } else {
            lesson = SlotType::OTH;
        }
    }
};

struct Module {
    string modCode;
    int MCs;
    vector<ModRequire> requirements;
    Module(string m, int M, vector<ModRequire> r) : modCode(move(m)), MCs(M), requirements(move(r)) {}
};

struct ChosenModuleSlot {
    string modCode;
    SlotType lesson;
    vector<TimeSlot> slot;
    ChosenModuleSlot(string m, SlotType l, vector<TimeSlot> s) : modCode(move(m)), lesson(l), slot(move(s)) {}

    bool overlappingSlot(const ChosenModuleSlot& chM2) const {
        for (size_t i{0}; i < slot.size(); i++) {
            for (size_t j{0}; j < chM2.slot.size(); j++) {
                if (slot[i].overlap(chM2.slot[j])) {
                    return true;
                }
            }
        }
        return false;
    }

    bool overlappingTime(const TimeSlot& t2) const {
        for (size_t i{0}; i < slot.size(); i++) {
            if (slot[i].overlap(t2)) {
                return true;
            }
        }
        return false;
    }

    void printModule() const {
        string lessonType;
        string s_slot;
        string e_slot;
        string day;
        switch (lesson) {
            case SlotType::LEC:
                lessonType = "LEC";
                break;
            case SlotType::TUT:
                lessonType = "TUT";
                break;
            case SlotType::REC:
                lessonType = "REC";
                break;
            case SlotType::SEC:
                lessonType = "SEC";
                break;
            case SlotType::OTH:
                lessonType = "OTH";
                break;
            case SlotType::LAB: 
                lessonType = "LAB"; 
                break;
        }
        for (int i = 0; i < slot.size(); i++) {
            switch (slot[i].day) {
                case WeekDay::MON:
                    day = "MON";
                    break;
                case WeekDay::TUE:
                    day = "TUE";
                    break;
                case WeekDay::WED:
                    day = "WED";
                    break;
                case WeekDay::THU:
                    day = "THU";
                    break;
                case WeekDay::FRI:
                    day = "FRI";
                    break;
                case WeekDay::SAT:
                    day = "SAT";
                    break;
                case WeekDay::SUN:
                    day = "SUN";
                    break;
            }
            if (slot[i].startTime < 1000) {
                s_slot = "0" + to_string(slot[i].startTime);
            } else {
                s_slot = to_string(slot[i].startTime);
            }
            if (slot[i].endTime < 1000) {
                e_slot = "0" + to_string(slot[i].endTime);
            } else {
                e_slot = to_string(slot[i].endTime);
            }
            cout << modCode << " " << lessonType << "/" << slot[i].location << ": " << s_slot << " - " << e_slot << "\n";
        }
    }
};

using TimeTable = vector<ChosenModuleSlot>;

void generate_timetables(vector<vector<ChosenModuleSlot>>& allPossible, size_t componentIdx, TimeTable& curr, vector<TimeTable>& all_timetables, vector<TimeSlot> blockedPeriods) {
    size_t totRequirements{allPossible.size()};
    if (componentIdx == totRequirements) {
        all_timetables.push_back(curr);
        return;
    }
    else {
        vector<ChosenModuleSlot> choosingSlot = allPossible[componentIdx];
        for (size_t i{0}; i < choosingSlot.size(); i++) {
            bool clash = false;
            for (size_t j{0}; j < curr.size(); j++) {
                if (choosingSlot[i].overlappingSlot(curr[j])) {
                    clash = true;
                    break;
                }
            }
            for (size_t j{0}; j < blockedPeriods.size(); j++) {
                if (choosingSlot[i].overlappingTime(blockedPeriods[j])) {
                    clash = true;
                    break;
                }
            }
            if (!clash) {
                curr.push_back(choosingSlot[i]);
                generate_timetables(allPossible, componentIdx + 1, curr, all_timetables,blockedPeriods);
                curr.pop_back();
            }
        }
    }
}

vector<TimeTable> allValid(vector<Module> semester, vector<TimeSlot> blockedPeriods) {
    vector<TimeTable> s{};
    TimeTable initial{};
    vector<vector<ChosenModuleSlot>> allPossibleSlots;
    for (size_t i{0}; i < semester.size(); i++) {
        for (size_t j{0}; j < semester[i].requirements.size(); j++) {
            vector<ChosenModuleSlot> possibleSlotsForParticularLesson;
            for (size_t k{0}; k < semester[i].requirements[j].options.size(); k++) {
                ChosenModuleSlot particularSlot(semester[i].modCode, semester[i].requirements[j].lesson, semester[i].requirements[j].options[k]);
                possibleSlotsForParticularLesson.push_back(particularSlot);
            }
            allPossibleSlots.push_back(possibleSlotsForParticularLesson);
        }
    }
    generate_timetables(allPossibleSlots, 0, initial, s, blockedPeriods);
    return s;
}

void printTimetable(const TimeTable& curr) {
    vector<ChosenModuleSlot> mon, tue, wed, thu, fri, sat, sun;
    for (size_t i{0}; i < curr.size(); i++) {
        for (size_t j{0}; j < curr[i].slot.size(); j++) {
            switch (curr[i].slot[j].day) {
                case WeekDay::MON:
                    mon.push_back(ChosenModuleSlot(curr[i].modCode, curr[i].lesson, {curr[i].slot[j]}));
                    break;
                case WeekDay::TUE:
                    tue.push_back(ChosenModuleSlot(curr[i].modCode, curr[i].lesson, {curr[i].slot[j]}));
                    break;
                case WeekDay::WED:
                    wed.push_back(ChosenModuleSlot(curr[i].modCode, curr[i].lesson, {curr[i].slot[j]}));
                    break;
                case WeekDay::THU:
                    thu.push_back(ChosenModuleSlot(curr[i].modCode, curr[i].lesson, {curr[i].slot[j]}));
                    break;
                case WeekDay::FRI:
                    fri.push_back(ChosenModuleSlot(curr[i].modCode, curr[i].lesson, {curr[i].slot[j]}));
                    break;
                case WeekDay::SAT:
                    sat.push_back(ChosenModuleSlot(curr[i].modCode, curr[i].lesson, {curr[i].slot[j]}));
                    break;
                case WeekDay::SUN:
                    sun.push_back(ChosenModuleSlot(curr[i].modCode, curr[i].lesson, {curr[i].slot[j]}));
                    break;
            }
        }
    }
    vector<vector<ChosenModuleSlot>> week = {mon, tue, wed, thu, fri, sat, sun};
    vector<string> weekDayNames = {"MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY", "SUNDAY"};
    for (int i{0}; i < week.size(); i++) {
        for (size_t j{0}; j < week[i].size(); j++) {
            ChosenModuleSlot key{week[i][j]};
            long long int k = j - 1;
            while (k >= 0 and key.slot[0].startTime < week[i][k].slot[0].startTime) {
                week[i][k+1] = week[i][k];
                k -= 1;
            }
            week[i][k+1] = key;
        }
    }
    function<void(vector<ChosenModuleSlot>&)> printDay = [](const vector<ChosenModuleSlot>& chosenDay) -> void {
        for (size_t i{0}; i < chosenDay.size(); i++) {
            chosenDay[i].printModule();
        }
    };
    for (int i{0}; i < week.size(); i++) {
        if (!week[i].empty()) {
            cout << weekDayNames[i] + ": \n";
            printDay(week[i]);
            cout << "\n";
        }
    }
}

void printTimetables(const vector<TimeTable>& all, int noOfOutputTimeTables) {
    if (!all.empty()) {
        if (noOfOutputTimeTables < all.size()) {
            for (int i = 0; i < noOfOutputTimeTables; i++) {
            cout << "Timetable " << i+1 << ": [ \n";
            printTimetable(all[i]);
            cout << "], \n \n";
            }
        }
        else {
            cout << "Note: Not enough timetables than what you suggested! \n";
            for (int i = 0; i < all.size(); i++) {
                cout << "Timetable " << i+1 << ": [ \n";
                printTimetable(all[i]);
                cout << "], \n \n";
            }
        }
    }
    else {
        cout << "How to print? No timetable possible!";
    }
}

int score(TimeTable curr, map<string, int> multipliers) {
    vector<ChosenModuleSlot> mon, tue, wed, thu, fri, sat, sun;
    for (size_t i{0}; i < curr.size(); i++) {
        for (size_t j{0}; j < curr[i].slot.size(); j++) {
            switch (curr[i].slot[j].day) {
                case WeekDay::MON:
                    mon.push_back(ChosenModuleSlot(curr[i].modCode, curr[i].lesson, {curr[i].slot[j]}));
                    break;
                case WeekDay::TUE:
                    tue.push_back(ChosenModuleSlot(curr[i].modCode, curr[i].lesson, {curr[i].slot[j]}));
                    break;
                case WeekDay::WED:
                    wed.push_back(ChosenModuleSlot(curr[i].modCode, curr[i].lesson, {curr[i].slot[j]}));
                    break;
                case WeekDay::THU:
                    thu.push_back(ChosenModuleSlot(curr[i].modCode, curr[i].lesson, {curr[i].slot[j]}));
                    break;
                case WeekDay::FRI:
                    fri.push_back(ChosenModuleSlot(curr[i].modCode, curr[i].lesson, {curr[i].slot[j]}));
                    break;
                case WeekDay::SAT:
                    sat.push_back(ChosenModuleSlot(curr[i].modCode, curr[i].lesson, {curr[i].slot[j]}));
                    break;
                case WeekDay::SUN:
                    sun.push_back(ChosenModuleSlot(curr[i].modCode, curr[i].lesson, {curr[i].slot[j]}));
                    break;
            }
        }
    }
    vector<vector<ChosenModuleSlot>> week = {mon, tue, wed, thu, fri, sat, sun};
    auto freeDaysEvaluate = [week](int e) -> int {
        int count = 0;
        for (int i{0}; i < week.size(); i++) {
            if (week[i].empty()) {
                count += 1;
            } else {
                if (e == 0) {
                    bool elearn = true;
                    for (size_t j{0}; j < week[i].size(); j++) {
                        if (week[i][j].slot[0].location == "E-Learn_C") {
                            elearn = false;
                            break;
                        }
                    }
                    if (elearn) {
                        count += 1;
                    }
                }
            }
        }
        return count;
    };
    
    
    return freeDaysEvaluate(0);
}

using json = nlohmann::json;

int main() {
    ifstream jsonFile("C:\\Users\\User\\repos\\TryingLearnCPP\\SolutionToTimeTables\\ModuleData.json");
    json moduleData;
    jsonFile >> moduleData;
    jsonFile.close();
    vector<Module> semesterModules{};
    for (size_t i{0}; i < moduleData.size(); i++) {
        string modCode = moduleData[i].at("moduleCode").get<string>();
        int moduleCreds{moduleData[i].at("MCs").get<int>()};
        json reqData = moduleData[i]["potentialLessons"];
        vector<ModRequire> modReq;
        for (auto& [lessonT, classes] : reqData.items()) {
            string lessonType = lessonT;
            vector<vector<TimeSlot>> opt;
            for (auto& [classNo, lessonGroup] : classes.items()) {
                vector<TimeSlot> timeSlotGroup;
                for (size_t i{0}; i < lessonGroup.size(); i++) {
                    TimeSlot slot(lessonGroup[i].at("day").get<string>(), lessonGroup[i].at("startTime").get<int>(), lessonGroup[i].at("endTime").get<int>(), lessonGroup[i].at("location").get<string>());
                    timeSlotGroup.push_back(slot);
                }
                opt.push_back(timeSlotGroup);
            }
            modReq.push_back(ModRequire(lessonType, opt));
        }
        semesterModules.push_back(Module(modCode, moduleCreds, modReq));
    }
    vector<TimeSlot> blocked{};
    ifstream jsonBlockedFile("C:\\Users\\User\\repos\\TryingLearnCPP\\SolutionToTimeTables\\BlockPeriodFromUser.json");
    json blockedData;
    jsonBlockedFile >> blockedData;
    jsonBlockedFile.close();
    if (blockedData.size() != 0) {
        map<string, string> mappingForBlock = {
            {"mon", "Monday"},
            {"tue", "Tuesday"},
            {"wed", "Wednesday"},
            {"thu", "Thursday"},
            {"fri", "Friday"},
            {"sat", "Saturday"},
            {"sun", "Sunday"}
        };
        for (size_t i{0}; i < blockedData.size(); i++) {
            blocked.push_back(TimeSlot(mappingForBlock[blockedData[i].at("day").get<string>()],blockedData[i].at("startTime").get<int>(), blockedData[i].at("endTime").get<int>(), "Nil"));
        }
    }
    try {
        vector<TimeTable> allPossible = allValid(semesterModules, blocked);
        printTimetables(allPossible, 3);
    } catch (const exception& e) {
        cerr << "Unexpected Error: " << e.what();
    }
}
//Compiler command to compile updated code: g++ LogicOfTimetabling.cpp -o LogicOfTimetabling.exe