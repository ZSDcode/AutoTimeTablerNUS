import requests
import json
import subprocess
import datetime
import os
import math

def defaultValueErrorString(a, b):
    return f"Please enter an integer between {a} and {b}"

def gettingSemesterInput():
    while True:
        semesterNoStr = input("Which semester are you planning for (1 - 4, 3 and 4 means special term): ")
        try:
            if ('.' in semesterNoStr):
                print(defaultValueErrorString(1, 4))
            else: 
                semNo = int(semesterNoStr)
                if (semNo <= 0 or semNo > 4):
                    print(defaultValueErrorString(1, 4))
                else:
                    return semNo
        except ValueError as e:
            print(defaultValueErrorString(1, 4))
        except Exception as e:
            print(f"An Unknown Error Occured: {e} \n Please Try Again :(")

def gettingNoOfModules():
    while True:
        noOfModulesStr = input("How many Module do you plan on taking this semester (Integer 1 - 20): ")
        try:
            if('.' in noOfModulesStr):
                print(defaultValueErrorString(1, 20))
            else:
                noOfMods = int(noOfModulesStr)
                if (noOfMods <= 0 or noOfMods > 20):
                    print(defaultValueErrorString(1, 20))
                else:
                    return noOfMods
        except ValueError as e:
            print(defaultValueErrorString(1, 20))
        except Exception as e:
            print(f"An Unknown Error Occured: {e} \n Please Try Again :(")

def getAcadYear():
    dateNow = datetime.datetime.now()
    currYear = dateNow.year
    currMth = dateNow.month
    if (currMth >= 1 and currMth <= 5):
        return currYear-1
    else:
        return currYear
    
currAcadYear = getAcadYear()

def generate_modAPIURL(modCode):
    yearKey = f"{currAcadYear}-{currAcadYear+1}"
    return f"https://api.nusmods.com/v2/{yearKey}/modules/{modCode}.json"

def inputMod(semesterNo):
    while True:
        modCode = input("Please key in a Module (Press b to remove the previous added mod): ").upper()
        try:
            if (modCode == 'B'):
                return modCode
            else:
                response = requests.get(generate_modAPIURL(modCode))
                response.raise_for_status()
                data = response.json()
                semesterAvail = []
                for i in range(len(data["semesterData"])):
                    semesterAvail.append(data["semesterData"][i]["semester"])
                if (semesterNo not in semesterAvail):
                    print("Module is not available in the semester")
                else:
                    return modCode
        except requests.exceptions.HTTPError as e:
            print("Are you sure the module is valid?")
        except requests.exceptions.ConnectionError as e:
            print("Get better connection, then try again!")
        except Exception as e:
            print(f"An Unknown Error Occured: {e} \n Please Try Again :(")

def generate_locAPIURL(semester):
    yearKey = f"{currAcadYear}-{currAcadYear+1}"
    return f"https://api.nusmods.com/v2/{yearKey}/semesters/{semester}/venues.json"

def gettingModules(noMods):
    modules = []
    def pushingMod(count):
        print(modules)
        if (count == noMods):
            return
        else:
            modCode = inputMod(semNo)
            if (modCode == 'B'):
                if (count == 0):
                    count = count - 1
                else:
                    modules.pop()
                    pushingMod(count-1)
            elif (modCode in modules):
                print("You already keyed in that Module!")
                pushingMod(count)
            else:
                modules.append(modCode)
        pushingMod(count+1)
    pushingMod(0)
    return modules

semNo = gettingSemesterInput()
noOfMods = gettingNoOfModules()
modCodeList = gettingModules(noOfMods)
modData = []

def gettingNumberOfBlockPeriods():
    while True:
        try:
            numberStr = input("How many block periods would you like to input: ")
            if ('.' in numberStr):
                print("Please input a non-negative integer")
            else:
                number = int(numberStr)
                if (number < 0):
                    print("Number inputted must be greated than or equal to 0")
                else:
                    return number
        except ValueError as e:
            print("Please input a non-negative integer")
        except Exception as e:
            print(f"An unexpected error has occured: {e}")

