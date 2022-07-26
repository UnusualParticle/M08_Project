/*
* Program name: M08_Project.cpp
* Author: Donovan Blake
* Date last updated: today
* Purpose: Play the board game Sorry!
*/

// Using the SFML 2.5.1 Library for graphics
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <chrono>
#include <iostream>
#include <random>
#include <string>

// Settings
namespace Settings
{
	// Sources:
	//	Arrow PNG: <a href="https://www.flaticon.com/free-icons/arrow" title="arrow icons">Arrow icons created by Uniconlabs - Flaticon</a>, 'image: Flaticon.com'. This cover has been designed using resources from Flaticon.com
	//  Player PNG: <a target="_blank" href="https://icons8.com/icon/8uSoJLfHWY3D/pawn">Pawn</a> icon by <a target="_blank" href="https://icons8.com">Icons8</a>
	

	const sf::Vector2f windowSize{ 900, 900 };
	const sf::Vector2f gridSize{ 90,90 };
	const size_t PLAYERS{ 4 };
	const size_t DICESIDES{ 6 };
	
	sf::Font font{};
	sf::Texture boardTexture{};
	sf::Texture playerTexture[PLAYERS]{};
	sf::Texture diceTexture[DICESIDES]{};

	// Load fonts and textures
	void initialize()
	{
		font.loadFromFile("Assets/arial.ttf");
		boardTexture.loadFromFile("Assets/board.png");
		for (size_t i{}; i < PLAYERS; ++i)
			playerTexture[i].loadFromFile("Assets/player" + std::to_string(i) + ".png");
		for (size_t i{}; i < DICESIDES; ++i)
			diceTexture[i].loadFromFile("Assets/dice" + std::to_string(i) + ".png");
	}
}

// Generates a random number low <= x <= high
int randint(int low, int high)
{
	static std::mt19937 mt{ std::random_device{}() };
	std::uniform_int_distribution distr{ low, high };
	return distr(mt);
}

// A timer that runs in milliseconds
class Timer_t
{
private:
	static std::chrono::steady_clock m_clock;
	std::chrono::time_point<std::chrono::steady_clock> m_set{};
	size_t m_length{};
public:
	Timer_t() = default;
	Timer_t(size_t ms)
		: m_length(ms), m_set(m_clock.now() + std::chrono::milliseconds(ms))
	{}
	~Timer_t() = default;

	void start()
	{
		m_set = m_clock.now() + std::chrono::milliseconds(m_length);
	}
	void start(size_t ms)
	{
		setLength(ms);
		m_set = m_clock.now() + std::chrono::milliseconds(m_length);
	}
	void setLength(size_t ms)
	{
		m_length = ms;
	}

	bool done() const
	{
		return (m_clock.now() >= m_set);
	}
	size_t length() const
	{
		return m_length;
	}
	size_t current() const
	{
		return static_cast<size_t>(std::chrono::milliseconds((m_clock.now() - m_set).count()).count());
	}
};
std::chrono::steady_clock Timer_t::m_clock{};

// Player object. Stores position
class Player_t : public sf::Drawable
{
private:
	static int idCounter;
	static size_t MOVETIME;

	int m_playerID{};
	sf::Sprite m_sprite{};
	int m_buffer{};
	int m_pos{-1};
	Timer_t timer{MOVETIME};
public:
	Player_t() :
		m_playerID(idCounter++)
	{
		m_sprite.setTexture(Settings::playerTexture[m_playerID]);
		setPosition();
	};

