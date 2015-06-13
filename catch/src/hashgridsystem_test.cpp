#include "catch.hpp"
#include "hashgridsystem.hpp"
#include "spatialindexer.hpp"
#include "componentmanager.hpp"
#include "textureboundingbox.hpp"

TEST_CASE("HashGridSystem") {
    ComponentManager cm;
    TextureBoundingBox tbb(&cm);
    SpatialIndexer* hg = new HashGridSystem(&cm, &tbb);
}