def inputBlock():
    weekdays = ["mon", "tue", "wed", "thu", "fri", "sat", "sun"]
    block = {}
    while True:
        day = input("Enter the day of the blocked period (in ddd) format or b if you would like to undo the previous slot: ").lower()
        if (day == 'b'):
            return day
        if (day not in weekdays):
            print("Invalid day")
        else:
            block["day"] = day
            break
    while True:
        try:
            startTimeStr = input("Enter the starting time of your blocked period (in HHMM format): ")
            if (len(startTimeStr) != 4):
                print("Invalid Time")
            else:
                startTime = int(startTimeStr)
                if (startTime < 0 or startTime > 2359):
                    print("Invalid Time")
                else:
                    if (startTime % 100 >= 60):
                        print("Invalid Time")
                    else:
                        block["startTime"] = startTime
                        break
        except ValueError as e:
            print("Invalid Time")
        except Exception as e:
            print(f"An unexpected error occured: {e}")
    while True:
        try:
            endTimeStr = input("Enter the ending time of your blocked period (in HHMM format): ")
            if (len(endTimeStr) != 4):
                print("Invalid Time")
            else:
                endTime = int(endTimeStr)
                if (endTime < startTime or endTime > 2359):
                    print("Invalid End Time")
                else:
                    if (endTime % 100 >= 60):
                        print("Invalid Time")
                    else:
                        block["endTime"] = endTime
                        return block
        except ValueError as e:
            print("Invalid Time")
        except Exception as e:
            print(f"An unexpected error occured: {e}")

def gettingBlockPeriod(count):
    blockPeriods = []
    def pushPeriods(c):
        if (c == count):
            return
        else:
            block = inputBlock()
            if (block != 'b'):
                blockPeriods.append(block)
                pushPeriods(c+1)
            else:
                if (len(blockPeriods) == 0):
                    pushPeriods(c)
                else:
                    blockPeriods.pop()
                    pushPeriods(c-1)
    pushPeriods(0)
    return blockPeriods

blockPeriodNumber = gettingNumberOfBlockPeriods()
blockPeriodList = gettingBlockPeriod(blockPeriodNumber)

def getMult():
    print("Right now, you'll be inputting multiple values on the scale of 1 to 10 (of course you can weight higher or even negative up to preference). " \
    "\n This is to evaluate your possible timetables to find your best one posible")
    multipliers = {}
    while True:
        try:
            freeDayMultStr = input("Please input how heavily you want to weigh Free Days: ")
            freeDayMult = float(freeDayMultStr)
            if (freeDayMult > 10 or freeDayMult < 0):
                print("Please enter a number from 0 - 10")
            else:
                multipliers["freeDayMult"] = freeDayMult
                break
        except ValueError as e:
            print("Please enter a number from 0 - 10")
    while True:
        try:
            multipliers["hours"] = []
            startHourStr = input("Please let us know when you would like your day to start in HHMM: ")
            if (len(startHourStr) != 4):
                print("Please enter a valid time!")
            else:
                startHour = int(startHourStr)
                if (startHour < 0 or startHour > 2359):
                    print("Please enter a valid time!")
                else:
                    startHour = math.floor(startHour/100) + (startHour%100)/60
                    multipliers["hours"].append(startHour)
                    break
        except ValueError as e:
            print("Please enter a valid time")
    while True:
        try:
            startTimeMultStr = input("Please input how heavily you want to weigh start time (later the better): ")
            startTimeMult = float(startTimeMultStr)
            if (startTimeMult > 10 or startTimeMult < 0):
                print("Please enter a number from 0 - 10")
            else:
                multipliers["startTimeMult"] = startTimeMult
                break
        except ValueError as e:
            print("Please enter a number from 0 - 10")
    while True:
        try:
            endHourStr = input("Please let us know when you would like your day to end in HHMM: ")
            if (len(endHourStr) != 4):
                print("Please enter a valid time!")
            else:
                endHour = int(endHourStr)
                if (endHour < startHour or endHour > 2359):
                    print("Please enter a valid end time!")
                else:
                    endHour = math.floor(endHour/100) + (endHour%100)/60
                    multipliers["hours"].append(endHour)
                    break
        except ValueError as e:
            print("Please enter a valid time")
    while True:
        try:
            endTimeMultStr = input("Please input how heavily you want to weigh end time (earlier the better): ")
            endTimeMult = float(endTimeMultStr)
            if (endTimeMult > 10 or endTimeMult < 0):
                print("Please enter a number from 0 - 10")
            else:
                multipliers["endTimeMult"] = endTimeMult
                break
        except ValueError as e:
            print("Please enter a number from 0 - 10")
    while True:
        try:
            multipliers["gapTimeMult"] = []
            optGapTimeStr = input("Please put the optimal gap time you wish to have per day, in hours: ")
            optimalGapTime = float(optGapTimeStr)
            if (optimalGapTime < 0 or optimalGapTime > 10):
                print("Please input a non-negative number smaller than 10")
            else:
                multipliers["gapTimeMult"].append(optimalGapTime)
                break
        except ValueError as e:
            print("Please input a non-negative number")
    while True:
        try:
            gapTimeMultStr = input("Please input how heavily you want to weigh gap time (closer to you optimal, the better): ")
            gapTimeMult = float(gapTimeMultStr)
            if (gapTimeMult > 10 or gapTimeMult < 0):
                print("Please enter a number fro 0 to 10")
            else:
                multipliers["gapTimeMult"].append(gapTimeMult)
                break
        except ValueError as e:
            print("Please enter a number from 0 - 10")
    while True:
        try:
            noOfTimetablesStr = input("How many Timetables would you like to print: ")
            if ('.' in noOfTimetablesStr):
                print("Please enter a positive integer")
            else:
                noOfTimetables = int(noOfTimetablesStr)
                multipliers["noOfTimetables"] = noOfTimetables
                break
        except ValueError as e:
            print("Please enter a positive integer")
    print("In this program, we also made a feature to take location into account.")
    while True:
        try:
            locationAccInputStr = input("If you would like to take location into account, enter 1. Otherwise, enter 0: ")
            locationInput = int(locationAccInputStr)
            if (locationInput != 1 and locationInput != 0):
                print("Please enter either 1 (for yes), or 0 (for no)")
            else:
                if (locationInput == 1):
                    locationMultStr = input("How much would you like to weight location on a scale from 0 to 10: ")
                    locationMult = float(locationMultStr)
                    if (locationMult < 0 or locationMult > 10):
                        print("Please enter a number from 0 - 10")
                else:
                    locationMult = 0
            multipliers["locationInputs"] = [locationInput, locationMult]
            break
        except ValueError as e:
            print("Please enter a number")
    return multipliers

