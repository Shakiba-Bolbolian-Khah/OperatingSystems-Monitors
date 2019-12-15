#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <bits/stdc++.h>
#include <thread> 
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <math.h>
#include <chrono>
#include <ctime>

using namespace std;


class PollutionMonitor{
    private:
        long int totalPollution;
        sem_t available;
    public:
        PollutionMonitor(){
            totalPollution = 0;
            sem_init(&available, 0, 1);
        }
        long int getTotalPollution(){ return totalPollution;}
        long int calTotalPollution(long int newPollution){
            sem_wait(&available);
            totalPollution += newPollution;
            sem_post(&available);
            return totalPollution;
        }
};

class PathMonitor{
    private:
        string start;
        string end;
        int h;
        sem_t isEmpty;

    public:
        PathMonitor(string _start, string _end, string _h){
            start = _start;
            end = _end;
            h = atoi( _h.c_str());
            sem_init(&isEmpty, 0, 1);
        }
        string getStart(){ return start;}
        string getEnd(){ return end;}

        vector <string> passPath(int p, PollutionMonitor &pollutionMonitor){
            sem_wait(&isEmpty);
            chrono::milliseconds entranceTime = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
            vector <string> pollutions;
            long int pollution = 0;
            for( long int i = 0;i < 10000000; i++ ){
                pollution += i / (1000000 * p * h);
            }
            pollutions.push_back(to_string(entranceTime.count()));
            pollutions.push_back(to_string(pollution));
            pollutions.push_back(to_string(pollutionMonitor.calTotalPollution(pollution)));
            chrono::milliseconds exitTime = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
            pollutions.push_back(to_string(exitTime.count()));
            sem_post(&isEmpty);
            return pollutions;
        }

};




vector <char*> split(string str,const char* delimeter){
    vector<char*> strToks;
    char *cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
    char *token = strtok(cstr, delimeter); 
    strToks.push_back(token); 
    while (token != NULL) 
    { 
        token = strtok(NULL, delimeter); 
        strToks.push_back(token);
    } 
    return strToks;
}

int findPathMonitor(vector<PathMonitor> &pathMonitors, string start, string end){
    for( int i = 0; i < pathMonitors.size(); i++){
        if( (pathMonitors[i].getEnd() == end) && (pathMonitors[i].getStart() == start))
            return i;
    }
    cout << "No path with start: " << start << " and end: " << end << " had been found!" << endl;
    exit(0);
}

void driveCar( int pathNum, int carNumber, int p, vector<string> carPath, vector<PathMonitor> &pathMonitors, PollutionMonitor &pollutionMonitor){
    std::ofstream outfile (to_string(pathNum)+"-"+ to_string(carNumber)+".txt");
    for( int i = 0; i< carPath.size()-1; i++){
        int pmNum = findPathMonitor( pathMonitors, carPath[i], carPath[i+1]);
        vector<string> pollutions = pathMonitors[pmNum].passPath(p, pollutionMonitor);
        outfile << carPath[i] <<", " << pollutions[0] << ", " << carPath[i+1] << ", " << pollutions[3] << ", " << pollutions[1] << ", " << pollutions[2] << endl;
    }
    outfile.close();

}


int main(int argc, char* argv[]){

    if(argc != 2){
        cout << "Error in num of arguments!" << endl;
        exit(0);
    }
    std::ifstream file(argv[1]);
    vector <vector<string> > path, cars;
    vector <int> numOfCars;

    int index;
    string str;
    getline(file, str);
    while ( str != "#") {
        vector<char*> toks = split(str, " -");
        vector <string> newpath;
        newpath.push_back(toks[0]);
        newpath.push_back(toks[1]);
        newpath.push_back(toks[2]);
        path.push_back(newpath);
        getline(file, str);
    }
    while (getline(file,str))
    {
        vector<char*> toks = split(str, " -");
        vector <string> newcar;
        for( int i = 0; i < toks.size()-1; i++)
            newcar.push_back(toks[i]);
        getline(file, str);
        cars.push_back(newcar);
        numOfCars.push_back(atoi(str.c_str()));
    }


    vector< PathMonitor> pathMonitors;
    PollutionMonitor pollutionMonitor;
    vector< thread> carThreads;


    for(int i = 0; i< path.size();i++){
        PathMonitor pm(path[i][0], path[i][1], path[i][2]);
        pathMonitors.push_back(pm);
    }


    for(int i = 0;i< cars.size();i++){

        for( int j =0; j < numOfCars[i]; j++){
            
            int p = rand() % 10 + 1;
            thread carThread(driveCar,i+1, j+1, p, cars[i], ref(pathMonitors), ref(pollutionMonitor));
            carThreads.push_back(move(carThread));
        }
    }
    for( int i = 0; i < carThreads.size(); i++)
        carThreads[i].join();
}