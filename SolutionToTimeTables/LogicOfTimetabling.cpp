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
#include <filesystem>
#include <algorithm>
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

vector<vector<ChosenModuleSlot>> sortDaysOnTime(TimeTable& curr) {
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
    return week;
}

void printTimetable(TimeTable& curr) {
    vector<vector<ChosenModuleSlot>> week = sortDaysOnTime(curr);
    function<void(vector<ChosenModuleSlot>&)> printDay = [](const vector<ChosenModuleSlot>& chosenDay) -> void {
        for (size_t i{0}; i < chosenDay.size(); i++) {
            chosenDay[i].printModule();
        }
    };
    vector<string> weekDayNames = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    for (int i{0}; i < week.size(); i++) {
        if (!week[i].empty()) {
            cout << weekDayNames[i] + ": \n";
            printDay(week[i]);
            cout << "\n";
        }
    }
}

void printTimetables(vector<TimeTable>& all, int noOfOutputTimeTables) {
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

using json = nlohmann::json;

string locParse(const string& location) {
    string u = "Unknown Loc";
    if (location[0] == 'A') {
        if (location.substr(0, 2) == "AS") {
            return location.substr(0, 3);
        }
        else {
            return u;
        }
    }
    else if (location[0] == 'B') {
        if (location.substr(0, 3) == "BIZ") {
            return location.substr(0, 4);
        }
        else {
            return u;
        }
    }
    else if (location[0] == 'C') {
        if (location.substr(0, 4) == "CAPT") {
            return "RC";
        } else if (location.substr(0, 4) == "CELC") {
            return "CELC";
        } else if (location.substr(0, 4) == "CELS") {
            return "CELS";
        } else if (location.substr(0, 3) == "COM") {
            return location.substr(0, 4);
        } else if (location.substr(0, 3) == "CQT") {
            return "S15";
        } else {
            return u;
        }
    }
    else if (location[0] == 'E') {
        if (location.substr(0, 3) == "ERC") {
            return "ERC";
        } else if (location.substr(0, 2) == "EW") {
            if (location[2] == '1') {
                return "EW1";
            } else {
                return "E8";
            }
        } else if (location.substr(0, 2) == "EA" or location == "ENG-AUD") {
            return "EA";
        } else if (location.substr(0, 2) == "EC") {
            return "EC";
        } else if (isdigit(location[1])) {
            if (location[2] == 'A') {
                return location.substr(0, 3);
            } else {
                return location.substr(0, 2);
            }
        } else {
            return u;
        }
    }
    else if (location.substr(0, 3) == "FED") {
        return "LAW";
    }
    else if (location[0] == 'F') {
        return "Frontier";
    }
    else if (location.substr(0, 3) == "HSS") {
        return "HSS";
    }
    else if (location[0] == 'I') {
        if (location.substr(0, 2) == "I3" || location == "Interaction") {
            return "I3";
        } else if (location.substr(0, 2) == "I4") {
            return "I4";
        } else {
            return u;
        }
    }
    else if (location.substr(0, 3) == "LAW" || location.substr(0, 3) == "LKY") {
        return "LAW";
    }
    else if (location.substr(0, 2) == "LT") {
        if (location.length() == 3) {
            return location;
        } else if (location == "LT7A") {
            return "EA";
        } else if (location.length() >= 4) {
            return location.substr(0, 4);
        }
    } else if (location.substr(0, 2) == "MD") {
        if (isdigit(location[3])) {
            return location.substr(0, 4);
        } else if (location.substr(0, 4) == "MD4A") {
            return "MD4A";
        } else {
            return location.substr(0, 3);
        }
    } else if (location == "NAK-AUD") {
        return "ERC";
    } else if (location.substr(0, 2) == "RC") {
        return location.substr(0, 3);
    } else if (location.substr(0, 3) == "RVR") {
        return location.substr(0, 3);
    } else if (location.substr(0, 3) == "RMI") {
        return "I3";
    } else if (location[0] == 'S') {
        if (isdigit(location[1])) {
            if (isalpha(location[2]) or isdigit(location[2])) {
                return location.substr(0, 3);
            } else {
                return location.substr(0, 2);
            }
        } else if (location.substr(0, 3) == "SDE") {
            if (!isdigit(location[3])) {
                return "SDE2";
            } else {
                return location.substr(0, 4);
            }
        } else if (location == "SR_LT19") {
            return "COM2";
        } else if (location == "SPSWETLAB") {
            return "S16";
        }
    } else if (location.substr(0, 2) == "TC") {
        return "RC";
    } else if (location.substr(0, 2) == "TP") {
        return "UT";
    } else if (location[0] == 'U') {
        if (location == "UTSRC-SR4") {
            return "ERC";
        } else if (location == "UT-AUD3") {
            return "RC";
        } else {
            return "UT";
        }
    } else if (location[0] == 'Y') {
        return "YST";
    }
    return u;
}

static const map<string, vector<string>> closestBuildings = {
    {"AS1", {"AS2", "AS3", "LT9"}},
    {"AS2", {"LT11", "LT12", "LT13", "AS1", "AS3"}},
    {"AS3", {"LT12", "LT13", "AS1", "AS2", "AS4"}},
    {"AS4", {"AS3", "AS5"}},
    {"AS5", {"LT8", "AS4"}},
    {"AS6", {"LT14", "LT15"}},
    {"AS8", {"AS6", "AS1"}},
    {"BIZ1", {"HSS"}},
    {"CELC", {"SDE2"}},
    {"CELS", {"S6", "MD2"}},
    {"COM1", {"COM2", "COM3"}},
    {"COM2", {"COM1", "COM3", "LT16", "LT17", "LT18", "LT19", "HSS"}},
    {"COM3", {"COM1", "COM2", "COM4"}},
    {"COM4", {"COM3", "I4"}},
    {"BIZ2", {"LT19", "LT18", "I3"}},
    {"E1", {"E1A", "EW1", "E2"}},
    {"E1A", {"E1", "EA", "E2"}},
    {"E2", {"E1A", "E1", "LT1", "LT2"}},
    {"E3", {"LT7", "E4"}},
    {"E3A", {"EA"}},
    {"E4", {"E1", "LT6", "E4A", "E5", "E3"}},
    {"E4A", {"E4"}},
    {"E5", {"E4"}},
    {"E5A", {"E3"}},
    {"E6", {"E7"}},
    {"E7", {"E6"}},
    {"EA", {"LT7", "E2", "E1A", "E3A"}},
    {"ERC", {"RC"}},
    {"Frontier", {"S10", "S16", "LT26", "MD9", "MD7"}},
    {"HSS", {"COM2", "LT18"}},
    {"I3", {"BIZ2"}},
    {"I4", {"COM4"}},
    {"LT1", {"E2", "LT2", "E3", "LT7"}},
    {"LT2", {"LT1", "E2"}},
    {"LT3", {"LT4"}},
    {"LT4", {"LT3"}},
    {"LT6", {"E4"}},
    {"LT7", {"EA", "LT1"}},
    {"LT8", {"AS5"}},
    {"LT9", {"AS1", "AS4", "LT10"}},
    {"LT10", {"AS1", "AS4", "LT9"}},
    {"LT11", {"AS2"}},
    {"LT12", {"AS3", "AS2", "LT13"}},
    {"LT13", {"AS3", "AS2", "LT12"}},
    {"LT14", {"AS6", "LT15"}},
    {"LT15", {"AS6", "LT14"}},
    {"LT16", {"COM2"}},
    {"LT17", {"COM2"}},
    {"LT18", {"BIZ2"}},
    {"LT19", {"COM2", "BIZ2"}},
    {"LT20", {"S3"}},
    {"LT21", {"S5"}},
    {"LT26", {"Frontier", "MD9", "MD7"}},
    {"LT27", {"LT28", "LT29", "S16", "Frontier", "LT31"}},
    {"LT28", {"LT27", "LT29", "S16", "Frontier", "LT31"}},
    {"LT29", {"LT27", "LT28", "S16", "Frontier", "LT31"}},
    {"LT31", {"S16", "S15", "S14", "Frontier", "LT27", "LT28", "LT29"}},
    {"LT32", {"S1", "S3", "S1A"}},
    {"LT33", {"S17", "LT34"}},
    {"LT34", {"S17", "LT33"}},
    {"MD1", {"MD4", "MD3", "S5"}},
    {"MD2", {"MD3"}},
    {"MD3", {"MD2", "MD1", "MD4", "MD4A", "MD5"}},
    {"MD4", {"MD4A", "MD5", "MD7"}},
    {"MD4A", {"MD4", "MD5", "MD3", "MD1"}},
    {"MD5", {"MD4", "MD4A", "MD7", "MD3", "MD6"}},
    {"MD6", {"MD5", "MD11"}},
    {"MD7", {"MD5", "MD4", "LT26"}},
    {"MD9", {"LT26", "LT27", "LT28", "LT29", "Frontier", "MD10"}},
    {"MD10", {"MD9", "MD11"}},
    {"MD11", {"MD6", "MD10"}},
    {"RC", {"TC"}},
    {"S1", {"S1A", "LT32", "S2"}},
    {"S1A", {"S1", "S3"}},
    {"S2", {"S1", "S3"}},
    {"S3", {"S2", "S4", "LT20"}},
    {"S4", {"S4A", "S3", "S5", "LT21"}},
    {"S5", {"S4", "S6", "S10", "LT21"}},
    {"S6", {"S5", "CELS"}},
    {"S7", {"S8", "S11"}},
    {"S8", {"S7", "S10"}},
    {"S10", {"S8", "Frontier"}},
    {"S11", {"S7", "S12"}},
    {"S12", {"S11", "S13"}},
    {"S13", {"S12", "S14"}},
    {"S14", {"S13", "S15"}},
    {"S15", {"S14", "S16", "S17"}},
    {"S16", {"S15", "S14", "Frontier"}},
    {"S17", {"LT34", "S15"}},
    {"SDE1", {"SDE2", "SDE3", "SDE4", "EW1"}},
    {"SDE2", {"CELC", "SDE1"}},
    {"SDE3", {"SDE4", "SDE1"}},
    {"SDE4", {"SDE1", "SDE3"}},
    {"TC", {"RC"}},
};

bool checkClose(const string& loc1, const string& loc2) {
    if (loc1 == loc2) {
        return true;
    } else {
        bool close = false;
        if (closestBuildings.count(loc1)){
            if (!closestBuildings.at(loc1).empty()) {
                for (size_t j{0}; j < closestBuildings.at(loc1).size(); j++) {
                    if (loc2 == closestBuildings.at(loc1)[j]) {
                        close = true;
                        break;
                    }
                }
            }
        }
        if (!close) {
            if (closestBuildings.count(loc2)) {
                if (!closestBuildings.at(loc2).empty()){
                    for (size_t j{0}; j < closestBuildings.at(loc2).size(); j++) {
                        if (loc1 == closestBuildings.at(loc2)[j]) {
                            close = true;
                            break;
                        }
                    }
                }
            }
        }
        return close;
    }
}

int score(TimeTable& curr, json& multipliers) {
    vector<vector<ChosenModuleSlot>> week = sortDaysOnTime(curr);
    auto freeDaysCounter = [&week]() -> int {
        int count = 0;
        for (int i{0}; i < week.size(); i++) {
            if (week[i].empty()) {
                count += 1;
            } else {
                bool elearn = true;
                for (size_t j{0}; j < week[i].size(); j++) {
                    if (week[i][j].slot[0].location != "E-Learn_C") {
                        elearn = false;
                        break;
                    }
                }
                if (elearn) {
                    count += 1;
                }
            }
        }
        return count;
    };

    auto startTimeIterator = [&](int i) -> int {
        int startTime = 0;
        if (week[i].empty()) {return 0;}
        for (int idx = 0; idx < week[i].size(); ++idx) {
            if (week[i][idx].slot[0].location != "E-Learn_C") {
                startTime = week[i][idx].slot[0].startTime;
                break;
            }
        }
        return startTime;
    };

    auto startTimeVect = [&]() -> vector<float> {
        vector<float> startTimes{};
        for (size_t i{0}; i < week.size(); i++) {
            if (week[i].empty()) {
                startTimes.push_back(0);
            } else {
                int startingTime = startTimeIterator(i);
                startTimes.push_back(floor(startingTime/100) + static_cast<float>(startingTime%100)/60);
            }
        }
        return startTimes;
    };

    auto endTimeIterator = [&](int i) -> int {
        int endTime{0};
        if (week[i].empty()) {return 0;}
        for (size_t j{week[i].size()-1}; j > -1; j--) {
            if (week[i][j].slot[0].location != "E-Learn_C") {
                endTime = week[i][j].slot[0].endTime;
                break;
            }
        }
        return endTime;
    };

    auto endTimeVect = [&]() -> vector<float> {
        vector<float> endTimes{};
        for (size_t i{0}; i < week.size(); i++) {
            if (week[i].empty()) {
                endTimes.push_back(0);
            } else {
                int endingTime = endTimeIterator(i);
                endTimes.push_back(floor(endingTime/100) + static_cast<float>(endingTime%100)/60);
            }
        }
        return endTimes;
    };

    auto gapTimeVect = [&week]() -> vector<float> {
        vector<float> gapTimes{};
        for (size_t i{0}; i < week.size(); i++) {
            float gapSum{0};
            if (week[i].empty()) {
                gapTimes.push_back(0);
                continue;
            }
            vector<TimeSlot> inPersonClasses{};
            if (!week[i].empty()) {
                for (size_t j{0}; j < week[i].size(); j++) {
                    if (week[i][j].slot[0].location != "E-Learn_C") {
                        inPersonClasses.push_back(week[i][j].slot[0]);
                    }
                }
            } 
            if (inPersonClasses.size() <= 1) {
                gapTimes.push_back(0);
                continue;
            }
            else {
                for (size_t j{0}; j < inPersonClasses.size()-1; j++) {
                    int k = j + 1;
                    float startTime = static_cast<float>(floor(inPersonClasses[k].startTime / 100)) + static_cast<float>(inPersonClasses[k].startTime % 100) / 60.0f;
                    float endTime = static_cast<float>(floor(inPersonClasses[j].endTime / 100)) + static_cast<float>(inPersonClasses[j].endTime % 100) / 60.0f;
                    float gap = startTime - endTime;
                    gapSum += gap;
                }
                gapTimes.push_back(gapSum);
            }
        }
        return gapTimes;
    };

    float score{0};
    int freeDays = freeDaysCounter();
    vector<float> startTimes = startTimeVect();
    vector<float> endTimes = endTimeVect();
    vector<float> gapTimes = gapTimeVect();
    score -= static_cast<float>(freeDays/week.size()) * multipliers.at("freeDayMult").get<float>();
    float earliest = multipliers["hours"][0].get<float>();
    float latest = multipliers["hours"][1].get<float>();
    float longestDay = latest - earliest;
    for (size_t i{0}; i < startTimes.size(); i++) {
        if (startTimes[i] == 0) {
            continue;
        } else {
            score += (24.0f - startTimes[i])/longestDay * multipliers.at("startTimeMult").get<float>();
            score += (endTimes[i] - earliest)/longestDay * multipliers.at("endTimeMult").get<float>();
            if (week[i].size() <= 1) {
                continue;
            } else {
                score += fabs(gapTimes[i] - multipliers["gapTimeMult"][0].get<float>()) * multipliers["gapTimeMult"][1].get<float>();
            }
        }
    }

    auto locationBasedScoring = [&]() -> vector<int> {
        if (multipliers["locationInputs"][0].get<int>() == 0) {
            return {};
        } else {
            vector<int> noGapsFarApart = {};
            for (size_t i{0}; i < week.size(); i++) {
                int noGapsFarApartToday{0};
                if (week[i].empty() || week[i].size() <= 1) {
                    noGapsFarApart.push_back(0);
                    continue;
                }
                for (size_t j{0}; j < week[i].size()-1; j++) {
                    if (week[i][j].slot[0].endTime == week[i][j+1].slot[0].startTime) {
                        string location1 = week[i][j].slot[0].location;
                        string location2 = week[i][j+1].slot[0].location;
                        if (location1 == "E-Learn_C" || location2 == "E-Learn_C") {
                            continue;
                        } else {
                            location1 = locParse(location1);
                            location2 = locParse(location2);
                            if (!checkClose(location1, location2)) {
                                noGapsFarApartToday += 1;
                            }
                        }
                    }
                }
                noGapsFarApart.push_back(noGapsFarApartToday);
            }
            return noGapsFarApart;
        }
    };
    vector<int> locationCounts = locationBasedScoring();
    if (!locationCounts.empty()) {
        for (size_t i{0}; i < locationCounts.size(); i++) {
            if (locationCounts[i] == 0) {
                if (week[i].empty()) {
                    continue;
                } else {
                    score -= 0.5 * multipliers["locationInputs"][1].get<float>();
                }
            }
            else {
                score += locationCounts[i] * multipliers["locationInputs"][1].get<float>();
            }
        }
    }
    return score;
}

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    fs::path basePath = fs::path(argv[0]).parent_path();
    fs::path jsonFilePath = basePath / "ModuleData.json";
    fs::path jsonBlockedFilePath = basePath / "BlockPeriodFromUser.json";
    fs::path jsonMultipliersPath = basePath / "Multipliers.json";
    ifstream jsonFile(jsonFilePath);
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
    ifstream jsonBlockedFile(jsonBlockedFilePath);
    json blockedData;
    jsonBlockedFile >> blockedData;
    jsonBlockedFile.close();
    ifstream jsonMult(jsonMultipliersPath);
    json mult;
    jsonMult >> mult;
    jsonMult.close();
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
        auto start = std::chrono::high_resolution_clock::now();
        vector<TimeTable> allPossible = allValid(semesterModules, blocked);
        cout << allPossible.size() << "\n";
        sort(allPossible.begin(), allPossible.end(), [&](TimeTable& a, TimeTable& b) {
            return score(a, mult) < score(b, mult);
        });
        printTimetables(allPossible, mult.at("noOfTimetables").get<int>());
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        cout << "Execution time: " << duration.count() << " seconds." << "\n";
    } catch (const exception& e) {
        cerr << "Unexpected Error: " << e.what();
    }
}
//Compiler command to compile updated code: g++ LogicOfTimetabling.cpp -o LogicOfTimetabling.exe