	// Draw function
	void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(m_sprite, states);
	}

	// Reset player ID for a new game
	static void resetcounter()
	{
		idCounter = 0;
	}

	// Set player position
	void setPosition()
	{
		// I don't want to type everything out
		const auto& window{ Settings::windowSize };
		const auto& grid{ Settings::gridSize };

		// Handle each side of the board
		sf::Vector2f pos{};
		if (m_pos == -1)
			pos = {270.f + m_playerID * grid.x, 225.f};
		else if (m_pos < 10)		// left
			pos= { 0,window.y - (1 + m_pos) * grid.y };
		else if (m_pos < 19)	// top
			pos = { grid.x * (m_pos - 9), 0 };
		else if (m_pos < 28)	// right
			pos = { window.x - grid.x, (m_pos - 18) * grid.y };
		else if (m_pos < 36)	// bottom
			pos = { window.x - (m_pos - 26) * grid.x, window.y - grid.y };
		else if (m_pos < 44)	// left 2
			pos = { grid.x, window.y - (m_pos - 34) * grid.y };
		else if (m_pos < 51)	// top 2
			pos = { grid.x * (m_pos - 42), grid.y };
		else if (m_pos < 58)	// right 2
			pos = { window.x - 2 * grid.x, (m_pos - 49) * grid.y };
		else if (m_pos == 58)	// pre-win
			pos = { 630.f, 720.f };
		else if (m_pos == 59)	// win
			pos = { 495.f, 675.f };
		else
			throw std::exception{ "Not a valid position" };

		m_sprite.setPosition(pos);
	}

	// Move the player
	void move(int m, bool direct = false)
	{
		if (!direct)
			m_buffer = m;
		else
			m_pos = m;

		// Set position is down here in case m_pos is > 59
		if (direct)
			setPosition();
	}

	// Swap positions with another player
	void swap(Player_t& p2)
	{
		// Check for same player
		if (p2.m_playerID == m_playerID)
			return;

		// Swap positions
		int temp{ p2.getPosition() };
		p2.move(m_pos, true);
		move(temp, true);
	}

	// Check if the player is moving yet
	void poll()
	{
		if (m_buffer && timer.done())
		{
			if (m_buffer > 0)
			{
				++m_pos;
				--m_buffer;
			}
			else
			{
				--m_pos;
				++m_buffer;
			}
			if (m_pos > 59)
			{
				m_pos = 59;
				m_buffer = 0;
			}
			else if (m_pos < 0)
			{
				m_pos = 0;
			}
			timer.start();
			setPosition();
		}
	}

	// Get the player's position
	int getPosition() const
	{
		return m_pos;
	}

	// Check if the player is done moving
	bool done()
	{
		return !m_buffer;
	}

	// Get the player ID
	size_t getID() const
	{
		return m_playerID;
	}
};
int Player_t::idCounter{};
size_t Player_t::MOVETIME{500};

// Dice object that can be rolled
class Dice_t : public sf::Drawable
{
private:
	static int ROLLCOUNT;
	static size_t ROLLTIME;

	int m_rollcount{ -1 };
	Timer_t m_rolltime{ ROLLTIME };
	int m_current{};
	sf::Sprite m_sprite{};
public:
	Dice_t()
	{
		int num{ randint(0,5) };
		m_sprite.setTexture(Settings::diceTexture[num]);
		m_current = num + 1;
	}

	// Draw function
	void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(m_sprite, states);
	}

	// Set the position of the sprite
	void setPosition(const sf::Vector2f& pos)
	{
		m_sprite.setPosition(pos);
	}

	// Check the state of the dice
	void poll()
	{
		// Currently rolling
		if (m_rollcount >= 0 && m_rolltime.done())
		{
			// Generate number and edit sprite
			int num{ randint(0,5) };
			m_sprite.setTexture(Settings::diceTexture[num]);
			m_current = num + 1;

			// Continue rolling
			--m_rollcount;
			m_rolltime.start();
		}
	}

	// Begin rolling
	void roll()
	{
		m_rollcount = ROLLCOUNT;
		m_rolltime.start();
	}

	// Check if the die is done rolling
	bool done() const
	{
		return m_rollcount < 0;
	}

	// Check value
	int value() const
	{
		if (done())
			return m_current;
		else
			return 0;
	}
};
int Dice_t::ROLLCOUNT{ 12 };
size_t Dice_t::ROLLTIME{ 200 };

// Button that can be pressed with the mouse
class Button_t : public sf::Drawable
{
private:
	static float PADDING;
	static sf::Color FILL;
	static sf::Color FILLHOVER;
	static sf::Color LINECOLOR;
	static float LINESIZE;
	static sf::Color TEXTCOLOR;

	sf::RectangleShape m_rect{};
	sf::Text m_text{};
	bool m_hover{};
public:
	Button_t() = default;
	Button_t(const sf::Vector2f& pos, const sf::Vector2f& size, const std::string& str)
		: m_text(str, Settings::font, 50u), m_rect(size)
	{
		m_rect.setPosition(pos);
		m_rect.setFillColor(FILL);
		m_rect.setOutlineColor(LINECOLOR);
		m_rect.setOutlineThickness(LINESIZE);

		m_text.setPosition(pos + sf::Vector2f{PADDING, PADDING});
		m_text.setFillColor(TEXTCOLOR);
	}

	void poll(sf::Vector2f mouse)
	{
		if (m_rect.getGlobalBounds().contains(mouse))
		{
			m_hover = true;
			m_rect.setFillColor(FILLHOVER);
		}
		else
		{
			m_hover = false;
			m_rect.setFillColor(FILL);
		}
	}

