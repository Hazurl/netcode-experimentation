#include <pong/client/state/MainLobby/Graphics.hpp>

namespace pong::client::state::mainlobby {

Graphics::Graphics(Application app)
:   title_txt(app.gui_allocator(), "MULTIPLAYER PONG", app.get_font())
,   by_hazurl_txt(app.gui_allocator(), "by Hazurl", app.get_font())
,   quit_txt(app.gui_allocator(), "QUIT", app.get_font(), 20)
,   quit_button(
        app.gui_allocator(), 
        [] () {},
        gui::Button::Theme{
            sf::Color{ 0x3A, 0x3C, 0x46 },
            sf::Color{ 0x31, 0x33, 0x3a },
            sf::Color{ 0x24, 0x25, 0x29 },
        })
{
    //////////////
    // Title
    //////////////

    title_txt.setFillColor({ 0xFF, 0x99, 0x00 });

    app.set_constraint<[] (float width, float window_width) {
        return (window_width - width) / 2.f;
    }>(title_txt.left(), { title_txt.width(), app.width_property() });

    app.set_constraint<[] (float height, float window_height) {
        return window_height / 4.f - height / 2.f;
    }>(title_txt.top(), { title_txt.size(), app.height_property() });

    app.set_constraint<[] (float window_height) {
        return window_height / 10.f;
    }>(title_txt.size(), { app.height_property() });


    //////////////
    // By Hazurl
    //////////////

    by_hazurl_txt.setFillColor(sf::Color::White);

    app.set_constraint<[] () {
        return 10.f;
    }>(by_hazurl_txt.left(), {});

    app.set_constraint<[] (float height, float window_height) {
        return window_height - 10.f - height;
    }>(by_hazurl_txt.top(), { by_hazurl_txt.size(), app.height_property() });

    app.set_constraint<[] (float window_height) {
        return window_height / 40.f;
    }>(by_hazurl_txt.size(), { app.height_property() });



    //////////////
    // Quit
    //////////////

    quit_txt.setFillColor(sf::Color::White);

    app.set_constraint<[] (float left_offset, float width, float w_width) {
        return w_width - left_offset - width - 10.f;
    }>(quit_txt.left(), { quit_txt.left_offset(), quit_txt.width(), app.width_property() });

    app.set_constraint<[] (float top_offset) {
        return top_offset + 10.f;
    }>(quit_txt.top(), { quit_txt.top_offset()});


    //////////////
    // Quit button
    //////////////

    app.set_constraint<[] (float qt_left, float qt_left_offset) {
        return qt_left + qt_left_offset - 5.f;
    }>(quit_button.left(), { quit_txt.left(), quit_txt.left_offset() });

    app.set_constraint<[] (float qt_top, float qt_top_offset) {
        return qt_top + qt_top_offset - 5.f;
    }>(quit_button.top(), { quit_txt.top(), quit_txt.top_offset() });

    app.set_constraint<[] (float qt_width) {
        return qt_width + 10.f;
    }>(quit_button.width(), { quit_txt.width() });

    app.set_constraint<[] (float qt_height) {
        return qt_height + 10.f;
    }>(quit_button.height(), { quit_txt.height() });

}

void Graphics::free_properties(gui::Allocator<> gui) const {
    title_txt.free_properties(gui);
    by_hazurl_txt.free_properties(gui);
    quit_txt.free_properties(gui);
    quit_button.free_properties(gui);
}


std::optional<Graphics::Button> Graphics::on_click(sf::Vector2f const& position) {
    if (quit_button.on_click(position)) {
        return Graphics::Button::Quit;
    }

    return std::nullopt;
}

std::optional<Graphics::Button> Graphics::on_release_click(sf::Vector2f const& position) {
    if (quit_button.on_release_click(position)) {
        return Graphics::Button::Quit;
    }

    return std::nullopt;
}

void Graphics::on_hover(sf::Vector2f const& position) {
    quit_button.on_hover(position);
}


void Graphics::update_animations(Application app, float dt) {
    quit_button.update(dt);
}

void Graphics::notify_gui(gui::Gui<>& gui) const {
    title_txt.notify_gui(gui);
    by_hazurl_txt.notify_gui(gui);
    quit_txt.notify_gui(gui);
    quit_button.notify_gui(gui);
}

void Graphics::update_properties(gui::Gui<> const& gui) {
    title_txt.update_properties(gui);
    by_hazurl_txt.update_properties(gui);
    quit_txt.update_properties(gui);
    quit_button.update_properties(gui);
}

void Graphics::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(title_txt, states);
    target.draw(by_hazurl_txt, states);
    target.draw(quit_button, states);
    target.draw(quit_txt, states);
}

}