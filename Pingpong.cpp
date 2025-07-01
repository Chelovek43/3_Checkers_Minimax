// Pingpong.cpp
#include "Pingpong.h"
#include <cmath>
#include <ctime>
#include <cstdlib>

// ����������� ����������� ��������
const float Pingpong::pi = 3.14159f; // �������� ����� ��
const int Pingpong::gameWidth = 800; // ������ �������� ����
const int Pingpong::gameHeight = 600; // ������ �������� ����
const sf::Vector2f Pingpong::paddleSize(25, 100); // ������� �������
const float Pingpong::ballRadius = 10.f; // ������ ����

// ����������� ������ Pingpong
Pingpong::Pingpong()
    : window(sf::VideoMode(gameWidth, gameHeight, 32), "SFML Pong", sf::Style::Titlebar | sf::Style::Close),
    AITime(sf::seconds(0.1f)), // ������������� �������� ������� ���������� ��
    paddleSpeed(400.f), // ������������� �������� �������
    rightPaddleSpeed(0.f), // ���������� �������� ������ ������� ����� 0
    ballSpeed(400.f), // ������������� �������� ����
    isPlaying(false) // ���������� ���� �� ��������
{
    window.setVerticalSyncEnabled(true); // �������� ������������ �������������

    // ��������� ���� ���� �� �����
    if (!ballSoundBuffer.loadFromFile("resources/ball.wav"))
        throw std::runtime_error("Failed to load ball sound");

    ballSound.setBuffer(ballSoundBuffer); // ��������� ���� � �������

    // ����������� ����� �������
    leftPaddle.setSize(paddleSize - sf::Vector2f(3, 3));
    leftPaddle.setOutlineThickness(3);
    leftPaddle.setOutlineColor(sf::Color::Black);
    leftPaddle.setFillColor(sf::Color(100, 100, 200));
    leftPaddle.setOrigin(paddleSize / 2.f);

    // ����������� ������ �������
    rightPaddle.setSize(paddleSize - sf::Vector2f(3, 3));
    rightPaddle.setOutlineThickness(3);
    rightPaddle.setOutlineColor(sf::Color::Black);
    rightPaddle.setFillColor(sf::Color(200, 100, 100));
    rightPaddle.setOrigin(paddleSize / 2.f);

    // ����������� ���
    ball.setRadius(ballRadius - 3);
    ball.setOutlineThickness(3);
    ball.setOutlineColor(sf::Color::Black);
    ball.setFillColor(sf::Color::White);
    ball.setOrigin(ballRadius / 2, ballRadius / 2);

    // ��������� ����� ������
    if (!font.loadFromFile("resources/sansation.ttf"))
        throw std::runtime_error("Failed to load font");

    // ����������� ��������� �����
    pauseMessage.setFont(font);
    pauseMessage.setCharacterSize(40);
    pauseMessage.setPosition(170.f, 150.f);
    pauseMessage.setFillColor(sf::Color::White);
    pauseMessage.setString("Welcome to SFML pong!\nPress space to start the game");
}

// �������� ���� ����
void Pingpong::run()
{
    std::srand(static_cast<unsigned int>(std::time(NULL))); // �������������� ��������� ��������� �����

    while (window.isOpen())
    {
        processEvents(); // ������������ �������
        if (isPlaying)
        {
            update(clock.restart()); // ��������� ��������� ����
        }
        render(); // ������������ ������� �� ������
    }
}

// ��������� �������
void Pingpong::processEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close(); // ��������� ����
        }
        else if (event.type == sf::Event::KeyPressed)
        {
            handlePlayerInput(event.key.code, true); // ������������ ������� ������
        }
    }
}

