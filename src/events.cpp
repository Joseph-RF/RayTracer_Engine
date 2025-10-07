#include <events.hpp>

Event::Event(Action action, float mouse_xpos, float mouse_ypos, float scroll_x, float scroll_y)
    : action(action)
    , mouse_xpos(mouse_xpos)
    , mouse_ypos(mouse_ypos)
    , scroll_x(scroll_x)
    , scroll_y(scroll_y) {}

EventManager::EventManager() {
    pressed_keys[Key::W]      = false;
    pressed_keys[Key::A]      = false;
    pressed_keys[Key::S]      = false;
    pressed_keys[Key::D]      = false;
    pressed_keys[Key::L_CTRL] = false;
    pressed_keys[Key::SPACE]  = false;
    pressed_keys[Key::M]      = false;
    pressed_keys[Key::G]      = false;
    pressed_keys[Key::F1]     = false;
    pressed_keys[Key::F2]     = false;
    pressed_keys[Key::ESC]    = false;
    pressed_keys[Key::OTHER]  = false;

    pressed_buttons[MouseButton::LEFT]  = false;
    pressed_buttons[MouseButton::RIGHT] = false;
    pressed_buttons[MouseButton::OTHER] = false;
}

void EventManager::update() {
    if (pressed_keys[Key::W]) {
        events.push(Event(Action::MOVE_FORWARD, 0.0f, 0.0f, 0.0f, 0.0f));
    }
    if (pressed_keys[Key::A]) {
        events.push(Event(Action::MOVE_LEFT, 0.0f, 0.0f, 0.0f, 0.0f));
    }
    if (pressed_keys[Key::S]) {
        events.push(Event(Action::MOVE_BACKWARD, 0.0f, 0.0f, 0.0f, 0.0f));
    }
    if (pressed_keys[Key::D]) {
        events.push(Event(Action::MOVE_RIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
    }
    if (pressed_keys[Key::L_CTRL]) {
        events.push(Event(Action::MOVE_DOWN, 0.0f, 0.0f, 0.0f, 0.0f));
    }
    if (pressed_keys[Key::SPACE]) {
        events.push(Event(Action::MOVE_UP, 0.0f, 0.0f, 0.0f, 0.0f));
    }
    if (pressed_buttons[MouseButton::LEFT]) {
        events.push(Event(Action::L_BUTTON_PRESSED, 0.0f, 0.0f, 0.0f, 0.0));
    }
    if (pressed_buttons[MouseButton::RIGHT]) {
        events.push(Event(Action::R_BUTTON_PRESSED, 0.0f, 0.0f, 0.0f, 0.0));
    }
}

void EventManager::addScreenResizeEvent(int width, int height) {
    events.push(
        Event(Action::SCREEN_RESIZE, static_cast<float>(width), static_cast<float>(height), 0.0f, 0.0f));
}

void EventManager::keyEvent(Key key, bool pressed) {
    switch (key) {
    case Key::W:
        pressed_keys[Key::W] = pressed;
        break;
    case Key::A:
        pressed_keys[Key::A] = pressed;
        break;
    case Key::S:
        pressed_keys[Key::S] = pressed;
        break;
    case Key::D:
        pressed_keys[Key::D] = pressed;
        break;
    case Key::L_CTRL:
        pressed_keys[Key::L_CTRL] = pressed;
        break;
    case Key::SPACE:
        pressed_keys[Key::SPACE] = pressed;
        break;
    case Key::M:
        if (pressed_keys[Key::M] && !pressed) {
            // If button was just released
            events.push(Event(Action::TOGGLE_MOUSE, 0.0f, 0.0f, 0.0f, 0.0f));
        }
        pressed_keys[Key::M] = pressed;
        break;
    case Key::G:
        if (pressed_keys[Key::G] && !pressed) {
            // If button was just released
            events.push(Event(Action::TOGGLE_GIZMO, 0.0f, 0.0f, 0.0f, 0.0f));
        }
        pressed_keys[Key::G] = pressed;
        break;
    case Key::F1:
        if (pressed_keys[Key::F1] && !pressed) {
            // If button was just released
            events.push(Event(Action::SAVE_SCENE, 0.0f, 0.0f, 0.0f, 0.0f));
        }
        pressed_keys[Key::F1] = pressed;
        break;
    case Key::F2:
        if (pressed_keys[Key::F2] && !pressed) {
            // If button was just released
            events.push(Event(Action::LOAD_SCENE, 0.0f, 0.0f, 0.0f, 0.0f));
        }
        pressed_keys[Key::F2] = pressed;
        break;
    default:
        break;
    }
}

void EventManager::addMouseMovedEvent(float mouse_xpos, float mouse_ypos, bool turning) {
    if (turning) {
        events.push(Event(Action::TURN, mouse_xpos, mouse_ypos, 0.0f, 0.0f));
    } else {
        events.push(Event(Action::MOVE_CURSOR, mouse_xpos, mouse_ypos, 0.0f, 0.0f));
    }
}

void EventManager::addScrollEvent(float scroll_x, float scroll_y) {
    events.push(Event(Action::ZOOM, 0.0f, 0.0f, scroll_x, scroll_y));
}

void EventManager::addMouseClickEvent(MouseButton button, bool pressed) {
    switch (button) {
    case MouseButton::LEFT:
        if (pressed_buttons[MouseButton::LEFT] && !pressed) {
            // If button was just released
            events.push(Event(Action::L_CLICK, 0.0f, 0.0f, 0.0f, 0.0f));
        }
        pressed_buttons[MouseButton::LEFT] = pressed;
        break;
    case MouseButton::RIGHT:
        if (pressed_buttons[MouseButton::RIGHT] && !pressed) {
            // If button was just released
            events.push(Event(Action::R_CLICK, 0.0f, 0.0f, 0.0f, 0.0f));
        }
        pressed_buttons[MouseButton::RIGHT] = pressed;
        break;
    default:
        break;
    }
}