	bool isHovered() const { return m_hover; }

	void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(m_rect, states);
		target.draw(m_text, states);
	}
};
float Button_t::PADDING{ 10.f };
sf::Color Button_t::FILL{ sf::Color::Black };
sf::Color Button_t::FILLHOVER{ 100,100,100 };
sf::Color Button_t::LINECOLOR{ sf::Color::White };
float Button_t::LINESIZE{ -5.f };
sf::Color Button_t::TEXTCOLOR(sf::Color::White);

// Ask for number of players
size_t promptPlayers(sf::RenderWindow& window)
{
	sf::Text prompt{ "How many players will there be?", Settings::font };
	prompt.setPosition({ 180.f,300.f });
	prompt.setFillColor(sf::Color::White);

	sf::Vector2f SIZE{ 200.f,74.f };
	Button_t two{ {50.f, 450.f}, SIZE, "Two" };
	Button_t three{ {350.f, 450.f}, SIZE, "Three" };
	Button_t four{ {650.f, 450.f}, SIZE, "Four" };

	size_t players{};
	while (players == 0)
	{
		sf::Event event{};
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::MouseMoved)
			{
				sf::Vector2f mouse{ (float)event.mouseMove.x, (float)event.mouseMove.y };
				two.poll(mouse);
				three.poll(mouse);
				four.poll(mouse);
			}
			else if (event.type == sf::Event::MouseButtonPressed)
			{
				if (two.isHovered())
					players = 2;
				else if (three.isHovered())
					players = 3;
				else if (four.isHovered())
					players = 4;
			}
		}


		window.clear();
		window.draw(prompt);
		window.draw(two);
		window.draw(three);
		window.draw(four);
		window.display();
	}

	return players;
}

// Ask to play again
bool promptAgain(sf::RenderWindow& window)
{
	Button_t playagain{ {360.f,490.f},{180,74}, "Play again?" };
	bool exit{};
	while (!exit)
	{
		sf::Event event{};
		while (window.pollEvent(event))
		{
			// Close the window
			if (event.type == sf::Event::Closed)
				exit = true;

			if (event.type == sf::Event::MouseMoved)
				playagain.poll({ (float)event.mouseMove.x, (float)event.mouseMove.y });
			else if (event.type == sf::Event::MouseButtonPressed)
			{
				exit = playagain.isHovered();
			}
		}

		window.clear();
		window.draw(playagain);
		window.display();
	}

	return exit;
}

// Game states
enum class GameState
{
	Ready,
	Rolling,
	Moving,
	Done
};

