#include "Player.h"

#include <utility>

namespace cellular_automaton {
	const float Player::kSpeed = 1.f / 8.f;

	Player::Player(map_type& map, bool is_player)
		: map_(map), id_(0), position_(), is_player_(is_player), listener_([](const Player&) {}) { }

	Player::Player(const Player& other)
		: map_(other.map_), id_(other.id_), position_(other.position_), is_player_(other.is_player_), listener_(other.listener_) {}

	Player::Player(Player&& other) noexcept
		: map_(other.map_), id_(std::move(other.id_)),
		  position_(std::move(other.position_)), is_player_(std::move(is_player_)),
		  listener_(std::move(other.listener_)) {}

	Player& Player::operator=(const Player& other) {
		map_ = other.map_;
		id_ = other.id_;
		position_ = other.position_;
		is_player_ = other.is_player_;
		listener_ = other.listener_;

		return *this;
	}

	Player& Player::operator=(Player&& other) noexcept {
		map_ = other.map_;
		id_ = std::move(other.id_);
		position_ = std::move(other.position_);
		is_player_ = std::move(is_player_);
		listener_ = std::move(other.listener_);

		return *this;
	}

	void Player::Update() {
		const sf::Vector2i old_position = position_;

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

		if (position_ != old_position) {
			listener_(*this);
		}
	}

	generated_files::Player Player::GetProto() const {
		generated_files::Player result;

		result.set_x(position_.x);
		result.set_y(position_.y);
		result.set_id(0);
		result.set_is_player(is_player_);

		return result;
	}

	unsigned int Player::GetId() const {
		return id_;
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

	void Player::UpdateProto(const generated_files::Player& player) {
		if (!player.has_id() || !player.has_is_player() || player.id() != id_ || player.is_player() != is_player_) {
			throw std::exception();
		}

		if (player.has_x()) {
			position_.x = player.x();
		}
		if (player.has_y()) {
			position_.y = player.y();
		}
	}

	bool Player::IsPlayer() const {
		return is_player_;
	}

	void Player::SetListener(std::function<void(const Player&)> listener) {
		listener_ = std::move(listener);
	}
}
