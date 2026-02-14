#include "IO/Stream.hpp"
#include <stdexcept>
#include <cstring>

namespace Library::IO
{
    BufferStream::BufferStream() {}

    BufferStream::BufferStream(std::vector<std::byte> vector) : buffer(vector) {}

    BufferStream BufferStream::fromPacket(Packet packet)
    {
        return BufferStream(packet.buffer());
    }

    Packet BufferStream::toPacket(BufferStream stream)
    {
        return Packet(stream.buffer);
    }

    void BufferStream::write(void const* data, size_t const size)
    {
        auto begin = reinterpret_cast<std::byte const*>(data);
        buffer.insert(buffer.end(), begin, begin + size);
    }

    void BufferStream::read(void* data, size_t const size)
    {
        if (pos + size > buffer.size()) throw std::out_of_range("BufferStream::read()");
        std::memcpy(data, buffer.data() + pos, size);
        pos += size;
    }
}