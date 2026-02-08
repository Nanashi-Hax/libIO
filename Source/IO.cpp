#include "IO.hpp"
#include "Network/Result.hpp"
#include <Network.hpp>
#include <stdexcept>

namespace Library::IO
{
    TcpStream::TcpStream(Network::TcpSocket& socket) : socket(socket) {}

    void TcpStream::write(void const * data, size_t const size)
    {
        size_t total = 0;
        while (total < size)
        {
            socket.waitWrite(timeoutMs);
            void const * ptr = reinterpret_cast<std::byte const *>(data) + total;
            size_t s = size - total;
            auto res = socket.send(ptr, s);
            if(res.type == Network::ResultType::Error) throw std::runtime_error("Failed to Socket::send()");
            if(res.type == Network::ResultType::Disconnected) throw std::runtime_error("Disconnected in Socket::send()");
            total += res.bytes;
        }
    }

    void TcpStream::read(void * data, size_t const size)
    {
        size_t total = 0;
        while (total < size)
        {
            socket.waitRead(timeoutMs);
            void * ptr = reinterpret_cast<std::byte *>(data) + total;
            size_t s = size - total;
            auto res = socket.recv(ptr, s);
            if(res.type == Network::ResultType::Error) throw std::runtime_error("Failed to Socket::recv()");
            if(res.type == Network::ResultType::Disconnected) throw std::runtime_error("Disconnected in Socket::recv()");
            total += res.bytes;
        }
    }

    std::unique_ptr<Stream> StreamFactory::tcp(Network::TcpSocket socket)
    {
        return std::make_unique<TcpStream>(socket);
    }
}