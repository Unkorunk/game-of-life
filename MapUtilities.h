#pragma once

#include <SFML/Window/Keyboard.hpp>
#include "Map.h"
#include "MapRenderer.h"

namespace game_of_life {
	template <typename T>
	class MapUtilities {
	public:
		MapUtilities(Map<T>& map, MapRenderer<T>& map_renderer) : map_(map), map_renderer_(map_renderer) { }
		virtual ~MapUtilities() = default;

		virtual void Update() {
			if (key_g_is_released_) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
					map_renderer_.SetGridVisible(!map_renderer_.GetGridVisible());
					key_g_is_released_ = false;
				}
			}
			if (!sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
				key_g_is_released_ = true;
			}

			if (key_c_is_released_) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
					map_.Clear();
					key_c_is_released_ = false;
				}
			}
			if (!sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
				key_c_is_released_ = true;
			}
		}

	private:
		bool key_g_is_released_ = true;
		bool key_c_is_released_ = true;

	protected:
		Map<T>& map_;
		MapRenderer<T>& map_renderer_;
	};
}
