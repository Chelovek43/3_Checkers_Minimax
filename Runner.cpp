#include "Runner.h"
#include "Enemy.h"
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <iostream> // ��� ���������� ���������
#include <sstream>

const int Runner::gameWidth = 800;
const int Runner::gameHeight = 600;
//const int Runner::frameWidth = 207;
//const int Runner::frameHeight = 156;
const int Runner::numFrames = 4;
const float Runner::frameDuration = 0.1f;
const float Runner::gravity = 980.f;      // ��������� ���������� �������
const float Runner::jumpSpeed = 500.f;    // �������� ������


Runner::Runner()
    : window(sf::VideoMode(gameWidth, gameHeight), "Runner Game"),
    isPlaying(false),
    currentFrame(0),
    isJumping(false),                  // ������������� ����� ������
    isCrouching(false), 
    verticalVelocity(0.f),             // ������������� ������������ ��������
    runnerMovement(0.f, 0.f)
{
    window.setVerticalSyncEnabled(true);
    loadTextures();
    reset();
    std::srand(static_cast<unsigned int>(std::time(0)));

    // ������������� ������ �������� ������
    walkingFrames[0] = sf::IntRect(0, 0, 120, 82);
    walkingFrames[1] = sf::IntRect(123, 0, 120, 82);
    walkingFrames[2] = sf::IntRect(246, 0, 120, 82);
    walkingFrames[3] = sf::IntRect(369, 0, 120, 82);

    float groundLevel = gameHeight - walkingFrames[0].height - 100.f;
    spriteRunner.setPosition(50.f, groundLevel);

    std::cout << "Game initialized" << std::endl;
}

// �������� �������
void Runner::loadTextures()
{
    // ������ ������� ����
    sf::RectangleShape sky(sf::Vector2f(gameWidth, gameHeight));
    sky.setFillColor(sf::Color(135, 206, 250));

    // ������ ���������� �����
    sf::RectangleShape ground(sf::Vector2f(gameWidth, 100)); // ������ ����� 100 ��������
    ground.setFillColor(sf::Color(139, 69, 19));
    ground.setPosition(0.f, gameHeight - 100);

    // ������� �������� ��� ����
    sf::RenderTexture renderTexture;
    if (!renderTexture.create(gameWidth, gameHeight))
        throw std::runtime_error("Failed to create render texture");

    renderTexture.clear();
    renderTexture.draw(sky);
    renderTexture.draw(ground);
    renderTexture.display();

    textureBackground = renderTexture.getTexture();
    spriteBackground.setTexture(textureBackground);
    spriteBackground.setPosition(0.f, 0.f);

    // ��������� �������� �������� ������
    if (!textureWalking.loadFromFile("resources/walking.png"))
        throw std::runtime_error("Failed to load walking texture");

    spriteRunner.setTexture(textureWalking);
    spriteRunner.setTextureRect(walkingFrames[0]);
    spriteRunner.setPosition(50.f, gameHeight - walkingFrames[0].height - 100.f);


    // ��������� �������� �����
    if (!textureGrass.loadFromFile("resources/grass.png"))
    {
        std::cerr << "Failed to load grass texture" << std::endl;
        throw std::runtime_error("Failed to load grass texture");
    }
    else
    {
        std::cout << "Grass texture loaded successfully" << std::endl;
    }

    // �������������� ������� �����
    for (int i = 0; i < 5; ++i)
    {
        spawnGrass();
    }

    if (!textureEnemy.loadFromFile("resources/spider11.png"))
    {
        std::cerr << "Failed to load enemy texture" << std::endl;
        throw std::runtime_error("Failed to load enemy texture");
    }
    std::cout << "Enemy texture loaded successfully" << std::endl;

    if (!textureThorns.loadFromFile("resources/thorns.png"))
    {
        std::cerr << "Failed to load thorns texture" << std::endl;
        throw std::runtime_error("Failed to load thorns texture");
    }
    std::cout << "Thorns texture loaded successfully" << std::endl;

    if (!textureResetButton.loadFromFile("resources/reset_button.png"))
    {
        std::cerr << "Failed to load reset button texture" << std::endl;
        throw std::runtime_error("Failed to load reset button texture");
    }
    std::cout << "Reset button texture loaded successfully" << std::endl;

    // ��������� ����� ������
    if (!font.loadFromFile("resources/Blacker-Pro-Text-Book-trial.ttf"))
    {
        std::cerr << "Failed to load font" << std::endl;
        throw std::runtime_error("Failed to load font");
    }
    std::cout << "Font loaded successfully" << std::endl;

    // ��������� ����� ������
    if (!fontForNums.loadFromFile("resources/AsA_rus.ttf"))
    {
        std::cerr << "Failed to load font" << std::endl;
        throw std::runtime_error("Failed to load font");
    }
    std::cout << "Font loaded successfully" << std::endl;

    spriteResetButton.setTexture(textureResetButton);
    spriteResetButton.setTextureRect(sf::IntRect(0, 0, 64, 64));
    spriteResetButton.setPosition((gameWidth - 64) / 2.f, gameHeight / 2.f);

    startText.setFont(font);
    startText.setString(L"������� �� ������ ��� ������ ����");
    startText.setCharacterSize(24);
    startText.setFillColor(sf::Color::White);
    startText.setPosition((gameWidth - startText.getLocalBounds().width) / 2.f, gameHeight / 2.f);

    gameOverText.setFont(font);
    gameOverText.setString(L"������� ���?");
    gameOverText.setCharacterSize(24);
    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setPosition((gameWidth - gameOverText.getLocalBounds().width) / 2.f, gameHeight / 2.f - 50);

    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
	scoreText.setPosition(gameWidth - 200, 10); // ������� ������ ������ ������

    scoreNums.setFont(fontForNums);
    scoreNums.setCharacterSize(24);
    scoreNums.setFillColor(sf::Color::White);
    scoreNums.setPosition(gameWidth - 100, 10); // ������� ������ ������ ������

    spawnEnemy();
    std::cout << "Textures loaded and enemies spawned" << std::endl;
}

