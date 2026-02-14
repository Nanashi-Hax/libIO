#include <Network.hpp>

#include <stdexcept>
#include <cstring>
#include <vector>

#include "IO/Packet.hpp"
#include "Network/Result.hpp"

namespace Library::IO
{
    Packet::Packet() {}
    Packet::Packet(std::vector<std::byte> vector) : _data(vector) {}

    std::vector<std::byte> Packet::buffer()
    {
        return _data;
    }

    std::byte const * Packet::data() const
    {
        return _data.data();
    }

    size_t Packet::size() const
    {
        return _data.size();
    }

    bool PacketSender::enqueue(Packet const & packet)
    {
        uint32_t size = packet.size();

        size_t oldSize = _buffer.size();
        size_t newSize = sizeof(uint32_t) + size;
        _buffer.resize(oldSize + newSize);

        uint32_t sizeNetwork = size;
        if(std::endian::native == std::endian::little) sizeNetwork = std::byteswap(sizeNetwork);
        std::memcpy(_buffer.data() + oldSize, &sizeNetwork, sizeof(sizeNetwork));
        std::memcpy(_buffer.data() + oldSize + sizeof(size), packet.data(), size);

        return true;
    }

    bool PacketSender::poll(Network::TcpSocket& socket)
    {
        while (_offset < _buffer.size())
        {
            auto data = _buffer.data() + _offset;
            auto size = _buffer.size() - _offset;
            auto res = socket.send(data, size);

            if (res.type == Network::ResultType::WouldBlock) return false;
            if (res.type == Network::ResultType::Error) throw std::runtime_error("Send error");
            if (res.type == Network::ResultType::Disconnected) throw std::runtime_error("Disconnected");

            _offset += res.bytes;
        }

        _buffer.clear();
        _offset = 0;
        return true;
    }

    bool PacketReceiver::poll(Network::TcpSocket& socket)
    {
        bool receivedAny = false;

        while (true)
        {
            if (_state == State::ReadingSize)
            {
                if (_buffer.size() != sizeof(uint32_t)) _buffer.resize(sizeof(uint32_t));

                auto data = _buffer.data() + _offset;
                auto size = sizeof(uint32_t) - _offset;
                auto res = socket.recv(data, size);

                if (res.type == Network::ResultType::WouldBlock) return false;
                if (res.type == Network::ResultType::Error) throw std::runtime_error("Recv error");
                if (res.type == Network::ResultType::Disconnected) throw std::runtime_error("Disconnected");

                _offset += res.bytes;

                if (_offset == sizeof(uint32_t))
                {
                    std::memcpy(&_dataSize, _buffer.data(), sizeof(_dataSize));
                    if(std::endian::native == std::endian::little) _dataSize = std::byteswap(_dataSize);
                    if (_dataSize > Packet::MaxSize) throw std::runtime_error("Packet too large");

                    _buffer.clear();
                    _buffer.resize(_dataSize);

                    _offset = 0;
                    _state = State::ReadingData;
                }
            }
            else if(_state == State::ReadingData)
            {
                auto data = _buffer.data() + _offset;
                auto size = _dataSize - _offset;
                auto res = socket.recv(data, size);

                if (res.type == Network::ResultType::WouldBlock) return false;
                if (res.type == Network::ResultType::Error) throw std::runtime_error("Recv error");
                if (res.type == Network::ResultType::Disconnected) throw std::runtime_error("Disconnected");

                _offset += res.bytes;

                if (_offset == _dataSize)
                {
                    _packets.push(Packet(std::move(_buffer)));
                    _buffer.clear();
                    _offset = 0;
                    _state = State::ReadingSize;
                    receivedAny = true;
                }
            }
        }
        return receivedAny;
    }
    
    bool PacketReceiver::dequeue(Packet& outPacket)
    {
        if (_packets.empty()) return false;
        outPacket = std::move(_packets.front());
        _packets.pop();
        return true;
    }
}