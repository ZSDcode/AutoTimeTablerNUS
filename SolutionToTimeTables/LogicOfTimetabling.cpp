#include <iostream>
#include <bits/stdc++.h>
#include <chrono>
#include <ctime>
// #include <C:\Users\User\Downloads\json.hpp>
#include <fstream>
using namespace std;

enum class WeekDay {MON, TUE, WED, THU, FRI, SAT, SUN};
enum class SlotType {LEC, TUT, REC, SEM, SEC, LAB, OTH};

struct TimeSlot {
    WeekDay day;
    int startTime;
    int endTime;
    string location;
    TimeSlot(WeekDay d, int s, int e, string loc) : day(d), startTime(s), endTime(e), location(move(loc)) {}

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
    vector<TimeSlot> options;
    ModRequire(SlotType l, vector<TimeSlot> o) : lesson(l), options(move(o)) {}
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
    TimeSlot slot;
    ChosenModuleSlot(string m, SlotType l, TimeSlot s) : modCode(move(m)), lesson(l), slot(s) {}

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
            case SlotType::SEM:
                lessonType = "SEM";
                break;
            case SlotType::OTH:
                lessonType = "OTH";
                break;
        }
        switch (slot.day) {
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
        if (slot.startTime < 1000) {
            s_slot = "0" + to_string(slot.startTime);
        } else {
            s_slot = to_string(slot.startTime);
        }
        if (slot.endTime < 1000) {
            e_slot = "0" + to_string(slot.endTime);
        } else {
            e_slot = to_string(slot.endTime);
        }
        cout << modCode << " " << lessonType << "/" << slot.location << ": " << s_slot << " - " << e_slot << "\n";
    }
};

using TimeTable = vector<ChosenModuleSlot>;

void generate_timetables(vector<vector<ChosenModuleSlot>>& allPossible, size_t componentIdx, TimeTable& curr, vector<TimeTable>& all_timetables) {
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
                if (choosingSlot[i].slot.overlap(curr[j].slot)) {
                    clash = true;
                    break;
                }
            }
            if (!clash) {
                curr.push_back(choosingSlot[i]);
                generate_timetables(allPossible, componentIdx + 1, curr, all_timetables);
                curr.pop_back();
            }
        }
    }
}

vector<TimeTable> allValid(vector<Module> semester) {
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
    generate_timetables(allPossibleSlots, 0, initial, s);
    return s;
}

void printTimetable(const TimeTable& curr) {
    vector<ChosenModuleSlot> mon, tue, wed, thu, fri, sat, sun;
    for (size_t i{0}; i < curr.size(); i++) {
        switch (curr[i].slot.day) {
            case WeekDay::MON:
                mon.push_back(curr[i]);
                break;
            case WeekDay::TUE:
                tue.push_back(curr[i]);
                break;
            case WeekDay::WED:
                wed.push_back(curr[i]);
                break;
            case WeekDay::THU:
                thu.push_back(curr[i]);
                break;
            case WeekDay::FRI:
                fri.push_back(curr[i]);
                break;
            case WeekDay::SAT:
                sat.push_back(curr[i]);
                break;
            case WeekDay::SUN:
                sun.push_back(curr[i]);
                break;
        }
    }
    vector<vector<ChosenModuleSlot>> week = {mon, tue, wed, thu, fri, sat, sun};
    vector<string> weekDayNames = {"MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY", "SUNDAY"};
    for (int i{0}; i < week.size(); i++) {
        for (size_t j{0}; j < week[i].size(); j++) {
            ChosenModuleSlot key{week[i][j]};
            long long int k = j - 1;
            while (k >= 0 and key.slot.startTime < week[i][k].slot.startTime) {
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

int main() {
    Module m1 ("MA1522", 4, {ModRequire(SlotType::LEC, {TimeSlot(WeekDay::MON, 800, 1000, "LT11"), TimeSlot(WeekDay::WED, 1600, 1800, "LT11")}), ModRequire(SlotType::LEC, {TimeSlot(WeekDay::THU, 800, 1000, "LT11"), TimeSlot(WeekDay::FRI, 1600, 1800, "LT11")})});
    Module m2 ("ST1131", 4, {ModRequire(SlotType::LEC, {TimeSlot(WeekDay::MON, 1600, 1800, "LT32")}), ModRequire(SlotType::TUT, {TimeSlot(WeekDay::TUE, 900, 1000, "S16"), TimeSlot(WeekDay::TUE, 1000, 1100, "S16"), TimeSlot(WeekDay::TUE, 1400, 1500, "S16"), TimeSlot(WeekDay::WED, 900, 1000, "S16"), TimeSlot(WeekDay::WED, 1000, 1100, "S16"), TimeSlot(WeekDay::WED, 1100, 1200, "S16"), TimeSlot(WeekDay::WED, 1200, 1300, "S16"), TimeSlot(WeekDay::WED, 1400, 1500, "S16"), TimeSlot(WeekDay::THU, 1200, 1300, "S16"), TimeSlot(WeekDay::THU, 1300, 1400, "S16")})});
    vector<TimeTable> allPossible {allValid({m1, m2})};
    printTimetable(allPossible[0]);
}

//Compiler command to compile updated code: g++ LogicOfTimetabling.cpp -o LogicOfTimetabling.exe