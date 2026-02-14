#pragma once

#include <vector>
#include <span>
#include <Network.hpp>

#include "IO/Packet.hpp"

namespace Library::IO
{
    class Stream
    {
    public:
        virtual ~Stream() = default;

        template<std::integral T>
        Stream& operator<<(T value)
        {
            if(std::endian::native == std::endian::little) value = std::byteswap(value);
            write(&value, sizeof(value));
            return *this;
        }

        template<std::integral T>
        Stream& operator>>(T& value)
        {
            read(&value, sizeof(value));
            if(std::endian::native == std::endian::little) value = std::byteswap(value);
            return *this;
        }

        Stream& operator<<(std::span<std::byte> value)
        {
            write(value.data(), value.size());
            return *this;
        }

        Stream& operator>>(std::span<std::byte> value)
        {
            read(value.data(), value.size());
            return *this;
        }

    protected:
        virtual void write(void const * data, size_t const size) = 0;
        virtual void read(void * data, size_t const size) = 0;
    };

    class BufferStream : public Stream
    {
    public:
        BufferStream();
        BufferStream(std::vector<std::byte> vector);

        static BufferStream fromPacket(Packet packet);
        static Packet toPacket(BufferStream stream);

    private:
        void write(void const * data, size_t const size) override;
        void read(void * data, size_t const size) override;

        std::vector<std::byte> buffer;
        size_t pos = 0;
    };
}