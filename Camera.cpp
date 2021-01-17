#include "Camera.h"

namespace cellular_automaton {
    const float Camera::kSpeed = 300;
    const float Camera::kZoomSpeed = 1.001f;
    const float Camera::kZoomDefault = 0.45f;
    const float Camera::kZoomMax = 2.f;
    const float Camera::kZoomMin = 0.1f;

	Camera::Camera(sf::RenderWindow& window) : window_(window), view_(window.getView()), zoom_(kZoomDefault) {
		window_.setView(view_);
        view_.zoom(kZoomDefault);
	}

	void Camera::Update(float delta_time) {
        sf::Vector2f direction;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window_.close();
        }
        if (sf::Joystick::isButtonPressed(0, sf::Joystick::Z) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            direction += sf::Vector2f(-1.f, 0.f);
        }
        if (sf::Joystick::isButtonPressed(0, sf::Joystick::Y) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            direction += sf::Vector2f(1.f, 0.f);
        }
        if (sf::Joystick::isButtonPressed(0, sf::Joystick::R) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            direction += sf::Vector2f(0.f, -1.f);
        }
        if (sf::Joystick::isButtonPressed(0, sf::Joystick::X) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            direction += sf::Vector2f(0.f, 1.f);
        }

        direction = Normalize(direction);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) || sf::Joystick::isButtonPressed(0, sf::Joystick::U)) {
            if (zoom_ > kZoomMin) {
                view_.zoom(1 / kZoomSpeed);
                zoom_ /= kZoomSpeed;
            }
        }
		
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E) || sf::Joystick::isButtonPressed(0, sf::Joystick::V)) {
            if (zoom_ < kZoomMax) {
                view_.zoom(kZoomSpeed);
                zoom_ *= kZoomSpeed;
            }
        }

        view_.setCenter(view_.getCenter() + direction * delta_time * kSpeed * zoom_);
        window_.setView(view_);
	}

    sf::View& Camera::GetViewEditable() {
		return view_;
	}

    sf::Vector2f Camera::Normalize(const sf::Vector2f& vec) const {
        float length = vec.x * vec.x + vec.y * vec.y;
        if (length < 1e-5) return vec;
        return vec / sqrt(length);
	}
}
