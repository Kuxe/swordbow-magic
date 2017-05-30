#ifndef MESSAGETYPES_HPP
#define MESSAGETYPES_HPP

/** So you want to add a messaage for swordbow-magic? Tutorial incoming:

    1.  Define an element in MESSAGE_TYPE suitable for your message.
        This enum is used to figure out what packet was sent on the receiving side.

    2.  Create some "Data" struct which contains the data you want to send.
        Implement an "Archive" method similarily to how other structs have done it.
        This will make sure the Data-struct can be serialized and received without
        worrying about big-endian vs little-endian etc.

    3.  Implement a switch-case in PacketManger.hpp similarily to the other switch-cases
        for your MESSAGE_TYPE and your Data-struct

    4.  Head into PacketManager and declare a virtual method similarily to other
        virtual methods there but with your Data-struct instead. Only then may
        PacketHandlers actually override the virtual method and thus implement
        concrete behaviour for the packet

    5.  Head into your PacketHandler and override the virtual method in PacketManager,
        use the data within your Data-struct as you wish!

    6.  Done!

**/

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
#include "cereal/archives/portable_binary.hpp"
/** A packet inherits from this if the IpAddress should be accesible from within the packet.
    Some code within PacketManager (as of 2017-05-29) looks if packet inherits from ContainIP
    and if so, assign the senders IP to the variable ip within ContainIP struct (such that
    the packet, which inherits ContainIP, contains the IP) **/


/** Interface with two concrete classes realizing it. A packet
    inherits either from WithIP if the packet should contain the senders IP address
    (handy for server when server wanna know who sent what input!) or from WoutIP
    if the IP address should not be part of the packet (which is really what you should default to) **/
struct WithWoutIP {
    virtual void setIPIfPossible(const IpAddress& ip) { };
    virtual ~WithWoutIP() { }
};

struct WithIP : WithWoutIP {
    IpAddress ip;
    WithIP(const IpAddress& ip = IpAddress()) : ip(ip) { }
    void setIPIfPossible(const IpAddress& ip) override {
        this->ip = ip;
    }
    virtual ~WithIP() { }
};

struct WoutIP : WithWoutIP { };

/** This is where messages is tied with the expected datatype on that message **/
/** These structs are used when doing multiple-dispatch on different client-states **/
struct UnknownData : WoutIP {
    bool data;

    UnknownData(bool data = true) : data(data) { }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(data);
    }
};

struct OutdatedData : WoutIP {
    bool data;

    OutdatedData(bool data = true) : data(data) { }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(data);
    }
};
struct ConnectToServerData : WithIP {
    bool data;

    ConnectToServerData(bool data = true) : data(data) { }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(data);
    }
};
struct ServerReplyToConnectData : WoutIP {
    ID id;
    System::Identifier sysid;

    ServerReplyToConnectData(const ID id = -1, const System::Identifier sysid = System::Identifier::UNDEFINED) : id(id), sysid(sysid) { }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(id, sysid);
    }
};
struct DisconnectData : WithIP {
    bool data;

    DisconnectData(bool data = true) : data(data) { }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(data);
    }
};
struct InputDataData : WithIP {
    InputData data;

    InputDataData(InputData data = InputData()) : data(data) { }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(data);
    }
};
struct BeginTransmittingInitialComponentsData : WoutIP {
    int data;

    BeginTransmittingInitialComponentsData(bool data = true) : data(data) { }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(data);
    }
};
struct InitialComponentsData : WoutIP {
   Components<MoveComponent> mcs;
   Components<RenderComponent> rcs;

   InitialComponentsData(
    const Components<MoveComponent>& mcs = Components<MoveComponent>(),
    const Components<RenderComponent>& rcs = Components<RenderComponent>()) : mcs(mcs), rcs(rcs) { }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(mcs, rcs);
    }
};
struct EndTransmittingInitialComponentsData : WoutIP {
    bool data;

    EndTransmittingInitialComponentsData(bool data = true) : data(data) { }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(data);
    }
};
struct MoveComponentsDiffData : WoutIP {
    Components<MoveComponent> data;

    MoveComponentsDiffData(Components<MoveComponent> data = Components<MoveComponent>()) : data(data) { }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(data);
    }
};
struct RenderComponentsDiffData : WoutIP {
    Components<RenderComponent> data;

    RenderComponentsDiffData(Components<RenderComponent> data = Components<RenderComponent>()) : data(data) { }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(data);
    }
};
struct PlaySoundData : WoutIP {
    SoundData data;

    PlaySoundData(SoundData data = SoundData()) : data(data) { }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(data);
    }
};
struct RegisterIdToSystemData : WoutIP {
    ID id;
    System::Identifier sysid;

    RegisterIdToSystemData(ID id = -1, System::Identifier sysid = System::Identifier::UNDEFINED) : id(id), sysid(sysid) { }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(id, sysid);
    }
};
struct RemoveIdData : WoutIP {
    ID data;

    RemoveIdData(ID data = -1) : data(data) { }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(data);
    }
};
struct RemoveIdFromSystemData : WoutIP {
    ID id;
    System::Identifier sysid;

    RemoveIdFromSystemData(ID id = -1, System::Identifier sysid = System::Identifier::UNDEFINED) : id(id), sysid(sysid) { }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(id, sysid);
    }
};
struct RemoveIdFromSystemsData : WoutIP {
    ID id;

    RemoveIdFromSystemsData(ID id = -1) : id(id) { }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(id);
    }
};
struct ActivateIdData : WoutIP {
    ID id;
    System::Identifier sysid;

    ActivateIdData(ID id = -1, System::Identifier sysid = System::Identifier::UNDEFINED) : id(id), sysid(sysid) { }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(id, sysid);
    }
};
struct CongestedClientData : WithIP {
    bool data;

    CongestedClientData(bool data = true) : data(data) { }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(data);
    }
};
struct NotCongestedClientData : WithIP {
    bool data;

    NotCongestedClientData(bool data = true) : data(data) { }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(data);
    }
};
struct KeepAliveData : WoutIP {
    bool data;

    KeepAliveData(bool data = true) : data(data) { }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(data);
    }
};

#endif //MESSAGETYPES_HPP
