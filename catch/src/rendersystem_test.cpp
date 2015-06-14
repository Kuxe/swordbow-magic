#include "catch.hpp"
#include "hashgridsystem.hpp"
#include "spatialindexer.hpp"
#include "componentmanager.hpp"
#include "textureboundingbox.hpp"
#include "entitymanager.hpp"
#include "systemmanager.hpp"
#include "deltatime.hpp"
#include "idmanager.hpp"
#include "movesystem.hpp"
#include "inputsystem.hpp"
#include "camerasystem.hpp"
#include "keyboardsystem.hpp"
#include <list>

TEST_CASE("RenderSystem") {

    bool running;

    DeltaTime dt;
    ComponentManager cm;
    SystemManager sm(&cm, &dt);
    TextureBoundingBox tbb(&cm);
    IdManager im;
    EntityManager entityManager(&sm, &cm, &im);

    HashGridSystem hg(&cm, &tbb);
    RenderSystem rs;
    MoveSystem ms;
    InputSystem is;
    CameraSystem cs(&rs);

    //Couple systems where neccesary
	rs.setCameraSystem(&cs);

    sm.add(&rs);
    sm.add(&hg);
    sm.add(&ms);
    sm.add(&is);
    sm.add(&cs);

    auto player = entityManager.createDummySquare({0.0f, 0.0f});

    //Camera should revolve around the player
	entityManager.registerIdToSystem("CameraSystem", player);

    SECTION("intersection to cliprect") {
        auto tile1 = entityManager.createTile();
        auto tile2 = entityManager.createTile();
        cm.moveComponents.at(tile1).pos = {0.0f, 0.0f};
        cm.moveComponents.at(tile2).pos = {0.0f, 32.0f};

        dt.start();
        sm.update();
        dt.stop();

        auto& mc = cm.moveComponents.at(tile1);
        auto& rc = cm.renderComponents.at(tile1);

        //Get the intersection between tile1 within drawarea and the drawarea
        const auto intersection = Rect::getIntersection(
            {16.0f, 30.99f, 1.0f, 1.0f}, hg.getBoundingBox(tile1)
        );

        const auto clipSource = RenderSystem::intersectionToClipSource(
            intersection,
            mc.pos,
            rc.xoffset, rc.yoffset
        );

        const auto clipDestination = RenderSystem::intersectionToClipDestination(
            intersection
        );

        const RenderData renderData = {
            tile1, &rc, rc.textureData, clipSource, clipDestination
        };

        REQUIRE(clipSource.x == 16);
        REQUIRE(clipSource.y == 30);
        REQUIRE(clipSource.w == 1);
        REQUIRE(clipSource.h == 1);
    }

    SECTION("intersection to cliprect") {
        auto tile1 = entityManager.createTile();
        auto tile2 = entityManager.createTile();
        cm.moveComponents.at(tile1).pos = {0.0f, 0.0f};
        cm.moveComponents.at(tile2).pos = {0.0f, 32.0f};

        dt.start();
        sm.update();
        dt.stop();

        auto& mc = cm.moveComponents.at(tile1);
        auto& rc = cm.renderComponents.at(tile1);

        //Get the intersection between tile1 within drawarea and the drawarea
        const auto intersection = Rect::getIntersection(
            {0.0f, 31.947388f, 1.0f, 1.0f}, hg.getBoundingBox(tile1)
        );

        const auto clipSource = RenderSystem::intersectionToClipSource(
            intersection,
            mc.pos,
            rc.xoffset, rc.yoffset
        );

        const auto clipDestination = RenderSystem::intersectionToClipDestination(
            intersection
        );

        const RenderData renderData = {
            tile1, &rc, rc.textureData, clipSource, clipDestination
        };

        REQUIRE(clipSource.x == 0);
        REQUIRE(clipSource.y == 31);
        REQUIRE(clipSource.w == 1);
        REQUIRE(clipSource.h == 1);

        REQUIRE(clipDestination.x == 0);
        REQUIRE(clipDestination.y == 31);
        REQUIRE(clipDestination.w == 1);
        REQUIRE(clipDestination.h == 1);
    }
}
