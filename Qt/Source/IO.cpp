#include "IO.hpp"
#include <stdexcept>

namespace Library::IO
{
    TcpStream::TcpStream(QTcpSocket& socket) : socket(socket) {}

    void TcpStream::write(void const * data, size_t const size)
    {
        size_t total = 0;
        while (total < size)
        {
            char const * ptr = reinterpret_cast<char const *>(data) + total;
            size_t s = size - total;
            auto n = socket.write(ptr, s);
            if (n < 0) throw std::runtime_error("QTcpSocket write error");
            bool res = socket.waitForBytesWritten(timeoutMs);
            if(!res)
            {
                if(socket.error() == QAbstractSocket::SocketTimeoutError) continue;
                else throw std::runtime_error("QTcpSocket read timeout");
            }
            total += static_cast<size_t>(n);
        }
    }

    void TcpStream::read(void * data, size_t const size)
    {
        size_t total = 0;
        while (total < size)
        {
            bool res = socket.waitForReadyRead(timeoutMs);
            if(!res)
            {
                if(socket.error() == QAbstractSocket::SocketTimeoutError) continue;
                else throw std::runtime_error("QTcpSocket read timeout");
            }
            char * ptr = reinterpret_cast<char *>(data) + total;
            size_t s = size - total;
            auto n = socket.read(ptr, s);
            if (n < 0) throw std::runtime_error("QTcpSocket read error");
            total += static_cast<size_t>(n);
        }
    }

    std::unique_ptr<Stream> StreamFactory::tcp(QTcpSocket& socket)
    {
        return std::make_unique<TcpStream>(socket);
    }
}
