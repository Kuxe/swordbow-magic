#include "catch.hpp"
#include "rect.hpp"
#include <iostream>

TEST_CASE("Rect") {

    Rect a, b, expected;

    SECTION("Equality") {
        a = {1.0f, 1.0f, 1.0f, 1.0f};
        b = {1.0f, 1.0f, 1.0f, 1.0f};
        REQUIRE(a == b);

        a = {0.0f, 0.0f, 1.0f, 1.0f};
        b = {1.0f, 1.0f, 1.0f, 1.0f};
        REQUIRE(a != b);
    }

    SECTION("Equality (floating points)") {
        a = {1.3f, 1.6f, 0.3f, 3.9f};
        b = {1.3f, 1.6f, 0.3f, 3.9f};
        REQUIRE(a == b);

        a = {1.3001f, 1.6f, 0.3f, 3.9f};
        b = {1.3f, 1.6f, 0.3f, 3.9f};
        REQUIRE(a != b);
    }

    SECTION("Intersections using rects with integer positions") {

        //Two equally large rectangles should intersect to another equally
        //sized rect
        a = {1, 1, 1, 1};
        b = {1, 1, 1, 1};
        expected = {1, 1, 1, 1};
        REQUIRE(Rect::getIntersection(a, b) == expected);

        //Almost intersecting, but not really
        a = {0, 0, 1, 1};
        b = {1, 1, 1, 1};
        expected = {0, 0, 0, 0};
        REQUIRE(Rect::getIntersection(a, b) == expected);
    }

    SECTION("Intersections using rects with floating-point positions") {

        //Two equally large rectangles should intersect to another equally
        //sized rect
        a =         {1.3f, 1.6f, 0.3f, 3.9f};
        b =         {1.3f, 1.6f, 0.3f, 3.9f};
        expected =  {1.3f, 1.6f, 0.3f, 3.9f};
        REQUIRE(Rect::getIntersection(a, b) == expected);

        //Almost intersecting, but not really
        a = {0.0f, 0.0f, 1.0f, 0.9999999f};
        b = {1.0f, 1.0f, 1.0f, 1.0f};
        expected = {0.0f, 0.0f, 0.0f, 0.0f};
        REQUIRE(Rect::getIntersection(a, b) == expected);

        //Some weird intersection
        a =         {1.3f, 1.6f, 0.3f, 3.9f};
        b =         {0.4f, 2.1f, 1.1f, 1.7f};
        expected =  {1.3f, 2.1f, 0.2f, 1.7f};
        REQUIRE(Rect::getIntersection(a, b) == expected);

        a =         {0.0f, 31.947388f, 1.0f, 1.0f};
        b =         {0.0f, 32.0f, 32.0f, 32.0f};
        expected =  {0.0f, 32.0f, 1.0f, 31.947388f + 1.0f - 32.0f};
        REQUIRE(Rect::getIntersection(a, b) == expected);

        a =         {0.0f, 31.947388f, 1.0f, 1.0f};
        b =         {0.0f, 0.0f, 32.0f, 32.0f};
        expected =  {0.0f, 31.947388f, 1.0f, 32.0f - 31.947388f};
        REQUIRE(Rect::getIntersection(a, b) == expected);
    }
}
