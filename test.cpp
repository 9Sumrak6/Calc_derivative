#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "factory.hpp"


using namespace testing;


class TFunctionTest : public Test {
protected:
    Factory factory;

    void SetUp() override {}

    void TearDown() override {}
};

// Testing creation and basic functionality
// -----------------------------------------
TEST_F(TFunctionTest, CreateIdentFunction) {
    auto identFunc = factory.Create("ident");
    EXPECT_EQ(identFunc->ToString(), "x");
    EXPECT_DOUBLE_EQ((*identFunc)(1.2), 1.2);
    EXPECT_DOUBLE_EQ((*identFunc)(-1.1), -1.1);
}

TEST_F(TFunctionTest, CreateConstFunction) {
    auto constFunc = factory.Create("const", 42);
    EXPECT_EQ(constFunc->ToString(), "42.000");
    EXPECT_EQ((*constFunc)(10), 42);
}

TEST_F(TFunctionTest, CreatePowerFunction) {
    auto powerFunc = factory.Create("power", 2);
    EXPECT_EQ(powerFunc->ToString(), "x^2.000");
    EXPECT_DOUBLE_EQ((*powerFunc)(3), 9);
}

TEST_F(TFunctionTest, CreateExpFunction) {
    auto expFunc = factory.Create("exp", 0);
    EXPECT_EQ(expFunc->ToString(), "e^x");
    EXPECT_DOUBLE_EQ((*expFunc)(0), 1);
}

TEST_F(TFunctionTest, CreatePolynomialFunction) {
    auto expFunc = factory.Create("polynomial", {1, 0, 1, 1});
    EXPECT_EQ(expFunc->ToString(), "1.000 + 1.000*x^2 + 1.000*x^3");
    EXPECT_DOUBLE_EQ((*expFunc)(0), 1);
}

// Testing derivation
// -----------------------------------------------
TEST_F(TFunctionTest, DerivativeOfPowerFunction) {
    auto powerFunc = factory.Create("power", 3);
    EXPECT_DOUBLE_EQ(powerFunc->GetDeriv(2), 12);
}

TEST_F(TFunctionTest, DerivativeOfConstFunction) {
    auto powerFunc = factory.Create("const", 3);
    EXPECT_DOUBLE_EQ(powerFunc->GetDeriv(2), 0);
}

TEST_F(TFunctionTest, DerivativeOfIdentFunction) {
    auto powerFunc = factory.Create("ident", 3);
    EXPECT_DOUBLE_EQ(powerFunc->GetDeriv(2), 1);
    EXPECT_DOUBLE_EQ(powerFunc->GetDeriv(-1), 1);
}

TEST_F(TFunctionTest, DerivativeOfPolynommialFunction) {
    auto powerFunc = factory.Create("polynomial", {3, 2, 1});
    EXPECT_DOUBLE_EQ(powerFunc->GetDeriv(2), 6);
}

// Testing operations
// -----------------------------------------
TEST_F(TFunctionTest, AdditionOfFunctions) {
    auto f = factory.Create("power", 2);
    auto g = factory.Create("polynomial", {7, 0, 3, 15}); 
    auto p = *f + *g;

    EXPECT_DOUBLE_EQ(p.GetDeriv(1), 53);
    EXPECT_DOUBLE_EQ(p(2), 4+7+3*4+15*8);
    EXPECT_EQ(p.ToString(), "(x^2.000) + (7.000 + 3.000*x^2 + 15.000*x^3)");
}

TEST_F(TFunctionTest, SubtractionOfFunctions) {
    auto f = factory.Create("const", 100);
    auto g = factory.Create("exp"); 
    auto p = *f - *g;

    EXPECT_DOUBLE_EQ(roundToThreeDecimalPlaces(p.GetDeriv(1)), -2.718);
    EXPECT_DOUBLE_EQ(roundToThreeDecimalPlaces(p(1)), 97.282);
    EXPECT_EQ(p.ToString(), "(100.000) - (e^x)");
}

TEST_F(TFunctionTest, ProductOfFunctions) {
    auto f = factory.Create("power", 5);
    auto g = factory.Create("exp"); 
    auto p = *f * *g;

    EXPECT_DOUBLE_EQ(roundToThreeDecimalPlaces(p.GetDeriv(1)), 16.31);
    EXPECT_DOUBLE_EQ(roundToThreeDecimalPlaces(p(1)), 2.718);
    EXPECT_DOUBLE_EQ(p(0), 0);
    EXPECT_EQ(p.ToString(), "(x^5.000) * (e^x)");
}

TEST_F(TFunctionTest, DevisionOfFunctions) {
    auto f = factory.Create("power", 2);
    auto g = factory.Create("polynomial", {7, 0, 3, 15}); 
    auto p = *g / *f;

    EXPECT_DOUBLE_EQ(p(1), 25);
    EXPECT_DOUBLE_EQ(p(2), 34.75);
    EXPECT_DOUBLE_EQ(p.GetDeriv(1), 1);
    EXPECT_DOUBLE_EQ(p.GetDeriv(2), 13.25);
    EXPECT_EQ(p.ToString(), "(7.000 + 3.000*x^2 + 15.000*x^3) / (x^2.000)");
}

// Testing exceptions
// ------------------------------------------------------
TEST_F(TFunctionTest, InvalidOperationThrowsLogicError) {
    auto f = factory.Create("power", 2);
    string s = "abs";

    EXPECT_THROW(*f + "abc", std::logic_error);
    EXPECT_THROW("abc" + *f, std::logic_error);
    EXPECT_THROW(*f + s, std::logic_error);
    EXPECT_THROW(s + *f, std::logic_error);
}

TEST_F(TFunctionTest, Lala) {
    auto f = factory.Create("ident");
    string s = "ident";

    EXPECT_DOUBLE_EQ(("ident" + *f)(2), 4);
    EXPECT_DOUBLE_EQ(("const" + *f)(2), 3);
    EXPECT_DOUBLE_EQ(("exp" + *f)(0), 1);
    EXPECT_DOUBLE_EQ(("power" + *f)(2), 4);
    EXPECT_DOUBLE_EQ(("polynomial" + *f)(2), 3);
    EXPECT_DOUBLE_EQ((*f + "ident")(2), 4);
    EXPECT_DOUBLE_EQ((*f + "const")(2), 3);
    EXPECT_DOUBLE_EQ((*f + "exp")(0), 1);
    EXPECT_DOUBLE_EQ((*f + "power")(2), 4);
    EXPECT_DOUBLE_EQ((*f + "polynomial")(2), 3);
    EXPECT_DOUBLE_EQ((*f + s)(2), 4);
    EXPECT_DOUBLE_EQ((s + *f)(2), 4);
}

TEST_F(TFunctionTest, DivisionByZeroThrowsLogicError) {
    auto numerator = factory.Create("power", 2);

    auto denominator1 = factory.Create("const", 0);
    auto denominator2 = factory.Create("ident");

    EXPECT_THROW(*numerator / *denominator1, std::logic_error);
    EXPECT_THROW((*numerator / *denominator2)(0), std::logic_error);
}

// Testing gradient descending
// ----------------------------------------------------
TEST_F(TFunctionTest, GradDescendingTest) {
    auto f = factory.Create("polynomial", {7, 0, 3, 15}); // TPolynomial 7 + 3*x^2 + 15*x^3

    EXPECT_NEAR(f->GradientDescent(1), -0.848378, 0.0001);
    EXPECT_NEAR(f->GradientDescent(-1), -0.848378, 0.0001);
}

// main
// ------------------------------
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);

  return RUN_ALL_TESTS();
}