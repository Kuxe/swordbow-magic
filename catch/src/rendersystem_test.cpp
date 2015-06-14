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
}











//
