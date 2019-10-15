#include <pong/client/state/MainLobby/MainLobby.hpp>

#include <pong/client/Logger.hpp>
#include <pong/client/Visitor.hpp>

#include <sftk/print/Printer.hpp>

#include <cmath>

namespace pong::client::state {

MainLobby::MainLobby(Application app, std::string _username) 
:   graphics(app)
,   username{ std::move(_username) }
{}

action::Actions MainLobby::on_window_event(Application, WindowEvent const& window_event) {
    return std::visit(Visitor{
        [this] (MouseButtonReleased const& event) {
            if (event.button == sf::Mouse::Button::Left) {
                if (auto button = graphics.on_release_click({ static_cast<float>(event.x), static_cast<float>(event.y) })) {
                    switch(*button) {
                        case mainlobby::Graphics::Button::Quit:
                            NOTICE("Pressed Quit button");
                            return action::seq(action::quit());

                        default: break;
                    }
                }
            }
            return action::Actions{}; 
        },

        [this] (MouseButtonPressed const& event) {
            if (event.button == sf::Mouse::Button::Left) {
                graphics.on_click({ static_cast<float>(event.x), static_cast<float>(event.y) });
            }
            return action::Actions{}; 
        },

        [this] (MouseMoved const& event) {
            graphics.on_hover({ static_cast<float>(event.x), static_cast<float>(event.y) });
            return action::Actions{}; 
        },

        [] (auto const&) {
            return action::Actions{};
        }
    }, window_event);
}

action::Actions MainLobby::on_send(Application, pong::packet::GamePacket const&) {
    return action::Actions{};
}

action::Actions MainLobby::on_receive(Application, pong::packet::GamePacket const& game_packet) {
    return action::Actions{};
}

action::Actions MainLobby::on_update(Application app, float dt) {
    graphics.update_animations(app, dt);
    return action::Actions{};
}

action::Actions MainLobby::on_connection(Application) {
    return action::Actions{};
}

action::Actions MainLobby::on_connection_failure(Application) {
    return action::Actions{};
}

action::Actions MainLobby::on_disconnection(Application) {
    return action::Actions{};
}

void MainLobby::notify_gui(gui::Gui<>& gui) const {
    graphics.notify_gui(gui);
}

void MainLobby::update_properties(gui::Gui<> const& gui) {
    graphics.update_properties(gui);
}

void MainLobby::free_properties(gui::Allocator<> gui) const {
    graphics.free_properties(gui);
}

void MainLobby::draw(Application app, sf::RenderTarget& target, sf::RenderStates states) const {
    graphics.draw(target, states);
}

}