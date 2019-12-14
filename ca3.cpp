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

using namespace std;

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

        double passPath(int p){
            sem_wait(&isEmpty);
            double pollution = 0;
            for( long int i = 0;i < 10000000; i++ ){
                pollution += i / (1000000 * p * h);
            }
            sem_post(&isEmpty);
            return pollution;
        }

};


class PollutionMonitor{
    private:
        double totalPollution;
        sem_t available;
    public:
        PollutionMonitor(){
            totalPollution = 0;
            sem_init(&available, 0, 1);
        }
        void calTotalPollution(double newPollution){
            sem_wait(&available);
            totalPollution += newPollution;
            sem_post(&available);
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

int findPath( vector<vector<string>  > input, char* start, char* end){
    for(int i = 0; i< input.size(); i++){
        if(input[i][0] == start && input[i][1] == end )
            return atoi(input[i][2].c_str());
    }
    return -1;
}

void driveCar( int carNumber, int &p, vector<string> &carPath, vector<PathMonitor> &pathMonitors, PollutionMonitor &pollutionMonitor){
    cout << "here   " << carNumber << endl;
    this_thread::sleep_for(std::chrono::milliseconds(200));
    time_t my_time = time(NULL);
    printf("%s", ctime(&my_time));

    // for();

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

    int carNumber = 0;
    vector< PathMonitor> pathMonitors;
    PollutionMonitor pollutionMonitor;
    vector< thread> carThreads;


    for(int i = 0; i< path.size();i++){
        PathMonitor pm(path[i][0], path[i][1], path[i][2]);
        pathMonitors.push_back(pm);
    }


    for(int i = 0;i< cars.size();i++){
        for( int j =0; j < numOfCars[i]; j++){
            int p = rand() % 10;
            cout << "p:  " <<p << endl;
            thread carThread(driveCar, ref(carNumber), ref(p), ref(cars[i]), ref(pathMonitors), ref(pollutionMonitor));
            carThreads.push_back(move(carThread));
            carNumber++;
        }
    }
    for( int i = 0; i < carThreads.size(); i++)
        carThreads[i].join();
}