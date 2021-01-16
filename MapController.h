#pragma once

#include "Map.h"

namespace game_of_life {
	class MapController {
		using map_type = Map<bool>;
	public:
		explicit MapController(map_type& map);
		
		void Update();
	
	private:
		map_type& map_;
		map_type buff_map_;
		
	};
}
