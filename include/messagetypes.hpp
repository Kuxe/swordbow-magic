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
    ANY,
    OUTDATED,
    CONNECT_TO_SERVER,
    SERVER_REPLY_TO_CONNECT,
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
#include "ipaddress.hpp"

/** A packet inherits from this if the IpAddress should be accesible from within the packet.
    Some code within PacketManager (as of 2017-05-29) looks if packet inherits from ContainIP
    and if so, assign the senders IP to the variable ip within ContainIP struct (such that
    the packet, which inherits ContainIP, contains the IP) **/
struct ContainIP {
    IpAddress ip;
};

/** This is where messages is tied with the expected datatype on that message **/
/** These structs are used when doing multiple-dispatch on different client-states **/
struct UnknownData { bool data; };
struct OutdatedData { bool data; };
struct ConnectToServerData : ContainIP { bool data; };
struct ServerReplyToConnectData { std::pair<ID, System::Identifier> data; };
struct DisconnectData : ContainIP { bool data; };
struct InputDataData : ContainIP { InputData data; };
struct BeginTransmittingInitialComponentsData { int data; };
struct InitialComponentsData { std::pair<Components<MoveComponent>, Components<RenderComponent>> data; };
struct EndTransmittingInitialComponentsData { bool data; };
struct MoveComponentsDiffData { Components<MoveComponent> data; };
struct RenderComponentsDiffData { Components<RenderComponent> data; };
struct PlaySoundData { SoundData data; };
struct RegisterIdToSystemData { std::pair<ID, System::Identifier> data; };
struct RemoveIdData { ID data; };
struct RemoveIdFromSystemData { std::pair<ID, System::Identifier> data; };
struct RemoveIdFromSystemsData { bool data; };
struct ActivateIdData { std::pair<ID, System::Identifier> data; };
struct CongestedClientData : ContainIP { bool data; };
struct NotCongestedClientData : ContainIP { bool data; };
struct KeepAliveData { bool data; };

#endif //MESSAGETYPES_HPP
