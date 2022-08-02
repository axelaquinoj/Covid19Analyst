#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <chrono>
#include "vector"
#include "Covid.h"
using namespace std::chrono;
using namespace std;

//Function Prototypes --------------------
void mergeSort(vector<Covid>& list, string& selection, int left, int right);
void merge(vector<Covid>& list, string& selection, int left, int middle, int right);
void quickSort(vector<Covid>& list, int low, int high, string selection);
int partition(vector<Covid>& list, int low, int high, string selection);
void analyzeCountries(vector<string> &countriesToAnalyze, map<string, int> availableCountries, int numCountry, string selection);
void readFile(ifstream &file, vector<Covid> &data1Unsorted, vector<Covid> &data2Unsorted, map <string,int> &availableCountries);
void printData(vector<Covid>& dataToSort, int numDataPoints, string selectionS);
void timeFrame (bool &validStart, bool &validEnd, int &timeFrameIndexStart, int &timeFrameIndexEnd, vector<Covid> &data1Unsorted);

int main() {
    vector<Covid> data1Unsorted;
    vector<Covid> data2Unsorted;
    map<string, int> availableCountries;
    ifstream  file;

    //Reads in the file and stores data into vectors for it to be sorted later.
    readFile(file, data1Unsorted, data2Unsorted, availableCountries);

    //User interface
    cout << "Welcome to the Covid-19 Analyst!" << endl;

    // MAIN PROGRAM BEGINS --------------------
    bool quit = false;
    while(!quit){
        cout << "\n-------Available Countries------- " << endl;
        string s;
        for (auto &it :availableCountries){
            cout << it.first << ", ";
            s += it.first;
            if (s.length() > 35){
                cout << endl;
                s = "";
            }
        }
        cout << "\n" << endl;
        cout << "Total number of countries available: " <<  availableCountries.size()<<endl;
        string selection;
        int numCountry;
        bool again = true;

        //Select Countries
        while(again){
            cout << "\nHow many countries would you like to analyze?" << endl;
            string numCountryS;
            cin >> numCountryS;

            try{
                numCountry = stoi(numCountryS);
                if(numCountry < 1 || numCountry > availableCountries.size())
                    throw(out_of_range("blah"));
                again = false;
            }
            catch(invalid_argument){
                cout << "ERROR: NOT A NUMBER\nPLEASE ENTER INTEGER VALUES ONLY" << endl;
            }
            catch(out_of_range){
                cout << "ERROR: INVALID NUMBER\nPLEASE ENTER VALUES BETWEEN 1 AND " << availableCountries.size() << endl;
            }
        }

        //Variables for below functions
        vector<string> countriesToAnalyze;
        bool validStart = false;
        bool validEnd = false;
        int timeFrameIndexStart;
        int timeFrameIndexEnd;

        // Handles input and storing of the countries.
        analyzeCountries(countriesToAnalyze, availableCountries, numCountry,selection);

        //Select time frame
        timeFrame(validStart, validEnd, timeFrameIndexStart, timeFrameIndexEnd, data1Unsorted);

        //Holds/pushes in all data within the selected countries time frame
        vector<Covid> dataToSort;
        for (int i = 0; i < countriesToAnalyze.size(); i++){
            string country = countriesToAnalyze[i];
            for (int j = timeFrameIndexStart; j <= timeFrameIndexEnd; j++){
                if (country == data1Unsorted[j].country){
                    dataToSort.push_back(data1Unsorted[j]);
                }
            }
        }

        //Select how to sort the data,
        //either by cases, deaths, or rate
        string selectionS;
        again = true;
        while(again){
            int selection;
            cout << "\nHow would you like to sort the Covid-19 data?" << endl;
            cout << " 1) Cases\n 2) Deaths\n 3) Infection Rate" <<endl;

            cin >> selectionS;
            selection = stoi(selectionS);
            if(selection == 1){
                selectionS = "cases";
                again = false;
            }
            else if(selection == 2){
                selectionS = "deaths";
                again = false;
            }
            else if(selection == 3){
                selectionS = "rate";
                again = false;
            }
            else{
                cout << "ERROR: INVALID SELECTION. PLEASE ENTER 1, 2, OR 3.";
            }
        }

        //Prompt number of data points user wants to see --------------------
        int numDataPoints;
        string numDataPointsS;
        again = true;
        cout<< "\nHow many data points would you like to see?" <<endl;
        while(again){
            cin >> numDataPointsS;
            try{
                numDataPoints = stoi(numDataPointsS);
            }
            catch(invalid_argument){
                cout<<"ERROR: PLEASE ENTER A VALID NUMBER" << endl;
                continue;
            }
            again = false;
        }

        vector<Covid> dataToSortCopy;
        for (int i = 0; i < dataToSort.size(); i++){
            dataToSortCopy.push_back(dataToSort[i]);
        }

        //Grabs the time of the sort functions --------------------
        auto start = high_resolution_clock::now();
        mergeSort(dataToSort,selectionS, 0, dataToSort.size()-1);
        auto stop = high_resolution_clock::now();
        auto durationMerge = duration_cast<microseconds>(stop - start);

        start = high_resolution_clock::now();
        quickSort(dataToSortCopy, 0, dataToSortCopy.size() - 1, selectionS);
        stop = high_resolution_clock::now();
        auto durationQuick = duration_cast<microseconds>(stop - start);



        //Prints the data of whatever user selects --------------------
        printData(dataToSort, numDataPoints, selectionS);
        cout<< "\nExecution time for merge sort: " << durationMerge.count() << " microseconds" << endl;
        cout<< "Execution time for quick sort: " << durationQuick.count() << " microseconds\n\n" << endl<<endl;
        // Reset structures --------------------
        dataToSort.clear();
        countriesToAnalyze.clear();
        dataToSortCopy.clear();

        for(auto iter = availableCountries.begin(); iter != availableCountries.end(); iter++){
            iter->second = 1;
        }

        //Ask user whether they would like to run another anaysis.
        cout << "\n\nWould you like to run another analysis? Enter Y for yes and N for no" << endl;
        again = true;
        while(again){

            cin >> s;
            if(s == "n" || s == "N"){
                quit = true;
                again = false;
            }
            else if(s=="y" || s=="Y"){
                again = false;
            }
            else{
                cout<<"ERROR: PLEASE ENTER Y OR N\n"<< endl;
            }
        }
    }
    return 0;
}
//End of main

