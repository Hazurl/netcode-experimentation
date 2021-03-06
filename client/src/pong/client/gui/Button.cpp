#include <SFML/Graphics.hpp>

#include <sftk/animated/Animated.hpp>
#include <sftk/eventListener/EventListener.hpp>
#include <sftk/print/Printer.hpp>
#include <sftk/fancyText/FancyText.hpp>

#include <pong/client/gui/RoundedRectangle.hpp>
#include <pong/client/gui/Element.hpp>
#include <pong/client/gui/Color.hpp>
#include <pong/client/gui/Button.hpp>

#include <functional>
#include <cmath>
#include <iostream>

namespace pong::client::gui {



sf::Color const& Button::Theme::get(Button::State state) const {
    switch(state) {
        case Button::State::Idle: return idle;
        case Button::State::Clicked: return idle;
        case Button::State::Hovered: return hovered;
        default: return clicked;
    }
}

sf::Color& Button::Theme::get(Button::State state) {
    switch(state) {
        case Button::State::Idle: return idle;
        case Button::State::Clicked: return idle;
        case Button::State::Hovered: return hovered;
        default: return clicked;
    }
}







Button::Button(Allocator<> gui, std::function<void()> _on_click, Button::Theme _theme)
:   RectElement(gui)
,   rectangle({0, 0}, 4)
,   color(&color_transition, _theme.idle) 
,   theme(_theme)
,   state(State::Idle)
,   on_click_cb(std::move(_on_click))
{}






void Button::change_color() {
    color.animate(theme.get(state), 0.1f);
}


void Button::change_state(Button::State _state) {
    if (state != _state) {
        state = _state;
        change_color();
    }
}

bool Button::is_clicked(Button::State state) {
    switch(state) {
        case Button::State::Clicked:
        case Button::State::ClickedHovered: 
            return true;

        default:
            return false;
    }
}

bool Button::is_hover(Button::State state) {
    switch(state) {
        case Button::State::Hovered:
        case Button::State::ClickedHovered: 
            return true;

        default:
            return false;
    }
}

Button::State Button::make_state(bool hover, bool clicked) {
    if (hover && clicked) {
        return Button::State::ClickedHovered;
    }

    if (hover) {
        return Button::State::Hovered;
    }

    if (clicked) {
        return Button::State::Clicked;
    }

    return Button::State::Idle;
}








void Button::set_theme(Theme _theme) {
    theme = _theme;
    change_color();
}

void Button::setSize(sf::Vector2f const& _size) {
    rectangle.setSize(_size);
} 

sf::Vector2f const& Button::getSize() const {
    return rectangle.getSize();
} 

void Button::set_rounded_radius(float _radius) {
    rectangle.setRadius(_radius);
} 

float Button::get_rounded_radius() const {
    return rectangle.getRadius();
} 

void Button::set_rounded_precision(unsigned _precision) {
    rectangle.setPrecision(_precision);
} 

unsigned Button::get_rounded_precision() const {
    return rectangle.getPrecision();
}







void Button::update(float dt) {
    color.update(dt);
    rectangle.setFillColor(color);
}







sftk::PropagateEvent Button::on_mouse_button_pressed(sf::Window&, sf::Event::MouseButtonEvent const& b) {
    bool hovering = getTransform().transformRect(rectangle.getGlobalBounds()).contains(static_cast<float>(b.x), static_cast<float>(b.y));
    if (hovering) {
        change_state(State::Clicked);
        return false;
    }

    return true;
}

sftk::PropagateEvent Button::on_mouse_button_released(sf::Window&, sf::Event::MouseButtonEvent const& b) {
    if (state == State::Clicked) {
        
        bool hovering = getTransform().transformRect(rectangle.getGlobalBounds()).contains(static_cast<float>(b.x), static_cast<float>(b.y));
        if (hovering) {
            change_state(State::Hovered);
            on_click_cb();
            return false;
        } else {
            change_state(State::Idle);
        }
    }

    return true;
}

sftk::PropagateEvent Button::on_mouse_moved(sf::Window&, sf::Event::MouseMoveEvent const& b) {
    if (state == State::Clicked) {
        return true;
    }

    bool hovering = getTransform().transformRect(rectangle.getGlobalBounds()).contains(static_cast<float>(b.x), static_cast<float>(b.y));
    if (hovering && state != State::Hovered) {
        change_state(State::Hovered);
    } else if (!hovering && state == State::Hovered) {
        change_state(State::Idle);
    }

    return true;
}



bool Button::on_click(sf::Vector2f const& position) {
    bool hovering = getTransform().transformRect(rectangle.getGlobalBounds()).contains(position);
    change_state(make_state(hovering, true));

    return hovering;
}

bool Button::on_release_click(sf::Vector2f const& position) {
    bool hovering = getTransform().transformRect(rectangle.getGlobalBounds()).contains(position);
    change_state(make_state(hovering, false));

    return hovering;
}

bool Button::on_hover(sf::Vector2f const& position) {
    bool hovering = getTransform().transformRect(rectangle.getGlobalBounds()).contains(position);
    change_state(make_state(hovering, is_clicked(state)));

    return hovering;
}



/* 
    sf::Drawable
*/

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    target.draw(rectangle, states);
}


void Button::debug_draw(sf::RenderTarget& target, sf::RenderStates states) const {
    sf::RectangleShape rect{ getSize() };
    rect.setPosition(getPosition());
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(sf::Color::Magenta);
    rect.setOutlineThickness(1);

    target.draw(rect, states);
}



}