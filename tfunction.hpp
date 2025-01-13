#pragma once
#include "lib.hpp"


class TFunction {
private:
    std::function<double(double)> deriv;
    std::function<double(double)> f;

    template <typename T>
    bool check(const T& other, string op) const;
    TFunctionPtr makePtrByStr(const string& s) const;
protected:
    string repr;
    string name;

    virtual double getDiff(double x) const { throw std::logic_error("Mistake with calling 'GetDeriv' in TFunction."); }
public:
    TFunction() {
        deriv = [this](double x) { return getDiff(x); };
        f = [this](double x) { return (*this)(x); };
    }

    TFunction(const TFunction& other) = default;

    TFunction(TFunction&& other) = default;

    virtual ~TFunction() {}

    double GradientDescent(double start) const {
        double x = start;

        for (int i = 0; i < ITERATIONS; ++i) {
            if (fabs(f(x)) < EPS)
                break;

            // double h = 1e-5;
            double grad, f_v;

            try{
                grad = deriv(x);
                f_v = f(x);
            } catch(...) {
                cout << "Division by zero in GradientDescent with x = " << x << ". Returned the input value: " << start << endl;
                return start;
            }

            if (grad < EPS) {
                cout << "Division by zero in GradientDescent with x = " << x << ". Returned the input value: " << start << endl;
                return start;
            }

            x -= f_v / grad;
        }

        return x;
    }

    double GetDeriv(double x) const {
        return deriv(x);
    }

    virtual string ToString() const {
        return repr;
    }

    virtual double operator()(double x) const {
        return f(x);
    }

    template <typename T>
    TFunction operator+(const T& tOther) const;

    TFunction operator+(const char * s) const {
        return this->operator+(string(s));
    }

    template <typename T>
    TFunction operator-(const T& tOther) const;

    TFunction operator-(const char * s) const {
        return this->operator-(string(s));
    }

    template <typename T>
    TFunction operator*(const T& tOther) const;

    TFunction operator*(const char * s) const {
        return this->operator*(string(s));
    }

    template <typename T>
    TFunction operator/(const T& tOther) const;

    TFunction operator/(const char * s) const {
        return this->operator/(string(s));
    }
};


class Ident : public TFunction {
public:
    Ident() {
        repr = "x";
        name = "ident";
    }

    double getDiff(double x) const { return 1; }
    double operator()(double x) const { return x; }
};


class Const : public TFunction {
    double value;
public:
    Const(double value = 1) : value(value) {
        repr = doubleToString(value, 3);
        name = "const";
    }

    double getDiff(double x) const { return 0; }
    double operator()(double x) const { return value; }
};


class Power : public TFunction {
    double p;
public:
    Power(double p = 1) : p(p) {
        repr = "x^" + doubleToString(p, 3);
        name = "power";
    }

    double getDiff(double x) const {
        if (p == 0) return 0.;
        if (p == 1) return 1.;

        if (fabs(x) < EPS && p < 0)
            throw std::logic_error("Division by zero in 'Power'");

        return p * pow(x, p - 1);
    }

    double operator()(double x) const {
        if (p == 0) return 1.;

        return pow(x, p);
    }
};


class Exp : public TFunction {
public:
    Exp() {
        repr = "e^x";
        name = "exp";
    }

    double getDiff(double x) const {
        return exp(x);
    }

    double operator()(double x) const {
        return exp(x);
    }
};


class Polynomial : public TFunction {
    vector<double> values;
    vector<double> diff_values;
public:
    Polynomial(std::initializer_list<double> vals = {1}) {
        name = "polynomial";

        values = vector<double>(vals);
        size_t size = values.size();

        if (size > 0 && values[0] != 0)
            repr = doubleToString(values[0], 3);

        for (size_t i = 1; i < size; i++) {
            if (values[i] != 0) {
                if (repr != "")
                    repr += " + ";

                repr += doubleToString(values[i], 3) + "*x^" + std::to_string(i);
            }

            diff_values.push_back(values[i] * i);
        }
    }

    double getDiff(double x) const {
        double ans = 0;
        double t = 1;

        for (const auto& i : diff_values) {
            ans += t * i;
            t *= x;
        }

        return ans;
    }

    double operator()(double x) const {
        double ans = 0;
        double t = 1;

        for (const auto& i : values) {
            ans += t * i;
            t *= x;
        }

        return ans;
    }
};


