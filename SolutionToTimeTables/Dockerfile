FROM python:3.13.5-slim
RUN apt-get update
RUN apt-get install -y g++
WORKDIR /SolutionToTimeTables
COPY . .
RUN pip3 install requests
RUN g++ LogicOfTimetabling.cpp -o LogicOfTimetabling.exe
CMD ["python3", "APILogic.py"]