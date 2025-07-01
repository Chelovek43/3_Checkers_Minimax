// Pingpong.cpp
#include "Pingpong.h"
#include <cmath>
#include <ctime>
#include <cstdlib>

// Определение статических констант
const float Pingpong::pi = 3.14159f; // Значение числа Пи
const int Pingpong::gameWidth = 800; // Ширина игрового окна
const int Pingpong::gameHeight = 600; // Высота игрового окна
const sf::Vector2f Pingpong::paddleSize(25, 100); // Размеры ракеток
const float Pingpong::ballRadius = 10.f; // Радиус мяча

// Конструктор класса Pingpong
Pingpong::Pingpong()
    : window(sf::VideoMode(gameWidth, gameHeight, 32), "SFML Pong", sf::Style::Titlebar | sf::Style::Close),
    AITime(sf::seconds(0.1f)), // Устанавливаем интервал времени обновления ИИ
    paddleSpeed(400.f), // Устанавливаем скорость ракеток
    rightPaddleSpeed(0.f), // Изначально скорость правой ракетки равна 0
    ballSpeed(400.f), // Устанавливаем скорость мяча
    isPlaying(false) // Изначально игра не запущена
{
    window.setVerticalSyncEnabled(true); // Включаем вертикальную синхронизацию

    // Загружаем звук мяча из файла
    if (!ballSoundBuffer.loadFromFile("resources/ball.wav"))
        throw std::runtime_error("Failed to load ball sound");

    ballSound.setBuffer(ballSoundBuffer); // Связываем звук с буфером

    // Настраиваем левую ракетку
    leftPaddle.setSize(paddleSize - sf::Vector2f(3, 3));
    leftPaddle.setOutlineThickness(3);
    leftPaddle.setOutlineColor(sf::Color::Black);
    leftPaddle.setFillColor(sf::Color(100, 100, 200));
    leftPaddle.setOrigin(paddleSize / 2.f);

    // Настраиваем правую ракетку
    rightPaddle.setSize(paddleSize - sf::Vector2f(3, 3));
    rightPaddle.setOutlineThickness(3);
    rightPaddle.setOutlineColor(sf::Color::Black);
    rightPaddle.setFillColor(sf::Color(200, 100, 100));
    rightPaddle.setOrigin(paddleSize / 2.f);

    // Настраиваем мяч
    ball.setRadius(ballRadius - 3);
    ball.setOutlineThickness(3);
    ball.setOutlineColor(sf::Color::Black);
    ball.setFillColor(sf::Color::White);
    ball.setOrigin(ballRadius / 2, ballRadius / 2);

    // Загружаем шрифт текста
    if (!font.loadFromFile("resources/sansation.ttf"))
        throw std::runtime_error("Failed to load font");

    // Настраиваем сообщение паузы
    pauseMessage.setFont(font);
    pauseMessage.setCharacterSize(40);
    pauseMessage.setPosition(170.f, 150.f);
    pauseMessage.setFillColor(sf::Color::White);
    pauseMessage.setString("Welcome to SFML pong!\nPress space to start the game");
}

// Основной цикл игры
void Pingpong::run()
{
    std::srand(static_cast<unsigned int>(std::time(NULL))); // Инициализируем генератор случайных чисел

    while (window.isOpen())
    {
        processEvents(); // Обрабатываем события
        if (isPlaying)
        {
            update(clock.restart()); // Обновляем состояние игры
        }
        render(); // Отрисовываем объекты на экране
    }
}

// Обработка событий
void Pingpong::processEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close(); // Закрываем окно
        }
        else if (event.type == sf::Event::KeyPressed)
        {
            handlePlayerInput(event.key.code, true); // Обрабатываем нажатия клавиш
        }
    }
}

// Обновление состояния игры
void Pingpong::update(sf::Time deltaTime)
{
    float dt = deltaTime.asSeconds(); // Переводим время в секунды

    // Перемещаем левую ракетку
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && (leftPaddle.getPosition().y - paddleSize.y / 2 > 5.f))
    {
        leftPaddle.move(0.f, -paddleSpeed * dt);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && (leftPaddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f))
    {
        leftPaddle.move(0.f, paddleSpeed * dt);
    }

    // Перемещаем правую ракетку
    if (((rightPaddleSpeed < 0.f) && (rightPaddle.getPosition().y - paddleSize.y / 2 > 5.f)) ||
        ((rightPaddleSpeed > 0.f) && (rightPaddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f)))
    {
        rightPaddle.move(0.f, rightPaddleSpeed * dt);
    }

    // Обновляем направление движения правой ракетки в зависимости от положения мяча
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

    // Перемещаем мяч
    float factor = ballSpeed * dt;
    ball.move(std::cos(ballAngle) * factor, std::sin(ballAngle) * factor);

    // Проверяем столкновения мяча с краями экрана
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

    // Проверяем столкновения мяча с ракетками
    // Левая ракетка
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

    // Правая ракетка
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

// Отрисовка объектов игры

void Pingpong::render()
{
    window.clear(sf::Color(50, 200, 50)); // Очищаем окно

    if (isPlaying)
    {
        window.draw(leftPaddle); // Рисуем левую ракетку
        window.draw(rightPaddle); // Рисуем правую ракетку
        window.draw(ball); // Рисуем мяч
    }
    else
    {
        window.draw(pauseMessage); // Рисуем сообщение паузы
    }

    window.display(); // Отображаем содержимое окна
}

// Сброс игры до начального состояния
void Pingpong::reset()
{
    isPlaying = false; // Останавливаем игру
    pauseMessage.setString("Welcome to SFML pong!\nPress space to start the game"); // Устанавливаем текст сообщения паузы

    // Возвращаем ракетки и мяч в начальные позиции
    leftPaddle.setPosition(10 + paddleSize.x / 2, gameHeight / 2);
    rightPaddle.setPosition(gameWidth - 10 - paddleSize.x / 2, gameHeight / 2);
    ball.setPosition(gameWidth / 2, gameHeight / 2);

    // Устанавливаем случайный угол движения мяча, чтобы он не был слишком вертикальным
    do
    {
        ballAngle = (std::rand() % 360) * 2 * pi / 360;
    } while (std::abs(std::cos(ballAngle)) < 0.7f);
}

// Обработка ввода от игрока
void Pingpong::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
    if (key == sf::Keyboard::Escape) // Если нажата клавиша Escape
    {
        window.close(); // Закрываем окно
    }
    if (key == sf::Keyboard::Space) // Если нажата клавиша Пробел
    {
        if (!isPlaying)
        {
            reset(); // Сбрасываем игру
            isPlaying = true; // Запускаем игру
            clock.restart(); // Перезапускаем таймер
        }
    }
}