multipliers = getMult()

for mod in modCodeList:
    particularModData = {}
    response = requests.get(generate_modAPIURL(mod))
    data = response.json()
    particularModData["moduleCode"] = data["moduleCode"]
    particularModData["MCs"] = int(data["moduleCredit"])
    semData = {}
    for semDict in data["semesterData"]:
        if (semDict["semester"] == semNo):
            semData = semDict
    particularModData["potentialLessons"] = {}
    intermediateGroupedLessons = {}
    days = ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"]
    for slot in semData["timetable"]:
        lessonType = slot["lessonType"]
        classNo = slot["classNo"]
        if lessonType not in intermediateGroupedLessons:
            intermediateGroupedLessons[lessonType] = {}
        if classNo not in intermediateGroupedLessons[lessonType]:
            intermediateGroupedLessons[lessonType][classNo] = []
        simplifiedTimeSlot = {"day":slot["day"], "startTime":int(slot["startTime"]), "endTime":int(slot["endTime"]), "location":slot["venue"]}
        intermediateGroupedLessons[lessonType][classNo].append(simplifiedTimeSlot)
    for lessonType in intermediateGroupedLessons.keys():
        listOfTime = []
        setListOfTime = {}
        dupeClasses = []
        for classNo in intermediateGroupedLessons[lessonType].keys():
            groupedTimeSlots = []
            for timeSlot in intermediateGroupedLessons[lessonType][classNo]:
                simplifiedFurther = (timeSlot["day"], timeSlot["startTime"], timeSlot["endTime"])
                groupedTimeSlots.append(simplifiedFurther)
            for i in range(len(groupedTimeSlots)):
                key = groupedTimeSlots[i]
                j = i - 1
                while( j >= 0 and (days.index(key[0]) < days.index(groupedTimeSlots[j][0]) or (key[0] == groupedTimeSlots[j][0] and key[1] < groupedTimeSlots[j][1]))):
                    groupedTimeSlots[j+1] = groupedTimeSlots[j]
                    j -= 1
                groupedTimeSlots[j+1] = key
            combinedString = ""
            for timeSlot in groupedTimeSlots:
                combinedString += f"{timeSlot[0]}{timeSlot[1]}{timeSlot[2]}"
            listOfTime.append(combinedString)
            setListOfTime = set(listOfTime)
            if (len(setListOfTime) != len(listOfTime)):
                listOfTime.pop()
                dupeClasses.append(classNo)
        for dupeClass in dupeClasses:
            if (intermediateGroupedLessons[lessonType][dupeClass][0]["location"] != "E-Learn_C"):
                del intermediateGroupedLessons[lessonType][dupeClass]
    particularModData["potentialLessons"] = intermediateGroupedLessons
    modData.append(particularModData)
baseDir = os.path.dirname(os.path.abspath(__file__))
exePath = os.path.join(baseDir, "LogicOfTimetabling.exe")
moduleJsonPath = os.path.join(baseDir, "ModuleData.json")
blockPeriodPath = os.path.join(baseDir, "BlockPeriodFromUser.json")
multipliersPath = os.path.join(baseDir, "Multipliers.json")

with open(moduleJsonPath, "w") as f:
    json.dump(modData, f, indent=4)
with open(blockPeriodPath, "w") as f:
    json.dump(blockPeriodList, f, indent=4)
with open(multipliersPath, "w") as f:
    json.dump(multipliers, f, indent=4)

try:
    result = subprocess.run([exePath], capture_output=True, text=True, check=True)
    print("C++ program Output: ")
    print(result.stdout)
except subprocess.CalledProcessError as e:
    print(f"Error running C++ program: {e}")