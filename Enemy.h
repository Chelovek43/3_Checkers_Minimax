#ifndef ENEMY_H
#define ENEMY_H

#include <SFML/Graphics.hpp>

class Enemy
{
public:
    virtual void update(sf::Time deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0; // Виртуальный метод render
    virtual sf::FloatRect getBounds() const = 0; // Виртуальный метод getBounds
    virtual sf::Vector2f getPosition() const = 0; // Виртуальный метод getPosition
    virtual void setPosition(float x, float y) = 0; // Виртуальный метод setPosition
    virtual void setSpeed(float s) = 0; // Виртуальный метод setSpeed

protected:
    sf::Sprite sprite;
    float speed;
};

class Spider : public Enemy
{
public:
    Spider(const sf::Texture& texture, float x, float y);
    void update(sf::Time deltaTime) override;
    void render(sf::RenderWindow& window) override; // Реализация метода render
    sf::FloatRect getBounds() const override; // Реализация метода getBounds
    sf::Vector2f getPosition() const override; // Реализация метода getPosition
    void setPosition(float x, float y) override; // Реализация метода setPosition
    void setSpeed(float s) override; // Реализация метода setSpeed
};

class Thorns : public Enemy
{
public:
    Thorns(const sf::Texture& texture, float x, float y);
    void update(sf::Time deltaTime) override;
    void render(sf::RenderWindow& window) override; // Реализация метода render
    sf::FloatRect getBounds() const override; // Реализация метода getBounds
    sf::Vector2f getPosition() const override; // Реализация метода getPosition
    void setPosition(float x, float y) override; // Реализация метода setPosition
    void setSpeed(float s) override; // Реализация метода setSpeed
};

#endif // ENEMY_H
