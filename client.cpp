#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <boost/asio.hpp>

#include "world.pb.h"

#include "Map.h"
#include "MapController.h"
#include "MapRenderer.h"
#include "MapUtilities.h"
#include "Camera.h"
#include "Player.h"
#include "PlayerCollection.h"

using boost::asio::ip::tcp;

const size_t width = 100;
const size_t height = 100;
const float fps = 1.f/8.f;
const char* default_port = "5000";
const char* default_host = "localhost";

using namespace std;

std::string buff(1024,' ');

void do_read(tcp::socket& socket, cellular_automaton::PlayerCollection& player_collection) {
	socket.async_read_some(
		boost::asio::buffer(buff),
		[&](boost::system::error_code ec, std::size_t length) {
			if (!ec) {
				generated_files::World world;
				world.ParseFromArray(buff.data(), length);

				player_collection.UpdateProto(world);
				
				do_read(socket, player_collection);
			} else {
				exit(0);
			}
		}
	);
}

class GameOfLifeMapController final : cellular_automaton::MapController<bool> {
public:
	explicit GameOfLifeMapController(cellular_automaton::Map<bool>& map)
		: MapController<bool>(map), buff_map_(map.GetWidth(), map.GetHeight()) {}

	void Update() override {
		for (size_t i = 0; i < map_.GetHeight(); i++) {
			for (size_t j = 0; j < map_.GetWidth(); j++) {
				size_t count = map_.MoorNeighborhood(i, j, 1, [](const bool& it) {
					return it;
				});

				buff_map_.Set(i, j, map_.Get(i, j));

				if (map_.Get(i, j)) {
					if (count < 2 || count > 3) {
						buff_map_.Set(i, j, false);
					}
				} else {
					if (count == 3) {
						buff_map_.Set(i, j, true);
					}
				}
			}
		}

		map_.Swap(buff_map_);
	}

private:
    cellular_automaton::Map<bool> buff_map_;

};

class GameOfLifeMapUtilities final : cellular_automaton::MapUtilities<bool> {
public:
	GameOfLifeMapUtilities(cellular_automaton::Map<bool>& map, cellular_automaton::MapRenderer<bool>& map_renderer)
		: MapUtilities<bool>(map, map_renderer), dist_(0, 1) {}

	void Update() override {
		MapUtilities<bool>::Update();

		if (key_r_is_released_) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
				map_.Clear();
				for (size_t i = 0; i < map_.GetHeight(); i++) {
					for (size_t j = 0; j < map_.GetWidth(); j++) {
						map_.Set(i, j, dist_(rd_));
					}
				}
				key_r_is_released_ = false;
			}
		}
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
			key_r_is_released_ = true;
		}
	}

private:
	bool key_r_is_released_ = true;
    random_device rd_;
    uniform_int_distribution<size_t> dist_;

};

int main()
{
    cellular_automaton::Map<bool> map(width, height);
	GameOfLifeMapController map_controller(map);

    boost::asio::io_context io_context;

    tcp::socket s(io_context);
    tcp::resolver resolver(io_context);
    boost::asio::connect(s, resolver.resolve(default_host, default_port));

    cellular_automaton::PlayerCollection player_collection;
	
    do_read(s, player_collection);
    
    sf::RenderWindow window(sf::VideoMode::getFullscreenModes()[0], "Game Of Life");

    cellular_automaton::MapRenderer<bool> map_renderer(window, map, player_collection);
	map_renderer.MapValueToExistence([](const bool& it) {
		return it;
	});
	map_renderer.MapValueToColor([](const bool& it) {
		return sf::Color(255, 255, 255);
	});
	
    cellular_automaton::Camera camera(window);

	auto cell_size = cellular_automaton::MapRenderer<bool>::kCellSize;
	auto cell_offset = cellular_automaton::MapRenderer<bool>::kCellOffset;
	
    camera.GetViewEditable().setCenter(sf::Vector2f(width * (cell_size.x + cell_offset.x) + cell_offset.x, height * (cell_size.y + cell_offset.y) + cell_offset.y) / 2.f);

    sf::Clock clock;
    sf::Clock fps_clock;
    
    cellular_automaton::Player player(map, true);
	player.SetListener([&s](const cellular_automaton::Player& player) {
        boost::asio::write(s, boost::asio::buffer(player.GetProto().SerializeAsString()));
	});
	player_collection.Add(player);

    GameOfLifeMapUtilities map_utilities(map, map_renderer);
	
    bool game_paused = true;
    bool key_p_is_released = true;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        
        io_context.poll();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

    	camera.Update(dt);
    	player_collection.Update();

        if (key_p_is_released) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
                game_paused = !game_paused;
                key_p_is_released = false;
            }
        }
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
            key_p_is_released = true;
        }

        map_utilities.Update();
        map_renderer.Update();

        if (fps_clock.getElapsedTime().asSeconds() < fps) {
            window.display();
            continue;
        }
        fps_clock.restart();

        if (!game_paused) {
        	map_controller.Update();
        }

        window.display();
    }

    return 0;
}