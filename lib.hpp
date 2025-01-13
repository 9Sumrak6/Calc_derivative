#pragma once

#include <iostream>

#include <vector>
#include <string>
#include <set>
#include <initializer_list>

#include <functional>
#include <memory>
#include <exception>
#include <cmath>
#include <iomanip>


using std::cin, std::cout, std::endl;
using std::vector, std::string;
using std::shared_ptr;


class TFunction;
using TFunctionPtr = std::shared_ptr<TFunction>;


const double EPS = 0.00001;
const int ITERATIONS = 100;


std::string doubleToString(double value, int precision) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}


double roundToThreeDecimalPlaces(double value) {
    return std::round(value * 1000.0) / 1000.0;
}