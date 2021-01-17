#pragma once

#include "Map.h"

namespace game_of_life {
	template <typename T>
	class MapController {
	public:
		explicit MapController(Map<T>& map) : map_(map) {}
		virtual ~MapController() = default;

		virtual void Update() = 0;

	protected:
		Map<T>& map_;

	};
}
