#ifndef CREATEBLOODSPLATTER_HPP
#define CREATEBLOODSPLATTER_HPP

#include "icommand.hpp"
#include "entitymanager.hpp"
#include "componentmanager.hpp"

typedef unsigned int ID;

struct CreateBloodsplatter : public ICommand {
    EntityManager* entityManager;
    ID id; //On what id should the bloodsplatter be created?

    constexpr CreateBloodsplatter(EntityManager* entityManager, ID id) :
        entityManager(entityManager),
        id(id) { }

    void execute() {
        entityManager->createBloodSplatter(entityManager->componentManager->moveComponents.at(id).transform);
    }
};



#endif //CREATEBLOODSPLATTER_HPP
