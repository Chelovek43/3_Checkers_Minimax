#include "Enemy.h"

// Базовые функции для всех врагов (оставляем абстрактными)

void Spider::render(sf::RenderWindow& window)
{
    window.draw(sprite);
}

sf::FloatRect Spider::getBounds() const
{
    return sprite.getGlobalBounds();
}

sf::Vector2f Spider::getPosition() const
{
    return sprite.getPosition();
}

void Spider::setPosition(float x, float y)
{
    sprite.setPosition(x, y);
}

void Spider::setSpeed(float s)
{
    speed = s;
}

// Реализация пауков
Spider::Spider(const sf::Texture& texture, float x, float y)
{
    sprite.setTexture(texture);
    sprite.setPosition(x, y);
    sprite.setOrigin(texture.getSize().x / 2.f, 0.f); // Центрирование спрайта
    speed = 600.f;
}

void Spider::update(sf::Time deltaTime)
{
    sprite.move(-speed * deltaTime.asSeconds(), 0.f); // Перемещаем паука по оси X
}

// Реализация шипов
void Thorns::render(sf::RenderWindow& window)
{
    window.draw(sprite);
}

sf::FloatRect Thorns::getBounds() const
{
    return sprite.getGlobalBounds();
}

sf::Vector2f Thorns::getPosition() const
{
    return sprite.getPosition();
}

void Thorns::setPosition(float x, float y)
{
    sprite.setPosition(x, y);
}

void Thorns::setSpeed(float s)
{
    speed = s;
}

Thorns::Thorns(const sf::Texture& texture, float x, float y)
{
    sprite.setTexture(texture);
    sprite.setPosition(x, y);
    sprite.setOrigin(texture.getSize().x / 2.f, 0.f); // Центрирование спрайта
    speed = 600.f;
}

void Thorns::update(sf::Time deltaTime)
{
    sprite.move(-speed * deltaTime.asSeconds(), 0.f); // Перемещаем шипы по оси X
}
