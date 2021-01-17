#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <functional>

#include "Map.h"

namespace cellular_automaton {
	template <typename T>
	class MapRenderer {
	public:
		static const sf::Vector2f kCellOffset;
		static const sf::Vector2f kCellSize;
		static const sf::Color kBackgroundColor;
		static const sf::Color kGridColor;

		MapRenderer(sf::RenderWindow& window, const Map<T>& map) : window_(window), map_(map), grid_visible_(true) { }

		void Update() const {
			window_.clear(kBackgroundColor);

			sf::RectangleShape cell_rect(kCellSize);
			sf::RectangleShape v_rect(sf::Vector2f(kCellOffset.x, map_.GetHeight() * (kCellSize.y + kCellOffset.y) + kCellOffset.y));
			v_rect.setFillColor(kGridColor);
			sf::RectangleShape h_rect(sf::Vector2f(map_.GetWidth() * (kCellSize.x + kCellOffset.x) + kCellOffset.x, kCellOffset.y));
			h_rect.setFillColor(kGridColor);

			if (grid_visible_) {
				for (size_t i = 0; i < map_.GetWidth() + 1; i++) {
					v_rect.setPosition(i * (kCellSize.x + kCellOffset.x), 0.f);
					window_.draw(v_rect);
				}

				for (size_t i = 0; i < map_.GetHeight() + 1; i++) {
					h_rect.setPosition(0.f, i * (kCellSize.y + kCellOffset.y));
					window_.draw(h_rect);
				}
			}

			for (size_t i = 0; i < map_.GetHeight(); i++) {
				for (size_t j = 0; j < map_.GetWidth(); j++) {
					if (existence_mapper_(map_.Get(i, j))) {
						cell_rect.setFillColor(color_mapper_(map_.Get(i, j)));
						cell_rect.setPosition(j * (kCellSize.x + kCellOffset.x) + kCellOffset.x, i * (kCellSize.y + kCellOffset.y) + kCellOffset.y);
						window_.draw(cell_rect);
					}
				}
			}
		}

		void SetGridVisible(bool visible) {
			grid_visible_ = visible;
		}

		bool GetGridVisible() const {
			return grid_visible_;
		}

		void MapValueToExistence(const std::function<bool(const T&)>& mapper) {
			existence_mapper_ = mapper;
		}

		void MapValueToColor(const std::function<sf::Color(const T&)>& mapper) {
			color_mapper_ = mapper;
		}

	private:
		sf::RenderWindow& window_;
		const Map<T>& map_;

		bool grid_visible_;

		std::function<bool(const T&)> existence_mapper_;
		std::function<sf::Color(const T&)> color_mapper_;
	};

	template <typename T>
	const sf::Vector2f MapRenderer<T>::kCellOffset(0.5f, 0.5f);

	template <typename T>
	const sf::Vector2f MapRenderer<T>::kCellSize(4.f, 4.f);

	template <typename T>
	const sf::Color MapRenderer<T>::kBackgroundColor(50, 50, 50);

	template <typename T>
	const sf::Color MapRenderer<T>::kGridColor(150, 150, 150);
}
