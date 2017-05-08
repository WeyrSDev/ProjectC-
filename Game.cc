#include "Game.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Globals.h"



void Game::setWalls() {
	b2BodyDef bodyDef;
	bodyDef.position.Set(0, 0);
	m_groundBody = m_world->CreateBody( &bodyDef );

	b2PolygonShape polygonShape;
	b2FixtureDef fixtureDef;
  	fixtureDef.shape = &polygonShape;
   	fixtureDef.isSensor = false;

	polygonShape.SetAsBox( 5.0f/RATIO, 150.0f/RATIO, b2Vec2(130.0f/RATIO,400.0f/RATIO), 0 );
	m_groundBody->CreateFixture(&fixtureDef);

	polygonShape.SetAsBox( 2000/RATIO, 10.0f/RATIO, b2Vec2(350.0f/RATIO,600.0f/RATIO), 0 );
	m_groundBody->CreateFixture(&fixtureDef);
}

Game::Game(sf::RenderWindow* window): m_window(window) {
	m_world = new b2World(b2Vec2(0,0));
	m_world->SetGravity( b2Vec2(0,0) );

	player.setCar(m_world);
	
	m_tBackGround.loadFromFile("./Images/background.png");
	m_tBackGround.setSmooth(true);
	m_sBackGround.setTexture(m_tBackGround);
	m_sBackGround.scale(1,1);
	m_controlState = 0;
	

	//setWalls();
}

b2World* Game::getWorld() {
	return m_world;
}

Player Game::getPlayer() {
	return player; 
}

void Game::step() {
	//m_window->draw(m_sBackGround);
	Car* Car = player.getCar();
	
	sf::Sprite sCar = Car->getSprite();
	sCar.setOrigin(CAR_WIDTH/2, CAR_HEIGHT/2);
	
	b2Vec2 vec = Car->getBody()->GetPosition();
	float angle = Car->getBody()->GetAngle();
	//std::cout << vec.x * RATIO << std::endl;

	

	sCar.setRotation(angle*RADTODEG);
	sCar.setPosition(vec.x*RATIO, vec.y*RATIO);
	m_window->draw(sCar);
	std::vector<Tire*> tires = Car->getTires();
	for(int i = 0; i < tires.size(); i++) {
		sf::Sprite sTire = tires[i]->getSprite();
		sTire.setOrigin(TIRE_WIDTH/2, TIRE_HEIGHT/2);
		vec = tires[i]->getBody()->GetPosition();
		angle = tires[i]->getBody()->GetAngle();
		sTire.setRotation(angle*RADTODEG);
		sTire.setPosition(vec.x*RATIO,vec.y*RATIO);
		m_window->draw(sTire);
	}


}


void Game::Keyboard(const char key) {
    switch (key) {
        case 'A' : m_controlState |= LEFT; break;
        case 'D' : m_controlState |= RIGHT; break;
        case 'W' : m_controlState |= UP; break;
        case 'S' : m_controlState |= DOWN; break;
    }
    Car* Car = player.getCar();
    Car->update(m_controlState);
    std::cout << m_controlState << std::endl;
}

void Game::KeyboardUp(const char key) {
    switch (key) {
        case 'A' : m_controlState &= ~LEFT; break;
        case 'D' : m_controlState &= ~RIGHT; break;
        case 'W' : m_controlState &= ~UP; break;
        case 'S' : m_controlState &= ~DOWN; break;
    }
    Car* Car = player.getCar();
    Car->update(m_controlState);
    std::cout << m_controlState << std::endl;
}