#include "IO/Transporter.hpp"
#include <mutex>

namespace Library::IO
{
    Transporter::Transporter(Network::TcpSocket& socket) : socket(socket) {}

    void Transporter::poll()
    {
        {
            std::lock_guard<std::mutex> lock(senderMutex);
            sender.poll(socket);
        }
        {
            std::lock_guard<std::mutex> lock(receiverMutex);
            receiver.poll(socket);
        }
    }

    bool Transporter::write(Packet const & packet)
    {
        std::lock_guard<std::mutex> lock(senderMutex);
        return sender.enqueue(packet);
    }

    bool Transporter::read(Packet & outPacket)
    {
        std::lock_guard<std::mutex> lock(receiverMutex);
        return receiver.dequeue(outPacket);
    }
}