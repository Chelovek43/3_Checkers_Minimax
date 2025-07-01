#ifndef RUNNER_H
#define RUNNER_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include "Enemy.h"

class Runner
{
public:
    Runner();
    void run();

private:
    static const int gameWidth;
    static const int gameHeight;
    static const int numFrames;
    static const float frameDuration;
    static const float gravity;          // ������� ����������
    static const float jumpSpeed;        // �������� ������

    sf::RenderWindow window;
    std::vector<sf::Texture> texturesRunner;
    sf::Texture textureWalking; // �������� �������� ������
    sf::Texture textureBackground;
    sf::Texture textureGrass;
    sf::Texture textureEnemy;
    sf::Texture textureThorns;
    sf::Texture textureResetButton; // �������� ������ �����������
    sf::Sprite spriteBackground;
    sf::Sprite spriteRunner;
    sf::Sprite spriteResetButton; // ������ ������ �����������
    std::vector<sf::Sprite> grassSprites;
    sf::Font font; // ����� ��� ��������� ���������
	sf::Font fontForNums; // ����� ��� ����������� �����
    sf::Text startText; // ����� ��� ������ ����
    sf::Text gameOverText; // ����� ��� ��������� ����
    sf::Text scoreText; // ����� ��� ����������� ������ ��� �����
    sf::Text scoreNums; // ����� ��� ����������� ����� �����
    sf::Clock gameClock; // ���� ��� ������������ ������� ����
    sf::Clock clock;
    sf::Clock animationClock;
    bool isPlaying;
    bool isGameOver; // ���� ��� ��������� ����
    bool isWalking;
    bool isJumping;                      // ���� ��� ������
    bool isCrouching;
    float currentFrame;
    float verticalVelocity;              // ������������ ��������
    int score;
    sf::Vector2f runnerMovement;
    sf::IntRect walkingFrames[4];

    std::vector<std::unique_ptr<Enemy>> enemies; // ������ ��� ���� ������

    void processEvents();
    void update(sf::Time deltaTime);
    void render();
    void reset();
    void updateAnimation(sf::Time deltaTime);
    void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);
    void loadTextures();
    void updateGrass(sf::Time deltaTime);
    void updateEnemies(sf::Time deltaTime);
    void spawnGrass();
    void spawnEnemy();
    bool checkCollisions();
    
    void showGameOverScreen(); // ������� ��� ����������� ������ ��������� ����
    bool isMouseOverButton(const sf::Sprite& button); // ������� ��� �������� ���������� ���� ��� �������
};

#endif // RUNNER_H
