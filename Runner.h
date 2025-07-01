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
    static const float gravity;          // Добавим гравитацию
    static const float jumpSpeed;        // Скорость прыжка

    sf::RenderWindow window;
    std::vector<sf::Texture> texturesRunner;
    sf::Texture textureWalking; // Текстура анимации ходьбы
    sf::Texture textureBackground;
    sf::Texture textureGrass;
    sf::Texture textureEnemy;
    sf::Texture textureThorns;
    sf::Texture textureResetButton; // Текстура кнопки перезапуска
    sf::Sprite spriteBackground;
    sf::Sprite spriteRunner;
    sf::Sprite spriteResetButton; // Спрайт кнопки перезапуска
    std::vector<sf::Sprite> grassSprites;
    sf::Font font; // Шрифт для текстовых подсказок
	sf::Font fontForNums; // Шрифт для отображения чисел
    sf::Text startText; // Текст для начала игры
    sf::Text gameOverText; // Текст для окончания игры
    sf::Text scoreText; // Текст для отображения текста для очков
    sf::Text scoreNums; // Текст для отображения числа очков
    sf::Clock gameClock; // Часы для отслеживания времени игры
    sf::Clock clock;
    sf::Clock animationClock;
    bool isPlaying;
    bool isGameOver; // Флаг для состояния игры
    bool isWalking;
    bool isJumping;                      // Флаг для прыжка
    bool isCrouching;
    float currentFrame;
    float verticalVelocity;              // Вертикальная скорость
    int score;
    sf::Vector2f runnerMovement;
    sf::IntRect walkingFrames[4];

    std::vector<std::unique_ptr<Enemy>> enemies; // Вектор для всех врагов

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
    
    void showGameOverScreen(); // Функция для отображения экрана окончания игры
    bool isMouseOverButton(const sf::Sprite& button); // Функция для проверки нахождения мыши над кнопкой
};

#endif // RUNNER_H
