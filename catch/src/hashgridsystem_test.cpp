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

TEST_CASE("HashGridSystem") {

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

    SECTION("Query fully contained area of entity") {
        auto tile1 = entityManager.createTile();
        auto tile2 = entityManager.createTile();
        cm.moveComponents.at(player).pos = {16.0f, 16.947388f};
        cm.moveComponents.at(tile1).pos = {0.0f, 0.0f};
        cm.moveComponents.at(tile2).pos = {0.0f, 32.0f};

        dt.start();
        sm.update();
        dt.stop();

        //querrying the area of dummysquare should at this time give both tiles,
        //since dummysquare is located at {16.0f, 32.947388f} with w = h = 1

        auto bb = hg.getBoundingBox(player);
        auto queryids = hg.query(bb);

        //There should be 2 entities within the query of dummysquare
        REQUIRE(queryids.size() == 2);
    }

    SECTION("Query partially contained area of entity TRY1") {
        auto tile1 = entityManager.createTile();
        auto tile2 = entityManager.createTile();
        cm.moveComponents.at(player).pos = {16.0f, 31.947388f};
        cm.moveComponents.at(tile1).pos = {0.0f, 0.0f};
        cm.moveComponents.at(tile2).pos = {0.0f, 32.0f};

        dt.start();
        sm.update();
        dt.stop();

        auto bb = hg.getBoundingBox(player);
        auto queryids = hg.query(bb);

        //There should be 3 entities within the query of dummysquare
        REQUIRE(queryids.size() == 3);
        for(auto id : queryids) {
            cout << cm.nameComponents.at(id).name << endl;
        }
    }

    SECTION("Query partially contained area of entity TRY2") {
        auto tile1 = entityManager.createTile();
        auto tile2 = entityManager.createTile();
        cm.moveComponents.at(player).pos = {16.0f, 31.076195f};
        cm.moveComponents.at(tile1).pos = {0.0f, 0.0f};
        cm.moveComponents.at(tile2).pos = {0.0f, 32.0f};

        dt.start();
        sm.update();
        dt.stop();

        auto bb = hg.getBoundingBox(player);
        auto queryids = hg.query(bb);

        //There should be 3 entities within the query of dummysquare
        REQUIRE(queryids.size() == 3);
        for(auto id : queryids) {
            cout << cm.nameComponents.at(id).name << endl;
        }
    }
}
