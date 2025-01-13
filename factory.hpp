#pragma once
#include "tfunction.hpp"


class Factory {
public:
    Factory() {}

    TFunctionPtr Create(const string& type, double val = 1) {
        if (type == "ident")
            return std::make_shared<Ident>();
        if (type == "const")
            return std::make_shared<Const>(val);
        if (type == "power")
            return std::make_shared<Power>(val);
        if (type == "exp")
            return std::make_shared<Exp>();

        throw std::logic_error("Incorrect type of class in factory: " + type);
    }

    TFunctionPtr Create(const string& type, std::initializer_list<double> values) {
        if (type == "polynomial")
            return std::make_shared<Polynomial>(values);

        throw std::logic_error("Incorrect type of class in factory: " + type);
    }
};


TFunction operator+(const string& s, const TFunction& other) {
    return other.operator+(s);
}

TFunction operator*(const string& s, const TFunction& other) {
    return other.operator*(s);
}