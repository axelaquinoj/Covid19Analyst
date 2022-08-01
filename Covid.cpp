//
// Created by Axel Aquino on 8/1/22.
//

#include "Covid.h"

Covid::Covid (int day, int month, int year, int cases, int deaths, string country, string code, int population, string continent, float rate){
    this->day = day;
    this->month = month;
    this->year = year;
    this-> cases = cases;
    this-> deaths = deaths;
    this-> country = country;
    this-> code = code;
    this->population = population;
    this-> continent = continent;
    this->rate = rate;
}