// �������� ���� ����
void Runner::run()
{
    loadTextures();

    while (window.isOpen())
    {
        processEvents();
        if (isPlaying)
        {
            sf::Time deltaTime = clock.restart();
            update(deltaTime);
        }
        render();
        
        if (isGameOver)
        {
            showGameOverScreen();
        }
    }
}

// ��������� �������
void Runner::processEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }
        else if (event.type == sf::Event::KeyPressed)
        {
            handlePlayerInput(event.key.code, true);
        }
        else if (event.type == sf::Event::KeyReleased)
        {
            handlePlayerInput(event.key.code, false);
        }
    }
}

// ���������� ��������� ����
void Runner::update(sf::Time deltaTime)
{
    std::cout << "Updating game" << std::endl;
    if (isPlaying)
    {
        // ���������� ���������� �����
        score = static_cast<int>(gameClock.getElapsedTime().asSeconds());
		scoreText.setString(L"����: ");
		scoreNums.setString(std::to_wstring(score));

        updateAnimation(deltaTime);
        updateGrass(deltaTime);
        updateEnemies(deltaTime);

        // ��������� ������������
        if (checkCollisions())
        {
            isPlaying = false;
            std::cout << "Game Over!" << std::endl;
        }

        // ��������� ���������� � ������
        if (isJumping)
        {
            verticalVelocity += gravity * deltaTime.asSeconds();
            spriteRunner.move(0.f, verticalVelocity * deltaTime.asSeconds());

            if (spriteRunner.getPosition().y + spriteRunner.getGlobalBounds().height >= gameHeight - 100.f)
            {
                spriteRunner.setPosition(spriteRunner.getPosition().x, gameHeight - spriteRunner.getGlobalBounds().height - 100.f);
                isJumping = false;
                verticalVelocity = 0.f;
            }
        }

        // ��������� ������� ������ � ������ �����������
        float newX = spriteRunner.getPosition().x + runnerMovement.x * deltaTime.asSeconds();
        newX = std::max(0.f, std::min(newX, static_cast<float>(gameWidth - spriteRunner.getGlobalBounds().width)));
        spriteRunner.setPosition(newX, spriteRunner.getPosition().y);

        
    }
}

