//
// Created by Axel Aquino on 8/1/22.
//

#ifndef COVID19ANALYST_COVID_H
#define COVID19ANALYST_COVID_H

#include "string"
#include "vector"
using namespace std;

struct Covid{
    int day, month, year, cases, deaths, population;
    string country, code, continent;
    float rate;

    Covid (int day , int month, int year, int cases, int deaths, string country, string code, int population, string continent,float rate);

};

#endif //COVID19ANALYST_COVID_H
