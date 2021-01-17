#pragma once

#include "Map.h"

namespace cellular_automaton {
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
