#ifndef ENEMY_H
#define ENEMY_H

#include <SFML/Graphics.hpp>

class Enemy
{
public:
    virtual void update(sf::Time deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0; // ����������� ����� render
    virtual sf::FloatRect getBounds() const = 0; // ����������� ����� getBounds
    virtual sf::Vector2f getPosition() const = 0; // ����������� ����� getPosition
    virtual void setPosition(float x, float y) = 0; // ����������� ����� setPosition
    virtual void setSpeed(float s) = 0; // ����������� ����� setSpeed

protected:
    sf::Sprite sprite;
    float speed;
};

class Spider : public Enemy
{
public:
    Spider(const sf::Texture& texture, float x, float y);
    void update(sf::Time deltaTime) override;
    void render(sf::RenderWindow& window) override; // ���������� ������ render
    sf::FloatRect getBounds() const override; // ���������� ������ getBounds
    sf::Vector2f getPosition() const override; // ���������� ������ getPosition
    void setPosition(float x, float y) override; // ���������� ������ setPosition
    void setSpeed(float s) override; // ���������� ������ setSpeed
};

class Thorns : public Enemy
{
public:
    Thorns(const sf::Texture& texture, float x, float y);
    void update(sf::Time deltaTime) override;
    void render(sf::RenderWindow& window) override; // ���������� ������ render
    sf::FloatRect getBounds() const override; // ���������� ������ getBounds
    sf::Vector2f getPosition() const override; // ���������� ������ getPosition
    void setPosition(float x, float y) override; // ���������� ������ setPosition
    void setSpeed(float s) override; // ���������� ������ setSpeed
};

#endif // ENEMY_H
