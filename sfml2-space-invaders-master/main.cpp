#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <sstream>      // std::stringstream
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Texture2D.h"
#include "Player.h"
#include "Enemy.h"
#include <iomanip>
#include "Bullet.h"
#include <list>
#include "Explosion.h"
#include "SoundEffect.h"

float deltaTime;
bool enemyHit = false;
int score = 0;
int rowCount = 0;
int itemCount = 0;

bool gameover = false;
bool gamestart = false;
float timer = 0.0f;

std::list<Bullet> laser;
std::list<Bullet> beam;
std::list<Enemy> enemy;
std::list<Explosion> explo_fx;

void initEnemies(Enemy _enemy)
{
	// create enemies
	for (int i = 0; i < 40; i++)
	{
		enemy.push_back(_enemy);

		//std::cout << "row:" << rowCount << std::endl;
		//std::cout << "item:" << itemCount * 40 << std::endl;
	}

	for (auto e = enemy.begin(); e != enemy.end(); e++)
	{
		if (itemCount % 10 == 0)
		{
			itemCount = 0;
			rowCount++;
		}

		itemCount++;

		e->LoadResources();
		e->startPos = itemCount * 40;
		e->rowPosID = 40 * (11 - itemCount);
		int ran = (rand() % (20 - 3)) + 3; // MAX - MIN + MIN
		e->shootTimeLimit = ran;
		e->setPosition(sf::Vector2f(itemCount * 40.0f, 40.0f * rowCount));
	}
}

void reset(Enemy _enemy)
{
	gameover = false;
	score = 0;
	enemy.clear();
	beam.clear();
	laser.clear();
	explo_fx.clear();
	rowCount = 0;
	itemCount = 0;
	initEnemies(_enemy);
}

