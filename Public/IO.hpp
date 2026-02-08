#pragma once

#include <bit>
#include <memory>
#include <Network.hpp>

namespace Library::IO
{
    class Stream
    {
    public:
        virtual ~Stream() = default;

    protected:
        virtual void write(void const * data, size_t const size) = 0;
        virtual void read(void * data, size_t const size) = 0;
    };

    class TcpStream : public Stream
    {
    public:
        TcpStream(Network::TcpSocket socket);

        template<std::integral T>
        TcpStream& operator<<(T value)
        {
            value = std::byteswap(value);
            if(std::endian::native == std::endian::little) write(&value, sizeof(value));
            return *this;
        }

        template<std::integral T>
        TcpStream& operator>>(T& value)
        {
            read(&value, sizeof(value));
            if(std::endian::native == std::endian::little) value = std::byteswap(value);
            return *this;
        }

    private:
        void write(void const * data, size_t const size) override;
        void read(void * data, size_t const size) override;
    
        Network::TcpSocket socket;
        static constexpr const int timeoutMs = 1000;
    };

    class StreamFactory
    {
    public:
        static std::unique_ptr<Stream> tcp(Network::TcpSocket socket);
    };        
}