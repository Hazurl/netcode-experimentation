#include <pong/client/net/PacketQueue.hpp>

#include <pong/client/Logger.hpp>
#include <pong/client/state/State.hpp>

namespace pong::client::net {

bool PacketQueue::push(pong::packet::client::Any new_packet) {
    if (packets.size() >= maximum_queue_size) {
        WARN("Maximum capacity reached");
        return false;
    }

    NOTICE("Push packet ", std::visit([] (auto const& p) { return p.name; }, new_packet));
    packets.emplace(std::move(new_packet));
    return true;
}

bool PacketQueue::empty() const {
    return packets.empty();
}

std::pair<Status, std::optional<pong::packet::client::Any>> PacketQueue::send(sf::TcpSocket& socket) {
    if (!partially_send) {
        if (empty()) {
            WARN("No packet in the queue");
            return std::make_pair(
                Status::Available,
                std::nullopt
            );
        }

        auto const& game_packet = packets.front();
        NOTICE("Prepare packet #", game_packet.index());
        packet.clear();
        packet << game_packet;
        partially_send = true;
    }

    switch(socket.send(packet)) {
        case sf::Socket::Status::Partial: {
            return std::make_pair(
                Status::Done,
                std::nullopt
            );
        }

        case sf::Socket::Status::Done: {
            SUCCESS("Packet ", std::visit([] (auto const& p) { return p.name; }, packets.front()), " sent");
            auto ret = std::make_pair(
                Status::Available,
                std::move(packets.front())
            );
            packets.pop();
            partially_send = false;
            return ret;
        }

        case sf::Socket::Status::NotReady: {
            WARN("Socket not ready to send a packet");
            return std::make_pair(
                Status::Done,
                std::nullopt
            );
        }

        default: {
            ERROR("Error when sending packet ", std::visit([] (auto const& p) { return p.name; }, packets.front()));
            return std::make_pair(
                Status::Error,
                std::nullopt
            );
        }
    }
}

}