int main()
{
	std::cout << "SFML 2.5 Space Invaders Game" << std::endl;

	std::stringstream buffer; //score buffer
	sf::RenderWindow window(sf::VideoMode(640, 480), "sfml2-space-invaders");

	Texture2D fmg_logo;
	fmg_logo.Load("rd/fmg_splash.png");

	Texture2D background;
	background.Load("rd/space3.png");

	Texture2D gameover_ui;
	gameover_ui.Load("rd/gameover_ui.png");
	
	Texture2D win_ui;
	win_ui.Load("rd/win_ui.png");

	Player player;
	Bullet _laser;
	Bullet _beam;
	Enemy _enemy;
	Explosion _explo_fx;

	player.LoadResources();
	_laser.LoadResources();
	_beam.reversed = true;
	_beam.LoadResources();

	sf::Font font;
	if (!font.loadFromFile("rd/vermin_vibes_1989.ttf"))
	{
		// error...
	}

	sf::Text score_text;
	score_text.setFont(font); // font is a sf::Font
	score_text.setString("SCORE: 00000");
	score_text.setCharacterSize(24); // in pixels
	score_text.setFillColor(sf::Color::White);
	score_text.setPosition(640 - score_text.getGlobalBounds().width - 2, 2);

	sf::Text msg_text;
	msg_text.setFont(font); // font is a sf::Font
	//msg_text.setString("YOU WIN!");
	msg_text.setCharacterSize(32); // in pixels
	msg_text.setFillColor(sf::Color::White);

	SoundEffect snd_blaster("rd/blaster.ogg");
	SoundEffect snd_blasterEnemy("rd/pusher.ogg");
	SoundEffect snd_explo("rd/explode1.ogg");

	sf::Music music;
	if (!music.openFromFile("rd/bodenstaendig.ogg"))
		return -1; // error

	music.setLoop(true);
	music.play();

	initEnemies(_enemy);

	sf::Clock clock;
	while (window.isOpen())
	{
		sf::Time elapsed = clock.restart();
		deltaTime = elapsed.asSeconds();

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Space && !player.shoot && !player.dead)
				{
					player.shoot = true;
					_laser.setPosition(sf::Vector2f(player.getPosition().x + player.tex2d.sprite.getGlobalBounds().width / 2 - 1.5f, player.getPosition().y - 32));//1.5f is half of laser width ;)
					laser.push_back(_laser);
					snd_blaster.Play();
				}		
				
				if (event.key.code == sf::Keyboard::Escape)
				{
					window.close();
				}

				if (event.key.code == sf::Keyboard::Enter && gameover)
				{
					reset(_enemy);
					player.setPosition(sf::Vector2f(640 / 2 - player.tex2d.sprite.getLocalBounds().width / 2, player.getPosition().y));
					player.dead = false;
				}
			}

			if (event.type == sf::Event::KeyReleased)
			{
				if (event.key.code == sf::Keyboard::Space && player.shoot)
				{
					player.shoot = false;
				}
			}

			if (event.type == sf::Event::Closed)
				window.close();
		}

		timer += 1 * deltaTime;

		if (timer >= 2.0f)
			gamestart = true;

		//std::cout << timer << std::endl;

		if (gamestart) {

			player.Input(deltaTime);
			player.Update(deltaTime);

			for (auto it = enemy.begin(); it != enemy.end(); it++) {

				if (it->shoot)
				{
					_beam.reversed = true;
					_beam.setPosition(sf::Vector2f(it->getPosition().x + it->tex2d.sprite.getGlobalBounds().width * 0.5f - 4.5f, it->getPosition().y + 9));//1.5f is half of laser width ;)
					beam.push_back(_beam);
					snd_blasterEnemy.Play();
				}

				for (auto itb = laser.begin(); itb != laser.end(); itb++)
				{
					if (it->TileBoundingBox().intersects(itb->TileBoundingBox()))
					{
						// Debug.WriteLine("BOOM!");      
						score += 100;
						_explo_fx.LoadResources();
						_explo_fx.setPosition(sf::Vector2f(it->getPosition().x - 128 / 2, it->getPosition().y - 128 / 2));
						explo_fx.push_back(_explo_fx);
						laser.erase(itb);
						enemy.erase(it);
						snd_explo.Play();
						goto escapeFromHere;
					}
				}
			}

		escapeFromHere:

			for (auto it = enemy.begin(); it != enemy.end(); it++)
				it->Update(deltaTime);

			for (auto it = beam.begin(); it != beam.end(); it++)
			{
				if (it->TileBoundingBox().intersects(player.TileBoundingBox()) && !player.dead)
				{
					_explo_fx.LoadResources();
					_explo_fx.setPosition(sf::Vector2f(it->getPosition().x - 128 / 2, it->getPosition().y - 128 / 2));
					explo_fx.push_back(_explo_fx);
					player.dead = true;
					snd_explo.Play();
					beam.erase(it);
					break;
				}

				if (it->getPosition().y >= 480)
				{
					beam.erase(it);
					break;
				}
			}

			for (auto it = beam.begin(); it != beam.end(); it++)
				it->Update(deltaTime);

			for (auto it = laser.begin(); it != laser.end(); it++)
			{
				it->Update(deltaTime);

				if (it->getPosition().y <= 0)
				{
					laser.erase(it);
					break;
				}
			}

			for (auto it = explo_fx.begin(); it != explo_fx.end(); it++)
			{
				it->Update(deltaTime);
				if (it->animationCompleted)
				{
					explo_fx.erase(it);
					break;
				}
			}
		}

		//draw loop
		window.clear();

		if (gamestart) {

			window.draw(background.sprite);

			if (!player.dead)
				window.draw(player.tex2d.sprite);

			for (Bullet l : laser)
				window.draw(l.tex2d.sprite);

			for (Bullet b : beam)
				window.draw(b.tex2d.sprite);

			for (Enemy e : enemy)
				window.draw(e.tex2d.sprite);

			for (Explosion ex : explo_fx)
				window.draw(ex.tex2d.sprite);

			buffer << "SCORE:" << std::setw(4) << std::setfill('0') << score << std::endl;
			score_text.setString(buffer.str());
			window.draw(score_text);
			buffer.str(""); // set contents to that of an empty string

			if (enemy.empty())
			{
				gameover = true;
				window.draw(win_ui.sprite);
				//msg_text.setString("You Win!");
				//msg_text.setPosition(640 / 2 - 60, 480 / 2 - 32);
			}
			else if (player.dead)
			{
				gameover = true;
				window.draw(gameover_ui.sprite);
				//msg_text.setString("Game Over!");
				//msg_text.setPosition(640 / 2 - 80, 480 / 2 - 32);
			}

			window.draw(msg_text);
		}
		else
		{
			window.draw(fmg_logo.sprite);
		}

		window.display();
	}

	return 0;
}
