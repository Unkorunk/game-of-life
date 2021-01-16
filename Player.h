#pragma once

#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "world.pb.h"

#include "Map.h"

namespace game_of_life {
	class Player {
		using map_type = Map<bool>;
	public:
		static const float kSpeed;
		
		Player(map_type& map);

		void Update();

		proto::Player GetProto() const;

		int GetX() const;
		int GetY() const;
		sf::Vector2i GetPosition() const;
	
	private:
		map_type& map_;
		
		int id_;
		sf::Vector2i position_;
		sf::Clock clock_;

		bool key_space_is_released_ = true;
	};
}