// ��������� �������� ����
void Runner::render()
{
    window.clear(sf::Color::White); // ������� ������ � ������ �����

    if (isPlaying)
    {
        window.draw(spriteBackground);

        for (const auto& grass : grassSprites)
        {
            window.draw(grass);
        }

        for (auto& enemy : enemies)
        {
            enemy->render(window); // ����� ������������ ������ render
        }

        window.draw(spriteRunner);
        window.draw(scoreText); // ��������� ������ �����
		window.draw(scoreNums); // ��������� ����� �����
        std::cout << "Rendering Sprite Runner Position: (" << spriteRunner.getPosition().x
            << ", " << spriteRunner.getPosition().y << ")" << std::endl;
    }
    else if (isGameOver)
    {
        window.draw(spriteBackground);

        for (const auto& grass : grassSprites)
        {
            window.draw(grass);
        }

        for (auto& enemy : enemies)
        {
            enemy->render(window); // ����� ������������ ������ render
        }

        window.draw(spriteRunner);

        window.draw(scoreText); // ��������� ������ �����
		window.draw(scoreNums); // ��������� ����� �����
        window.draw(gameOverText);
        window.draw(spriteResetButton);
    }
    else // ��������� �����
    {
        window.clear(sf::Color::Black);
        window.draw(startText);
    }

    window.display(); // ���������� ������ � ����� �����
}


// ����� ����
void Runner::reset()
{
    isPlaying = false;
    isGameOver = false;
    float groundLevel = gameHeight - walkingFrames[0].height - 100.f; // ������ ������� �����
    spriteRunner.setPosition(50.f, groundLevel);
    currentFrame = 0;
    grassSprites.clear(); // ������� ������� �����
    for (int i = 0; i < 4; ++i)
    {
        spawnGrass();
    }
    enemies.clear(); // ������� ������
    spawnEnemy();

    score = 0; // ����� �����
    gameClock.restart(); // ���������� �����
    std::cout << "Game reset" << std::endl; // ��������� � ������ ����
}

// ���������� ��������
void Runner::updateAnimation(sf::Time deltaTime) 
{
    if (animationClock.getElapsedTime().asSeconds() > frameDuration)
    {
		currentFrame += 0.5f; // ��������� ����, ��������� ����� (0.5f - �������� ��������)
        if (currentFrame >= 4.0f) currentFrame -= 4.0f; // ���� ����� �� ���������� �����, �������� �������
        spriteRunner.setTextureRect(walkingFrames[static_cast<int>(currentFrame)]); // ��������� ���� ��������
        animationClock.restart();
        std::cout << "Current Frame Updated to: " << static_cast<int>(currentFrame)
            << ", Sprite Position: (" << spriteRunner.getPosition().x
            << ", " << spriteRunner.getPosition().y << ")" << std::endl; // ���������� ���������
    }
}

// ��������� ����� �� ������
void Runner::handlePlayerInput(sf::Keyboard::Key key, bool isPressed) 
{
    float speed = 200.f;
    float groundLevel = gameHeight - walkingFrames[0].height - 100.f;

    if (key == sf::Keyboard::Space && isPressed && !isPlaying)
    {
        isPlaying = true;
        clock.restart();
        animationClock.restart();
        std::cout << "Game started" << std::endl;
    }

    isWalking = false;

    if (isPressed)
    {
        if ((key == sf::Keyboard::Down || key == sf::Keyboard::S))
        {
            isCrouching = true;
            spriteRunner.setScale(1.f, 0.5f); // ��������� ������� ��� ����������
            spriteRunner.setPosition(spriteRunner.getPosition().x, groundLevel + walkingFrames[0].height * 0.5f); // ��������� ��������� �� �����
        }
        else if ((key == sf::Keyboard::Up || key == sf::Keyboard::W) && !isJumping)
        {
            isJumping = true;
            verticalVelocity = -jumpSpeed;
            isWalking = true;
        }
        else if (key == sf::Keyboard::Left || key == sf::Keyboard::A)
        {
            isWalking = true;
        }
        else if (key == sf::Keyboard::Right || key == sf::Keyboard::D)
        {
            isWalking = true;
        }
    }
    else
    {
        if (key == sf::Keyboard::Down || key == sf::Keyboard::S)
        {
            isCrouching = false;
            spriteRunner.setScale(1.f, 1.f); // ���������� ������� ��� ���������� ����������
            spriteRunner.setPosition(spriteRunner.getPosition().x, groundLevel); // ��������� ��������� �� �����
        }
    }
}

