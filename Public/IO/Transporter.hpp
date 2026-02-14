#pragma once

#include "IO/Packet.hpp"
#include <mutex>
#include <Network.hpp>

namespace Library::IO
{
    class Transporter
    {
    public:
        Transporter(Network::TcpSocket& socket);

        void poll();
        bool write(Packet const & packet);
        bool read(Packet & outPacket);

    private:
        bool send(Packet const & packet);
        bool receive(Packet & outPacket);
    
        Network::TcpSocket& socket;

        PacketSender sender;
        std::mutex senderMutex;
        PacketReceiver receiver;
        std::mutex receiverMutex;
    };
}