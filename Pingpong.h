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
    // ���������, ������������ � ����
    static const float pi; // �������� ����� ��
    static const int gameWidth; // ������ �������� ����
    static const int gameHeight; // ������ �������� ����
    static const sf::Vector2f paddleSize; // ������� �������
    static const float ballRadius; // ������ ����

    // �������� ����
    sf::RenderWindow window; // ���� ����������
    sf::SoundBuffer ballSoundBuffer; // ����� ����� ����
    sf::Sound ballSound; // ���� ����
    sf::RectangleShape leftPaddle; // ����� �������
    sf::RectangleShape rightPaddle; // ������ �������
    sf::CircleShape ball; // ���
    sf::Font font; // ����� ������
    sf::Text pauseMessage; // ��������� �����

    // ���������� ��� ���������� �����
    sf::Clock AITimer; // ������ ��� ���������� ��
    const sf::Time AITime; // ����� ���������� ��
    const float paddleSpeed; // �������� �������
    float rightPaddleSpeed; // �������� ������ �������
    const float ballSpeed; // �������� ����
    float ballAngle; // ���� �������� ����
    sf::Clock clock; // ���� ��� ������������ �������
    bool isPlaying; // ���� ��������� ����

    // ������ ��� ���������� �������� � ����
    void processEvents(); // ��������� �������
    void update(sf::Time deltaTime); // ���������� ��������� ����
    void render(); // ��������� �������� ����
    void reset(); // ����� ����
    void handlePlayerInput(sf::Keyboard::Key key, bool isPressed); // ��������� ����� �� ������
};

#endif // PINGPONG_H
