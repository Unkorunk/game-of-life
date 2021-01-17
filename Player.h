#pragma once

#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "world.pb.h"

#include "Map.h"

namespace cellular_automaton {
	class Player {
		using map_type = Map<bool>;
	public:
		static const float kSpeed;

		Player(map_type& map, bool is_player);
		Player(const Player& other);
		Player(Player&& other) noexcept;

		Player& operator=(const Player& other);
		Player& operator=(Player&& other) noexcept;
		
		void Update();

		generated_files::Player GetProto() const;

		unsigned int GetId() const;

		int GetX() const;
		int GetY() const;
		sf::Vector2i GetPosition() const;

		void UpdateProto(const generated_files::Player& player);

		bool IsPlayer() const;

		void SetListener(std::function<void(const Player&)> listener);

	private:
		map_type& map_;

		unsigned int id_;
		sf::Vector2i position_;
		sf::Clock clock_;

		bool is_player_;

		bool key_space_is_released_ = true;

		std::function<void(const Player&)> listener_;
	};
}
