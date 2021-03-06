#pragma once

#include <pong/server/Common.hpp>
#include <pong/server/State.hpp>

#include <pong/server/Room.hpp>

namespace pong::server {

struct MainLobbyState : public State<MainLobbyState, user_t> {
    MainLobbyState(std::vector<std::unique_ptr<RoomState>>& _rooms) : State({
        // Receive
        { id_of(pong::packet::client::CreateRoom{}), &MainLobbyState::on_create_room },
        { id_of(pong::packet::client::EnterRoom{}), &MainLobbyState::on_enter_room }
    }), rooms{ _rooms } {}

    std::vector<std::unique_ptr<RoomState>>& rooms;

    void update_rooms() {
        unsigned id{ 0 };
        for(auto& room : rooms) {
            if (room && room->is_empty()) {
                std::cout << "update_rooms: Send OldRoom\n";
                broadcast(pong::packet::server::OldRoom{ id });
                room = nullptr;
            }

            ++id;
        }
    }

    std::vector<int> get_room_ids() const {
        std::vector<int> room_ids;

        for(std::size_t room_id{ 0 }; room_id < rooms.size(); ++room_id) {
            if (rooms[room_id] && !rooms[room_id]->is_empty()) {
                room_ids.emplace_back(static_cast<int>(room_id));
            }
        }

        return room_ids;
    }

    std::vector<std::string> get_all_usernames_except(user_handle_t except_handle) const {
        std::vector<std::string> usernames;
        usernames.reserve(number_of_user() - 1);


        for(user_handle_t h{ 0 }; h < number_of_user(); ++h) {
            if (is_valid(h) && h != except_handle) {
                usernames.emplace_back(get_user_data(h));
            }
        }

        return usernames;
    }

    Action on_create_room(user_handle_t handle, packet_t) {
        // Find an ID without a room
        std::size_t room_id{ 0 };
        for(; room_id < rooms.size(); ++room_id) {
            if (rooms[room_id] == nullptr || rooms[room_id]->is_empty()) {
                break;
            }
        }


        std::cout << "New room #" << room_id << " created\n";


        if (room_id >= rooms.size()) {
            rooms.emplace_back(std::make_unique<RoomState>(*this));


        } else if(rooms[room_id] == nullptr) {
            rooms[room_id] = std::make_unique<RoomState>(*this);


        }

        std::cout << "Send NewRoom\n";
        broadcast_other(handle, pong::packet::server::NewRoom{
            static_cast<unsigned>(room_id)
        });

        send(handle, pong::packet::server::CreateRoomResponse{
            pong::packet::server::CreateRoomResponse::Reason::Okay
        });

        return order_change_state(
            *rooms[room_id],
            handle,
            get_user_data(handle)
        );
    }


    Action on_enter_room(user_handle_t handle, packet_t packet) {
        auto room_id = static_cast<std::size_t>(from_packet<pong::packet::client::EnterRoom>(packet).id);
        if (room_id < rooms.size() && rooms[room_id] != nullptr) {
            std::cout << "Send EnterRoomResponse\n";
            send(handle, pong::packet::server::EnterRoomResponse{
                pong::packet::server::EnterRoomResponse::Result::Okay
            });

            return order_change_state(
                *rooms[room_id],
                handle,
                get_user_data(handle)
            );
            
        } else {
            std::cout << "Send EnterRoomResponse\n";
            send(handle, pong::packet::server::EnterRoomResponse{
                pong::packet::server::EnterRoomResponse::Result::InvalidID
            });
        }

        return Idle{};
    }


    void on_user_enter(user_handle_t handle) {

        auto room_ids = get_room_ids();
        auto usernames = get_all_usernames_except(handle);

        std::cout << "Send LobbyInfo with " << usernames.size() << " people\n";
        send(handle, pong::packet::server::LobbyInfo{
            std::move(usernames), std::move(room_ids)
        });

        std::cout << "Send NewUser\n";
        broadcast_other(handle, pong::packet::server::NewUser{
            get_user_data(handle)
        });
    }



    void on_user_leave(user_handle_t handle) {
        std::cout << "MainLobby::on_user_leave: Send OldUser\n";
        broadcast_other(handle, pong::packet::server::OldUser{
            get_user_data(handle)
        });
    }
};

}