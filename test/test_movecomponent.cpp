#include "catch.hpp"
#include "movecomponent.h"

TEST_CASE("MoveComponent") {
	MoveComponent mc;
	SECTION("Initialization") {
		REQUIRE(mc.xpos == 0);
		REQUIRE(mc.ypos == 0);
		REQUIRE(mc.xspeed == 1);
		REQUIRE(mc.yspeed == 1);
	}
}