// ------------------------Sort Functions Below ------------------------------

void mergeSort(vector<Covid>& list, string& selection, int left, int right){
    if (left < right){
        int middle = left + (right-left) /2;
        mergeSort(list,selection,left,middle);
        mergeSort(list,selection, middle+1,right);
        merge(list,selection,left,middle,right);
    }
}
void merge(vector<Covid>& list, string& selection, int left, int middle, int right){
    if (selection == "cases") {
        int n1 = middle - left+1;
        int n2 = right - middle;
        vector<Covid> first, second;

        for (int i = 0; i < n1; ++i) {
            first.push_back(list[left+i]);
        }
        for (int i = 0; i < n2; ++i) {
            second.push_back(list[middle+1+i]);
        }
        int i =0 , j=0 ,k = left;
        while (i < n1 && j < n2){
            if (first[i].cases <= second[j].cases){
                list[k] = first[i];
                ++i;
            }
            else{
                list[k] = second[j];
                ++j;
            }
            ++k;
        }
        while (i < n1){
            list[k] = first[i];
            ++i;
            ++k;
        }
        while (j < n2){
            list[k] = second[j];
            ++j;
            ++k;
        }
    }
    else if (selection == "deaths"){
        int n1 = middle - left+1;
        int n2 = right - middle;
        vector<Covid> first, second;

        for (int i = 0; i < n1; ++i) {
            first.push_back(list[left+i]);
        }
        for (int i = 0; i < n2; ++i) {
            second.push_back(list[middle+1+i]);
        }
        int i =0 , j=0 ,k = left;
        while (i < n1 && j < n2){
            if (first[i].deaths <= second[j].deaths){
                list[k] = first[i];
                ++i;
            }
            else{
                list[k] = second[j];
                ++j;
            }
            ++k;
        }
        while (i < n1){
            list[k] = first[i];
            ++i;
            ++k;
        }
        while (j < n2){
            list[k] = second[j];
            ++j;
            ++k;
        }
    }
    else if (selection == "rate"){
        int n1 = middle - left+1;
        int n2 = right - middle;
        vector<Covid> first, second;

        for (int i = 0; i < n1; ++i) {
            first.push_back(list[left+i]);
        }
        for (int i = 0; i < n2; ++i) {
            second.push_back(list[middle+1+i]);
        }
        int i =0 , j=0 ,k = left;
        while (i < n1 && j < n2){
            if (first[i].rate <= second[j].rate){
                list[k] = first[i];
                ++i;
            }
            else{
                list[k] = second[j];
                ++j;
            }
            ++k;
        }
        while (i < n1){
            list[k] = first[i];
            ++i;
            ++k;
        }
        while (j < n2){
            list[k] = second[j];
            ++j;
            ++k;
        }
    }
}
int partition(vector<Covid>& list, int low, int high, string selection) {
    int pivot, index;
    index = low;  // index is equal to the start
    pivot = high; // pivot is the very last element in the vector.

    if ( selection == "cases") {
        for (int i = low; i < high; i++) {
            if (list[i].cases < list[pivot].cases) {
                swap(list[i], list[index]);
                index++;
            }
        }
        swap(list[pivot], list[index]);
        return index;
    }
    else if ( selection == "deaths") {
        for (int i = low; i < high; i++) {
            if (list[i].deaths < list[pivot].deaths) {
                swap(list[i], list[index]);
                index++;
            }
        }
        swap(list[pivot], list[index]);
        return index;
    }
    else if ( selection == "rate") {
        for (int i = low; i < high; i++) {
            if (list[i].rate < list[pivot].rate ) {
                swap(list[i], list[index]);
                index++;
            }
        }
        swap(list[pivot], list[index]);
        return index;
    }
}
void quickSort(vector<Covid>& list, int low, int high, string selection) {
    int pivot;

    if(low < high) {
        pivot = partition(list,low,high,selection) ;
        quickSort(list, low, pivot-1,selection);
        quickSort(list, pivot+1, high,selection);
    }
}
void analyzeCountries(vector<string> &countriesToAnalyze, map<string, int> availableCountries, int numCountry, string selection){
    if(numCountry != availableCountries.size()){
        //Makes sure the input is valid and stores into vectors
        cout << "\nWhich country(ies) would you like to analyze?" << endl;

        for(int i = 0; i < numCountry; i++){
            cout << "Country " << i+1 << ": ";
            cin >> selection;
            if (availableCountries.count(selection) == 1){
                if (availableCountries[selection] == 1){
                    countriesToAnalyze.push_back(selection);
                    availableCountries[selection] ++;
                }
                else{
                    cout << "COUNTRY ALREADY ADDED" << endl;
                    cout << "RE-ENTER INPUT" << endl;
                    --i;
                    continue;
                }
            }
            else{
                cout << "ENTER A VALID COUNTRY" <<endl;
                cout << "RE-ENTER INPUT" <<endl;
                --i;
                continue;
            }
        }
    }
    else{
        cout << "\nAnalyzing all countries" << endl;
        for (auto iter = availableCountries.begin(); iter != availableCountries.end(); iter++){
            countriesToAnalyze.push_back(iter->first);
        }
    }
}
void readFile(ifstream &file, vector<Covid> &data1Unsorted, vector<Covid> &data2Unsorted, map <string,int> &availableCountries){
    file.open("covid.csv");

    if (file.is_open()) {
        string line;
        //Throw away line, useless info. First line of excel.
        getline(file, line);

        while(getline(file,line)){
            //Read file
            istringstream stream(line);
            string day, month, year, cases, death,country, code, population, continent, rate;

            getline(stream,day, ',');
            getline(stream,month, ',');
            getline(stream,year, ',');
            getline(stream,cases, ',');
            getline(stream,death, ',');
            getline(stream,country, ',');
            getline(stream,code,',');
            getline(stream,population,',');
            getline(stream,continent,',');
            getline(stream,rate,',');

            int dayI, monthI, yearI, caseI, deathI, populationI;
            float rateF;

            dayI = stol(day);
            monthI = stoi(month);
            yearI = stoi(year);
            caseI = stoi(cases);
            deathI = stoi(death);
            populationI = stoi(population);
            rateF = stof(rate);

            //Create Covid object and stores data into vectors.
            Covid   CovidObject(dayI,monthI,yearI,caseI,deathI,country,code,populationI,continent,rateF);

            data1Unsorted.push_back(CovidObject);
            data2Unsorted.push_back(CovidObject);
            availableCountries[country] = 1;
        }
    }
}
void timeFrame (bool &validStart, bool &validEnd, int &timeFrameIndexStart, int &timeFrameIndexEnd, vector<Covid> &data1Unsorted){

    cout << "\nAvailable Time Frame: 12/31/2019 --- 12/14/2020" << endl;
    cout << "\nSelect a time frame in which you would like to analyze the countries at" << endl;

    while (!validStart || !validEnd) {
        string monthS, dayS, yearS, month2S, day2S, year2S;
        int month, day, year, month2, day2, year2;


        validStart = false;
        validEnd = false;

        cout << "\nStart Date: " << endl;

        cout << "Month:" << endl;
        cin >> monthS;
        month = stoi(monthS);
        cout << "\nDay:" << endl;
        cin >> dayS;
        day = stoi(dayS);
        cout << "\nYear:" << endl;
        cin >> yearS;
        year = stoi(yearS);


        cout << "\nEnd Date: " << endl;

        cout << "Month:" << endl;
        cin >> month2S;
        month2 = stoi(month2S);
        cout << "\nDay:" << endl;
        cin >> day2S;
        day2 = stoi(day2S);
        cout << "\nYear:" << endl;
        cin >> year2S;
        year2 = stoi(year2S);

        bool foundStart = false;
        bool foundEnd = false;


        //Searches through the  unsorted vector to see if the data is available.
        for (int i = 0; i < data1Unsorted.size(); i++){
            //If the day,year,month is equal to a objects day,year, month then the date is valid.
            if (!foundStart){
                if (data1Unsorted[i].day == day && data1Unsorted[i].month == month && data1Unsorted[i].year == year){
                    foundStart = true;
                    validStart = true;
                    timeFrameIndexStart = i;
                }
            }

            if (data1Unsorted[i].day == day2 && data1Unsorted[i].month == month2 && data1Unsorted[i].year == year2){
                validEnd = true;
                timeFrameIndexEnd = i;
            }
        }

        if (validStart && validEnd){
            if (timeFrameIndexStart > timeFrameIndexEnd){
                validStart = false;
                validEnd = false;
                cout << "End date cannot be before start date." << endl;
            }
            continue;
        }
        if (!validStart || !validEnd){
            cout << "Invalid time frame. Re-enter." <<endl;
        }
        else if (validStart && validEnd){
            cout << "Time frame successfully entered." <<endl;
        }

    }
}


