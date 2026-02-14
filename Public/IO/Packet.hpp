#pragma once

#include <cstdint>
#include <vector>
#include <queue>

#include <Network.hpp>

namespace Library::IO
{
    class Packet
    {
    public:
        Packet();
        Packet(std::vector<std::byte> vector);

        std::vector<std::byte> buffer();
        std::byte const * data() const;
        size_t size() const;

        static constexpr const uint32_t MaxSize = 1024 * 1024;

    private:
        std::vector<std::byte> _data;
    };

    class PacketSender
    {
    public:
        bool enqueue(Packet const & packet);
        bool poll(Network::TcpSocket & socket);

    private:
        std::vector<std::byte> _buffer;
        size_t _offset = 0;
    };

    class PacketReceiver
    {
    public:
        bool dequeue(Packet & outPacket);
        bool poll(Network::TcpSocket & socket);

    private:
        enum class State
        {
            ReadingSize,
            ReadingData
        };

        std::vector<std::byte> _buffer;
        size_t _offset = 0;
        State _state = State::ReadingSize;
        uint32_t _dataSize = 0;
        std::queue<Packet> _packets;
    };
}