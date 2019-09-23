#include <SFML/Network.hpp>

#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <memory>
#include <functional>
#include <atomic>
#include <algorithm>
#include <variant>
#include <unordered_set>

#include <multipong/Game.hpp>
#include <multipong/Packets.hpp>

#include <ServerState.hpp>

void client_runner(std::mutex& clients_mutex, std::vector<std::unique_ptr<sf::TcpSocket>>& clients, std::atomic_bool& stop) {
    std::vector<std::unique_ptr<RoomState>> rooms;
    MainLobbyState main_lobby{ rooms };
    NewUserState new_users{ main_lobby };

    while(!stop) {
        {
            std::lock_guard lk{ clients_mutex };

            for(auto& client : clients) {
                new_users.create(User{ std::move(client) });
            }

            clients.clear();
        }

        new_users.receive_packets();
        main_lobby.receive_packets();
        for(auto& room : rooms) {
            if(room) {
                room->receive_packets();
            }
        }

        new_users.send_packets();
        main_lobby.send_packets();
        for(auto& room : rooms) {
            if(room) {
                room->send_packets();
            }
        }
    }
}

int main() {
    sf::TcpListener listener;
    listener.listen(48622);

    std::mutex clients_mutex;
    std::vector<std::unique_ptr<sf::TcpSocket>> clients;
    std::atomic_bool stop_thread{ false };

    std::thread client_thread(client_runner, std::ref(clients_mutex), std::ref(clients), std::ref(stop_thread));

    while(true) {
        auto client = std::make_unique<sf::TcpSocket>();
        if (listener.accept(*client) != sf::Socket::Done) {
            std::cerr << "Error\n";
            continue;
        }

        std::cout << "New client: " << client->getRemoteAddress() << ":" << client->getRemotePort() << std::endl;
        client->setBlocking(false);
        
        std::lock_guard lk{ clients_mutex };
        clients.emplace_back(std::move(client));
    }

    stop_thread = true;

}

