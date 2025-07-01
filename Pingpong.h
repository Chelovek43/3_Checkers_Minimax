// Pingpong.h
#ifndef PINGPONG_H
#define PINGPONG_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Pingpong
{
public:
    Pingpong();
    void run();

private:
    // Константы, используемые в игре
    static const float pi; // Значение числа Пи
    static const int gameWidth; // Ширина игрового окна
    static const int gameHeight; // Высота игрового окна
    static const sf::Vector2f paddleSize; // Размеры ракеток
    static const float ballRadius; // Радиус мяча

    // Элементы игры
    sf::RenderWindow window; // Окно приложения
    sf::SoundBuffer ballSoundBuffer; // Буфер звука мяча
    sf::Sound ballSound; // Звук мяча
    sf::RectangleShape leftPaddle; // Левая ракетка
    sf::RectangleShape rightPaddle; // Правая ракетка
    sf::CircleShape ball; // Мяч
    sf::Font font; // Шрифт текста
    sf::Text pauseMessage; // Сообщение паузы

    // Переменные для управления игрой
    sf::Clock AITimer; // Таймер для управления ИИ
    const sf::Time AITime; // Время обновления ИИ
    const float paddleSpeed; // Скорость ракетки
    float rightPaddleSpeed; // Скорость правой ракетки
    const float ballSpeed; // Скорость мяча
    float ballAngle; // Угол движения мяча
    sf::Clock clock; // Часы для отслеживания времени
    bool isPlaying; // Флаг состояния игры

    // Методы для выполнения действий в игре
    void processEvents(); // Обработка событий
    void update(sf::Time deltaTime); // Обновление состояния игры
    void render(); // Отрисовка объектов игры
    void reset(); // Сброс игры
    void handlePlayerInput(sf::Keyboard::Key key, bool isPressed); // Обработка ввода от игрока
};

#endif // PINGPONG_H