TFunctionPtr TFunction::makePtrByStr(const string& s) const {
    if (s == "ident")
        return std::make_shared<Ident>();
    else if (s == "const")
        return std::make_shared<Const>();
    else if (s == "power")
        return std::make_shared<Power>();
    else if (s == "exp")
        return std::make_shared<Exp>();
    else if (s == "polynomial")
        return std::make_shared<Polynomial>();
    else
        throw std::logic_error("Incorrect argument in '+'!");
}

template <typename T>
TFunction TFunction::operator+(const T& tOther) const {
    TFunction ret;
    TFunctionPtr new_o = nullptr;

    if constexpr (std::is_same_v<T, std::string>)
        new_o = makePtrByStr(tOther);
    else {
        check(tOther, "+");
        new_o = std::make_shared<TFunction>(tOther);
    }

    ret.deriv = [this, new_o](double x) { return this->deriv(x) + new_o->deriv(x); };
    ret.f = [this, new_o](double x) { return this->f(x) + new_o->f(x); };
    ret.repr = "(" + this->ToString() + ")" + " + " + "(" + new_o->ToString() + ")";

    return ret;
}

template <typename T>
TFunction TFunction::operator-(const T& tOther) const {
    TFunction ret;
    TFunctionPtr new_o = nullptr;

    if constexpr (std::is_same_v<T, std::string>)
        new_o = makePtrByStr(tOther);
    else {
        check(tOther, "+");
        new_o = std::make_shared<TFunction>(tOther);
    }

    ret.deriv = [this, new_o](double x) { return this->deriv(x) - new_o->deriv(x); };
    ret.f = [this, new_o](double x) { return this->f(x) - new_o->f(x); };
    ret.repr = "(" + this->ToString() + ")" + " - " + "(" + new_o->ToString() + ")"; 

    return ret;
}

template <typename T>
TFunction TFunction::operator*(const T& tOther) const {
    TFunction ret;
    TFunctionPtr new_o = nullptr;

    if constexpr (std::is_same_v<T, std::string>)
        new_o = makePtrByStr(tOther);
    else {
        check(tOther, "+");
        new_o = std::make_shared<TFunction>(tOther);
    }

    ret.deriv = [this, new_o](double x) { return this->deriv(x) * new_o->f(x) + new_o->deriv(x) * this->f(x); };
    ret.f = [this, new_o](double x) { return this->f(x) * new_o->f(x); };
    ret.repr = "(" + this->ToString() + ")" + " * " + "(" + new_o->ToString() + ")";

    return ret;
}

template <typename T>
TFunction TFunction::operator/(const T& tOther) const {
    TFunction ret;
    TFunctionPtr new_o = nullptr;

    if constexpr (std::is_same_v<T, std::string>)
        new_o = makePtrByStr(tOther);
    else {
        check(tOther, "+");
        new_o = std::make_shared<TFunction>(tOther);
    }

    if (new_o->name == "const" && fabs((*new_o)(1)) < EPS)
        throw std::logic_error("Division by zero in operator '/'");

    ret.deriv = [this, new_o](double x) {
        double v = new_o->f(x);
        if (fabs(v) < EPS)
            throw std::logic_error("Division by zero in operator '/'");

        double v_d = new_o->deriv(x);
        double u = this->f(x), u_d = this->deriv(x);

        return (u_d * v - v_d * u) / (v * v);
    };
    ret.f = [this, new_o](double x) {
        if (fabs(new_o->f(x)) < EPS)
            throw std::logic_error("Division by zero in operator '/'");

        return this->f(x) / new_o->f(x);
    };
    ret.repr = "(" + this->ToString() + ")" + " / " + "(" + new_o->ToString() + ")";

    return ret;
}

template <typename T>
bool TFunction::check(const T& other, string op) const {
    if (std::is_same<T, TFunction>::value) return true;
    if (std::is_same<T, Ident>::value) return true;
    if (std::is_same<T, Const>::value) return true;
    if (std::is_same<T, Power>::value) return true;
    if (std::is_same<T, Exp>::value) return true;
    if (std::is_same<T, Polynomial>::value) return true;

    throw std::logic_error("Invalid type of operand in operation '" + op + "'!");
    return false;
}