// ���������� �����
void Runner::updateGrass(sf::Time deltaTime) 
{
    float speed = 200.f;
    float movement = speed * deltaTime.asSeconds();

    for (auto& grass : grassSprites)
    {
        grass.move(-movement, 0.f);
        //std::cout << "Grass sprite at x: " << grass.getPosition().x << ", y: " << grass.getPosition().y << std::endl;
    }

    grassSprites.erase(
        std::remove_if(grassSprites.begin(), grassSprites.end(), [&](const sf::Sprite& grass) {
            return grass.getPosition().x + grass.getGlobalBounds().width < 0;
            }),
        grassSprites.end()
    );

    while (grassSprites.size() < 10)
    {
        spawnGrass();
    }
}

// �������� ����� �����
void Runner::spawnGrass()
{
    sf::Sprite grass(textureGrass);
    float x = static_cast<float>(gameWidth + (std::rand() % gameWidth));
    float y = gameHeight - grass.getGlobalBounds().height - 100.f;
    grass.setPosition(x, y);
    grassSprites.push_back(grass);
    //std::cout << "New grass sprite at x: " << x << ", y: " << y << std::endl; // ��������� � �������� ����� �����
}

// ���������� ������
void Runner::updateEnemies(sf::Time deltaTime)
{
    for (auto& enemy : enemies)
    {
        enemy->update(deltaTime); // ��������� ������� ������
    }

    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const std::unique_ptr<Enemy>& enemy) { return enemy->getPosition().x < 0; }), enemies.end());

    static sf::Clock spawnClock;
    if (spawnClock.getElapsedTime().asSeconds() > 2.f && enemies.size() == 0)
    {
        spawnEnemy();
        spawnClock.restart();
    }
}

// �������� ������ �����
void Runner::spawnEnemy()
{
    if (enemies.size() == 0)
    {
        int enemyType = std::rand() % 2; // 0 ��� �����, 1 ��� �����
        if (enemyType == 0)
        {
            enemies.push_back(std::make_unique<Spider>(textureEnemy, gameWidth, gameHeight - 250.f));
            std::cout << "Spider spawned" << std::endl;
        }
        else
        {
            enemies.push_back(std::make_unique<Thorns>(textureThorns, gameWidth, gameHeight - 130.f));
            std::cout << "Thorns spawned" << std::endl;
        }
    }
}

// �������� ������������
bool Runner::checkCollisions()
{
    for (const auto& enemy : enemies)
    {
        if (spriteRunner.getGlobalBounds().intersects(enemy->getBounds())) // ����� ������������ ������ getBounds
        {
            std::cout << "Collision detected at position: ("
                << spriteRunner.getPosition().x << ", "
                << spriteRunner.getPosition().y << ")" << std::endl;
            isGameOver = true;
            isPlaying = false;
            return true;
        }
    }
    return false;
}

// ����������� ������ ��������� ����
void Runner::showGameOverScreen()
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        if (isMouseOverButton(spriteResetButton))
        {
            reset();
        }
    }
}

// �������� ������� ���� �� ������
bool Runner::isMouseOverButton(const sf::Sprite& button)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::FloatRect buttonBounds = button.getGlobalBounds();
    return buttonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
}

//0,0,120,80;  123, 0, 243,82; 246, 0, 366,82; 369, 0, 489,82 