int main()
{
	sf::RenderWindow window{ {(size_t)Settings::windowSize.x, (size_t)Settings::windowSize.y},"Sorry!"};
	Settings::initialize();

	sf::Sprite board{ Settings::boardTexture };
	Button_t roll{ {225,720},{180,74},"Roll" };

	Dice_t d0{};
	Dice_t d1{};
	d0.setPosition({ 203.f, 585.f });
	d1.setPosition({ 337.f, 585.f });

	sf::Text information{ "Player 1 turn, Roll a double to start!", Settings::font, 20u };
	sf::Text result{"",Settings::font, 20u};
	information.setPosition({203.f, 400.f});
	result.setPosition({203.f,530.f});

	bool exit{};
	while (!exit)
	{
		// Setup the game by asking for the number of players and resetting the board
		Player_t::resetcounter();
		size_t playerCount{ promptPlayers(window) };
		std::vector<Player_t> players{};
		players.resize(playerCount);
		int diceRolls[10]{};


		// Play the game
		GameState state{};
		size_t playerTurn{};
		bool rollAgain{};
		while (state != GameState::Done && !exit)
		{
			sf::Event event{};
			while (window.pollEvent(event))
			{
				// Close the window
				if (event.type == sf::Event::Closed)
					exit = true;
				
				// Check if the player has rolled the dice
				if (state == GameState::Ready)
				{
					if (event.type == sf::Event::MouseMoved)
						roll.poll({ (float)event.mouseMove.x, (float)event.mouseMove.y });
					else if (event.type == sf::Event::MouseButtonPressed)
					{
						if (roll.isHovered())
						{
							if (rollAgain)
								rollAgain = false;
							d0.roll();
							d1.roll();
							state = GameState::Rolling;
							result.setString("Rolling the dice...");
						}
					}
				}
			}
			// Check if the dice are done rolling
			if (state == GameState::Rolling)
			{
				d0.poll();
				d1.poll();
				if (d0.done() && d1.done())
				{
					// Simplify code, thanks
					auto& p{ players[playerTurn] };

					// Check for double
					bool specialdouble{};
					if (true || d0.value() == d1.value())
					{
						// Rolled a double already
						if (rollAgain)
						{
							p.move(-1, true);
							rollAgain = false;
							specialdouble = true;
							information.setString("Rolled two doubles in a row! Start over!");
						}
						// Starting
						else if (p.getPosition() < 0)
						{
							p.move(0, true);
							specialdouble = true;
							information.setString("Rolled a double. Now you can start moving!");
						}
						else
							rollAgain = true;
					}
					else
						rollAgain = false;

					// Check the roll total
					int sum{ d0.value() + d1.value() };
					++diceRolls[sum - 2];
					if (!specialdouble && p.getPosition() >= 0)
					{
						size_t id{};
						switch (sum)
						{
						case 2:
							result.setString("Rolled a 2\nMove forward 2 spaces");
							p.move(2);
							break;
						case 3:
							result.setString("Rolled a 3\nMove forward 3 spaces");
							p.move(3);
							break;
						case 4:
							result.setString("Rolled a 4\nMove backward 1 space");
							p.move(-1);
							break;
						case 5:
							result.setString("Rolled a 5\nMove backward 3 spaces");
							p.move(-3);
							break;
						case 6:
							result.setString("Rolled a 6\nMove forward 6 spaces");
							p.move(6);
							break;
						case 7:
							result.setString("Rolled a 7\nSwap places with the player at the back");
							id = p.getID();
							for (auto& i : players)
							{
								if (i.getPosition() < players[id].getPosition())
									id = i.getID();
							}
							if (id == p.getID())
								information.setString("You are already in the back!");
							else
							{
								p.swap(players[id]);
								information.setString("Swapped places with player " + std::to_string(id+1));
							}
							break;
						case 8:
							result.setString("Rolled an 8\nDo nothing");
							break;
						case 9:
							result.setString("Rolled a 9\nMove forward 9 spaces");
							p.move(9);
							break;
						case 10:
							result.setString("Rolled a 10\nMove forward 10 spaces");
							p.move(10);
							break;
						case 11:
							result.setString("Rolled an 11\nSwap places with the front player");
							id = p.getID();
							for (auto& i : players)
							{
								if (i.getPosition() > players[id].getPosition())
									id = i.getID();
							}
							if (id == p.getID())
								information.setString("You are already in the front!");
							else
							{
								p.swap(players[id]);
								information.setString("Swapped places with player " + std::to_string(id+1));
							}
							break;
						case 12:
							result.setString("Rolled a 12\nRestart from the begining");
							p.move(-1, true);
						}
					}
					state = GameState::Moving;
				}
			}
			// Check if the player is done moving
			else if (state == GameState::Moving)
			{
				// Simplify code, thanks
				auto& p{ players[playerTurn] };

				// Move the player
				p.poll();
				if (p.done())
				{

					// Did the player land on another player?
					for (auto& i : players)
					{
						if (i.getID() == playerTurn)
							continue;
						if (p.getPosition() == i.getPosition())
						{
							// Move the player back to the beginning
							i.move(-1, true);
							break;
						}
					}

					if (p.getPosition() != 59 && !rollAgain)
					{
						state = GameState::Ready;
						++playerTurn;
						if (playerTurn >= playerCount)
							playerTurn = 0;
						if (players[playerTurn].getPosition() < 0)
							information.setString("Player " + std::to_string(playerTurn + 1) + " turn. Roll a double to start!");
						else
							information.setString("Player " + std::to_string(playerTurn + 1) + " turn");
					}
					else if (!rollAgain)
					{
						state = GameState::Done;
						std::string infstr{ "Player " + std::to_string(playerTurn + 1) + " Won!  Dice rolled:" };
						for (size_t i{}; i < 10; ++i)
						{
							if (i % 5 == 0)
								infstr += '\n';
							infstr += std::to_string(i + 2) + ": " + std::to_string(diceRolls[i]) + " - ";
						}
					}
					else
					{
						state = GameState::Ready;
						information.setString("You rolled a double. Roll again!");
					}
				}
			}

			// Draw everything
			window.clear();
			window.draw(board);
			window.draw(information);
			window.draw(result);
			window.draw(d0);
			window.draw(d1);
			if (state == GameState::Ready)
				window.draw(roll);
			for (auto& p : players)
				window.draw(p);
			window.display();
		}
		
		// Ask the player to play again
		if (!exit)
			exit = promptAgain(window);
	}
	window.close();

	return 0;
}