// ���������� ��������� ����
void Pingpong::update(sf::Time deltaTime)
{
    float dt = deltaTime.asSeconds(); // ��������� ����� � �������

    // ���������� ����� �������
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && (leftPaddle.getPosition().y - paddleSize.y / 2 > 5.f))
    {
        leftPaddle.move(0.f, -paddleSpeed * dt);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && (leftPaddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f))
    {
        leftPaddle.move(0.f, paddleSpeed * dt);
    }

    // ���������� ������ �������
    if (((rightPaddleSpeed < 0.f) && (rightPaddle.getPosition().y - paddleSize.y / 2 > 5.f)) ||
        ((rightPaddleSpeed > 0.f) && (rightPaddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f)))
    {
        rightPaddle.move(0.f, rightPaddleSpeed * dt);
    }

    // ��������� ����������� �������� ������ ������� � ����������� �� ��������� ����
    if (AITimer.getElapsedTime() > AITime)
    {
        AITimer.restart();
        if (ball.getPosition().y + ballRadius > rightPaddle.getPosition().y + paddleSize.y / 2)
        {
            rightPaddleSpeed = paddleSpeed;
        }
        else if (ball.getPosition().y - ballRadius < rightPaddle.getPosition().y - paddleSize.y / 2)
        {
            rightPaddleSpeed = -paddleSpeed;
        }
        else
        {
            rightPaddleSpeed = 0.f;
        }
    }

    // ���������� ���
    float factor = ballSpeed * dt;
    ball.move(std::cos(ballAngle) * factor, std::sin(ballAngle) * factor);

    // ��������� ������������ ���� � ������ ������
    if (ball.getPosition().x - ballRadius < 0.f)
    {
        isPlaying = false;
        pauseMessage.setString("You lost!\nPress space to restart or\nescape to exit");
    }
    if (ball.getPosition().x + ballRadius > gameWidth)
    {
        isPlaying = false;
        pauseMessage.setString("You won!\nPress space to restart or\nescape to exit");
    }
    if (ball.getPosition().y - ballRadius < 0.f)
    {
        ballSound.play();
        ballAngle = -ballAngle;
        ball.setPosition(ball.getPosition().x, ballRadius + 0.1f);
    }
    if (ball.getPosition().y + ballRadius > gameHeight)
    {
        ballSound.play();
        ballAngle = -ballAngle;
        ball.setPosition(ball.getPosition().x, gameHeight - ballRadius - 0.1f);
    }

    // ��������� ������������ ���� � ���������
    // ����� �������
    if (ball.getPosition().x - ballRadius < leftPaddle.getPosition().x + paddleSize.x / 2 &&
        ball.getPosition().x - ballRadius > leftPaddle.getPosition().x &&
        ball.getPosition().y + ballRadius >= leftPaddle.getPosition().y - paddleSize.y / 2 &&
        ball.getPosition().y - ballRadius <= leftPaddle.getPosition().y + paddleSize.y / 2)
    {
        if (ball.getPosition().y > leftPaddle.getPosition().y)
        {
            ballAngle = pi - ballAngle + (std::rand() % 20) * pi / 180;
        }
        else
        {
            ballAngle = pi - ballAngle - (std::rand() % 20) * pi / 180;
        }

        ballSound.play();
        ball.setPosition(leftPaddle.getPosition().x + ballRadius + paddleSize.x / 2 + 0.1f, ball.getPosition().y);
    }

    // ������ �������
    if (ball.getPosition().x + ballRadius > rightPaddle.getPosition().x - paddleSize.x / 2 &&
        ball.getPosition().x + ballRadius < rightPaddle.getPosition().x &&
        ball.getPosition().y + ballRadius >= rightPaddle.getPosition().y - paddleSize.y / 2 &&
        ball.getPosition().y - ballRadius <= rightPaddle.getPosition().y + paddleSize.y / 2)
    {
        if (ball.getPosition().y > rightPaddle.getPosition().y)
        {
            ballAngle = pi - ballAngle + (std::rand() % 20) * pi / 180;
        }
        else
        {
            ballAngle = pi - ballAngle - (std::rand() % 20) * pi / 180;
        }

        ballSound.play();
        ball.setPosition(rightPaddle.getPosition().x - ballRadius - paddleSize.x / 2 - 0.1f, ball.getPosition().y);
    }
}

// ��������� �������� ����

void Pingpong::render()
{
    window.clear(sf::Color(50, 200, 50)); // ������� ����

    if (isPlaying)
    {
        window.draw(leftPaddle); // ������ ����� �������
        window.draw(rightPaddle); // ������ ������ �������
        window.draw(ball); // ������ ���
    }
    else
    {
        window.draw(pauseMessage); // ������ ��������� �����
    }

    window.display(); // ���������� ���������� ����
}

// ����� ���� �� ���������� ���������
void Pingpong::reset()
{
    isPlaying = false; // ������������� ����
    pauseMessage.setString("Welcome to SFML pong!\nPress space to start the game"); // ������������� ����� ��������� �����

    // ���������� ������� � ��� � ��������� �������
    leftPaddle.setPosition(10 + paddleSize.x / 2, gameHeight / 2);
    rightPaddle.setPosition(gameWidth - 10 - paddleSize.x / 2, gameHeight / 2);
    ball.setPosition(gameWidth / 2, gameHeight / 2);

    // ������������� ��������� ���� �������� ����, ����� �� �� ��� ������� ������������
    do
    {
        ballAngle = (std::rand() % 360) * 2 * pi / 360;
    } while (std::abs(std::cos(ballAngle)) < 0.7f);
}

// ��������� ����� �� ������
void Pingpong::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
    if (key == sf::Keyboard::Escape) // ���� ������ ������� Escape
    {
        window.close(); // ��������� ����
    }
    if (key == sf::Keyboard::Space) // ���� ������ ������� ������
    {
        if (!isPlaying)
        {
            reset(); // ���������� ����
            isPlaying = true; // ��������� ����
            clock.restart(); // ������������� ������
        }
    }
}
