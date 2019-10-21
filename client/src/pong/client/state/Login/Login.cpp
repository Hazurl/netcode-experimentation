#include <pong/client/state/Login/Login.hpp>

#include <pong/client/Logger.hpp>
#include <pong/client/Visitor.hpp>

#include <pong/client/state/MainLobby/MainLobby.hpp>

#include <sftk/print/Printer.hpp>

#include <cmath>

namespace pong::client::state {

Login::Login(Application app) 
:   graphics(app)
{}

action::Actions Login::on_window_event(Application, WindowEvent const& window_event) {
    return std::visit(Visitor{
        [this] (MouseButtonReleased const& event) {
            if (event.button == sf::Mouse::Button::Left) {
                if (auto button = graphics.on_release_click({ static_cast<float>(event.x), static_cast<float>(event.y) })) {
                    switch(*button) {
                        case login::Graphics::Button::Quit:
                            NOTICE("Pressed Quit button");
                            return action::seq(action::quit());

                        default: break;
                    }
                }
            }
            return action::idle(); 
        },

        [this] (MouseButtonPressed const& event) {
            if (event.button == sf::Mouse::Button::Left) {
                graphics.on_click({ static_cast<float>(event.x), static_cast<float>(event.y) });
            }
            return action::idle(); 
        },

        [this] (MouseMoved const& event) {
            graphics.on_hover({ static_cast<float>(event.x), static_cast<float>(event.y) });
            return action::idle(); 
        },

        [this] (TextEntered const& event) {
            graphics.on_character_entered(event.unicode);
            return action::idle(); 
        },

        [this] (KeyPressed const& event) {
            if (event.code == sf::Keyboard::Left) {
                graphics.cursor_left();
            }

            else if (event.code == sf::Keyboard::Right) {
                graphics.cursor_right();
            }

            else if (event.code == sf::Keyboard::Enter) {
                if (auto valid_username = graphics.validate_username()) {
                    username = std::move(valid_username);
                    return action::seq(action::connect("127.0.0.1", 48624));
                }
            }

            return action::idle(); 
        },

        [] (auto const&) {
            return action::idle();
        }
    }, window_event);
}

action::Actions Login::on_send(Application, pong::packet::client::Any const&) {
    return action::idle();
}

action::Actions Login::on_receive(Application app, pong::packet::server::Any const& game_packet) {
    if (auto* response = std::get_if<pong::packet::server::UsernameResponse>(&game_packet)) {
        if(response->valid) {
            SUCCESS("Change state");
            return action::seq(action::change_state<MainLobby>(app, std::move(*username)));
        } else {
            ERROR("Invalid username");
            return action::idle();
        }
    }

    return action::idle();
}

action::Actions Login::on_update(Application app, float dt) {
    graphics.update_animations(app, dt);
    return action::idle();
}

action::Actions Login::on_connection(Application) {
    assert(username.has_value());

    auto actions = action::seq(action::send(pong::packet::client::ChangeUsername{ *username }));
    return actions;
}

action::Actions Login::on_connection_failure(Application) {
    ERROR("Connection Failure");
    username = std::nullopt;
    return action::idle();
}

action::Actions Login::on_disconnection(Application) {
    ERROR("Unexpected disconnection");
    username = std::nullopt;
    return action::idle();
}

void Login::notify_gui(gui::Gui<>& gui) const {
    graphics.notify_gui(gui);
}

void Login::update_properties(gui::Gui<> const& gui) {
    graphics.update_properties(gui);
}

void Login::free_properties(gui::Allocator<> gui) const {
    graphics.free_properties(gui);
}

void Login::draw(Application app, sf::RenderTarget& target, sf::RenderStates states) const {
    graphics.draw(app.is_connecting() || app.is_connected(), target, states);
}

}