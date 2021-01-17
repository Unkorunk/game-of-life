#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Joystick.hpp>

namespace cellular_automaton {
	class Camera {
	public:
		static const float kSpeed;
		static const float kZoomSpeed;
		static const float kZoomDefault;
		static const float kZoomMax;
		static const float kZoomMin;
		
		explicit Camera(sf::RenderWindow& window);

		void Update(float delta_time);

		sf::View& GetViewEditable();

	private:
		sf::RenderWindow& window_;
		sf::View view_;

		float zoom_;

		sf::Vector2f Normalize(const sf::Vector2f& vec) const;
		
	};
}
