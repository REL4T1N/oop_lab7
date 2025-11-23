#include <gtest/gtest.h>
#include <cmath>
#include <stdexcept>
#include "../include/geom/point.h"

TEST(PointTest, DefaultConstructor) {
    Point p;
    EXPECT_EQ(p.getX(), 0);
    EXPECT_EQ(p.getY(), 0);
}

TEST(PointTest, ParameterizedConstructor) {
    Point p(100, 200);
    EXPECT_EQ(p.getX(), 100);
    EXPECT_EQ(p.getY(), 200);
}

TEST(PointTest, ConstructorValidation) {
    EXPECT_THROW(Point(-1, 100), std::out_of_range);
    EXPECT_THROW(Point(100, -1), std::out_of_range);
    EXPECT_THROW(Point(600, 100), std::out_of_range);
    EXPECT_THROW(Point(100, 600), std::out_of_range);
    EXPECT_NO_THROW(Point(0, 0));
    EXPECT_NO_THROW(Point(500, 500));
}

TEST(PointTest, Setters) {
    Point p(10, 20);
    
    p.setX(30);
    EXPECT_EQ(p.getX(), 30);
    EXPECT_EQ(p.getY(), 20);
    
    p.setY(40);
    EXPECT_EQ(p.getX(), 30);
    EXPECT_EQ(p.getY(), 40);
    
    p.setCoordinates(50, 60);
    EXPECT_EQ(p.getX(), 50);
    EXPECT_EQ(p.getY(), 60);
    
    EXPECT_THROW(p.setX(-1), std::out_of_range);
    EXPECT_THROW(p.setY(600), std::out_of_range);
    EXPECT_THROW(p.setCoordinates(-10, 600), std::out_of_range);
}

TEST(PointTest, SetterValidation) {
    Point p(100, 100);
    
    EXPECT_THROW(p.setX(-1), std::out_of_range);
    EXPECT_THROW(p.setY(600), std::out_of_range);
    EXPECT_THROW(p.setCoordinates(-10, 600), std::out_of_range);
    
    // Проверяем что значения не изменились при исключении
    EXPECT_EQ(p.getX(), 100);
    EXPECT_EQ(p.getY(), 100);
}

TEST(PointTest, DistanceCalculation) {
    Point p1(0, 0);
    Point p2(3, 4);
    
    EXPECT_DOUBLE_EQ(p1.distanceTo(p2), 5.0);
    EXPECT_DOUBLE_EQ(p2.distanceTo(p1), 5.0);
    
    Point p3(100, 100);
    Point p4(100, 100);
    EXPECT_DOUBLE_EQ(p3.distanceTo(p4), 0.0);
}

TEST(PointTest, EqualityOperators) {
    Point p1(10, 20);
    Point p2(10, 20);
    Point p3(30, 40);
    
    EXPECT_TRUE(p1 == p2);
    EXPECT_FALSE(p1 == p3);
    EXPECT_TRUE(p1 != p3);
    EXPECT_FALSE(p1 != p2);
}

TEST(PointTest, OutputOperator) {
    Point p(123, 456);
    std::ostringstream oss;
    oss << p;
    EXPECT_EQ(oss.str(), "123 456");
}

TEST(PointTest, EdgeCases) {
    // Граничные значения
    EXPECT_NO_THROW(Point(0, 0));
    EXPECT_NO_THROW(Point(500, 500));
    EXPECT_NO_THROW(Point(0, 500));
    EXPECT_NO_THROW(Point(500, 0));
    
    // За границами
    EXPECT_THROW(Point(-1, 0), std::out_of_range);
    EXPECT_THROW(Point(0, -1), std::out_of_range);
    EXPECT_THROW(Point(501, 0), std::out_of_range);
    EXPECT_THROW(Point(0, 501), std::out_of_range);
}