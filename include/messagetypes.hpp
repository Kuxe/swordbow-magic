#ifndef MESSAGETYPES_HPP
#define MESSAGETYPES_HPP

/** This is really just a labeled list of ints
    [UNKNOWN=0, OUTDATED=1, CONNECT=2, ...]
    These labels are stored in each packet
    First the serialized packet is deserialized into
    an untyped packet, then the type is checked,
    then the serialized packet is deserialized, again,
    into a typed packet that can retreive the data.
**/
enum MESSAGE_TYPE {
    UNKNOWN,
    OUTDATED,
    CONNECT,
    DISCONNECT,
    INPUTDATA,
    BEGIN_TRANSMITTING_INITIAL_COMPONENTS,
    INITIAL_COMPONENTS,
    END_TRANSMITTING_INITIAL_COMPONENTS,
    MOVECOMPONENTSDIFF,
    RENDERCOMPONENTSDIFF,
    PLAY_SOUND,
    REGISTER_ID_TO_SYSTEM,
    REMOVE_ID,
    REMOVE_ID_FROM_SYSTEM,
    REMOVE_ID_FROM_SYSTEMS,
    ACTIVATE_ID,
    CONGESTED_CLIENT,
    NOT_CONGESTED_CLIENT,
    KEEP_ALIVE,
};

#include "sounddata.hpp"
#include "components.hpp"
#include "movecomponent.hpp"
#include "rendercomponent.hpp"
#include "systemidentifiers.hpp"
#include "inputdata.hpp"

/** If the type of a packet ever changes, this is the place to change in
    Changes here should propagate through all out the program **/
using UNKNOWN_TYPE = bool;
using OUTDATED_TYPE = bool;
using CONNECT_TYPE = std::pair<ID, System::Identifier>;
using DISCONNECT_TYPE = bool;
using INPUTDATA_TYPE = InputData;
using BEGIN_TRANSMITTING_INITIAL_COMPONENTS_TYPE = int;
using INITIAL_COMPONENTS_TYPE = std::pair<Components<MoveComponent>, Components<RenderComponent>>;
using END_TRANSMITTING_INITIAL_COMPONENTS_TYPE = bool;
using MOVECOMPONENTSDIFF_TYPE = Components<MoveComponent>;
using RENDERCOMPONENTSDIFF_TYPE = Components<RenderComponent>;
using PLAY_SOUND_TYPE = SoundData;
using REGISTER_ID_TO_SYSTEM_TYPE = std::pair<ID, System::Identifier>;
using REMOVE_ID_TYPE = ID;
using REMOVE_ID_FROM_SYSTEM_TYPE = std::pair<ID, System::Identifier>;
using REMOVE_ID_FROM_SYSTEMS_TYPE = bool;
using ACTIVATE_ID_TYPE = std::pair<ID, System::Identifier>;
using CONGESTED_CLIENT_TYPE = bool;
using NOT_CONGESTED_CLIENT_TYPE = bool;
using KEEP_ALIVE_TYPE = bool;

#endif //MESSAGETYPES_HPP
