#include "Player.h"

namespace game_of_life {
	const float Player::kSpeed = 1.f / 8.f;

	Player::Player(map_type& map) : map_(map), id_(0), position_() { }

	void Player::Update() {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			if (position_.x > 0 && clock_.getElapsedTime().asSeconds() > kSpeed) {
				position_ += sf::Vector2i(-1, 0);
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			if (position_.x + 1 < map_.GetWidth() && clock_.getElapsedTime().asSeconds() > kSpeed) {
				position_ += sf::Vector2i(1, 0);
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			if (position_.y > 0 && clock_.getElapsedTime().asSeconds() > kSpeed) {
				position_ += sf::Vector2i(0, -1);
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			if (position_.y + 1 < map_.GetHeight() && clock_.getElapsedTime().asSeconds() > kSpeed) {
				position_ += sf::Vector2i(0, 1);
			}
		}

		if (clock_.getElapsedTime().asSeconds() > kSpeed) {
			clock_.restart();
		}

		if (key_space_is_released_) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
				map_.Set(position_.y, position_.x, !map_.Get(position_.y, position_.x));
				key_space_is_released_ = false;
			}
		}
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			key_space_is_released_ = true;
		}
	}

	proto::Player Player::GetProto() const {
		proto::Player result;

		result.set_x(position_.x);
		result.set_y(position_.y);
		result.set_id(0);
		result.set_is_player(true);

		return result;
	}

	int Player::GetX() const {
		return position_.x;
	}

	int Player::GetY() const {
		return position_.y;
	}

	sf::Vector2i Player::GetPosition() const {
		return position_;
	}
}