void printData(vector<Covid> &dataToSort, int numDataPoints, string selectionS){

    bool nDPGreater = false;
    if(numDataPoints > dataToSort.size()){
        cout << "There is insufficient data for the selections you have made\n" <<endl;
        nDPGreater = true;
    }


    if(!nDPGreater){
        if (selectionS == "rate"){
            cout << "Top " << numDataPoints << " dates with highest infection " << selectionS << "\nwithin the chosen time frame: \n" << endl;
        }
        else{
            cout << "Top " << numDataPoints << " dates with highest number of " << selectionS << "\nwithin the chosen time frame: \n" << endl;
        }
    }

    else{
        if (selectionS == "rate"){
            cout << "Top " << dataToSort.size() << " dates with highest infection " << selectionS << "\nwithin the chosen time frame: \n" << endl;
        }
        else{
            cout << "Top " << dataToSort.size() << " dates with highest number of " << selectionS << "\nwithin the chosen time frame: \n" << endl;
        }
    }

    int count = 1;
    int deathSum = 0;
    int caseSum = 0;
    int min = dataToSort.size() - 1 - numDataPoints;
    if(selectionS == "cases"){

        for(int i = dataToSort.size()-1; i > min && i >= 0; i--){

            cout << count << ")  Cases: " << dataToSort[i].cases << "  Country: " << dataToSort[i].country << "  Date: " << dataToSort[i].month << "/" << dataToSort[i].day << "/" << dataToSort[i].year << endl;
            deathSum += dataToSort[i].deaths;
            caseSum += dataToSort[i].cases;
            ++count;
        }
    }
    else if(selectionS == "rate"){
        for(int i = dataToSort.size() - 1; i > min && i >= 0; i--){
            cout << count << ")  Rate: " << dataToSort[i].rate << "  Country: " << dataToSort[i].country << "  Date: " << dataToSort[i].month << "/" << dataToSort[i].day << "/" << dataToSort[i].year << endl;
            deathSum += dataToSort[i].deaths;
            caseSum += dataToSort[i].cases;
            ++count;
        }
    }
    else if(selectionS == "deaths"){
        for(int i = dataToSort.size() - 1; i > min && i >= 0; i--){
            cout << count << ")  Deaths: " << dataToSort[i].deaths << "  Country: " << dataToSort[i].country << "  Date: " << dataToSort[i].month << "/" << dataToSort[i].day << "/" << dataToSort[i].year << endl;
            caseSum += dataToSort[i].cases;
            deathSum += dataToSort[i].deaths;
            ++count;
        }

    }

    //Print a total counter for all cases and deaths within the time frame
    cout << "\nTotal number of deaths: " << deathSum <<endl;
    cout << "Total number of cases: " << caseSum <<endl;
}