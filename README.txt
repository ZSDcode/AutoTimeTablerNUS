Do you hate planning your own timetable every semester?
So do I!
Welcome to AutoTimeTabler

Just a small project about planning a University Timetable using code
1. Error Handling of inputs using Python
- Reasoning: Going to fetch API data using inputs from Python, so might as well include error-handling there
2. Fetching API data and parsing from api.nusmods.com using Python
- Reasoning: Easy syntax to learn, get data fast
3. Pipelining data to local JSON file for C++ code to read from
- Reasoning: C++ backend is much more efficient than Python, especially for large datasets
4. Output best timetable using scoring system
- Reasoning: Easy to tune according to preferences and efficient in checking and sorting timetables from worst to best
5. Allow user to determine the number of timetables he needs based on sorted score
- Reasoning: Sometimes, we need backups

Current Progress:
Coded the backend logic for C++ to iterate through timetables
Error handling of inputs
Parsing Data Settled
C++ to read json file and add into structs
Score function implemented
Location Based Feature implemented!
Touched up on the inefficiencies, shorting time from 400 seconds to 60 seconds execution time!

Next Step:
Removing redundant Timetables
Writing up web server and frontend to host this for better UI