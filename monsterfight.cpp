#include <iostream>
#include <vector>
#include <cstdlib> // для rand() и srand()
#include <ctime> // для time()

// Генерируем рандомное число между min и max
int getRandomNumber(int min, int max)
{
    static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
    // Равномерно распределяем генерацию значения из диапазона
    return static_cast<int>(rand() * fraction * (max - min + 1) + min);
}

class Creature
{
protected:
    std::string m_name;
    char m_symbol;
    int m_healthPoint;
    int m_attackPower;
    int m_goldCount;
public:
    Creature(std::string name, char symbol, int healthPoint, int attackPower, int goldCount) :
        m_name(name),
        m_symbol(symbol),
        m_healthPoint(healthPoint),
        m_attackPower(attackPower),
        m_goldCount(goldCount)
    {};

    // Getters:
    const std::string& getName() const { return m_name; }
    char getSymbol() const { return m_symbol; }
    int getHealth() const { return m_healthPoint; }
    int getAttackPower() const { return m_attackPower; }
    int getGold() const { return m_goldCount; }

    // Other methods:
    void reduceHealth(int count)
    {
        m_healthPoint -= count;
    }

    bool isDeath()
    {
        return m_healthPoint <= 0;
    }

    void addGold(int count)
    {
        m_goldCount += count;
    }
};

class Player : public Creature
{
private:
    int m_level{ 1 };
public:
    Player(std::string name, char symbol = '@', int healthPoint = 10, int attackPower = 1, int goldCount = 0) :
        Creature(name, symbol, healthPoint, attackPower, goldCount)
    {};
    //Getters:
    int getLevel() const { return m_level; }
    //Other methods:
    void levelUp() { ++m_level; ++m_attackPower; }
    bool hasWon() { return m_level >= 20; }

};


class Monster : public Creature
{
public:
    enum EType {
        ET_DRAGON,
        ET_ORC,
        ET_SLIME,
        ET_TROLL,
        ET_MAX_TYPES
    };

    struct MonsterData
    {
        const char* name;
        char symbol;
        int healthPoint;
        int attackPower;
        int goldCount;
    };

    static MonsterData monsterData[ET_MAX_TYPES];

    Monster(EType type)
        : Creature(monsterData[type].name, monsterData[type].symbol, monsterData[type].healthPoint, monsterData[type].attackPower, monsterData[type].goldCount)
    {
    }

    static EType getRandomMonster()
    {
        return static_cast<EType>(getRandomNumber(0, ET_MAX_TYPES - 1));
    }
};

Monster::MonsterData Monster::monsterData[Monster::ET_MAX_TYPES]
{
    { "dragon", 'D', 20, 4, 100 },
    { "orc", 'o', 4, 2, 25 },
    { "slime", 's', 1, 1, 10 },
    { "troll", 't', 3, 1, 15 }
};

char inputPlayer()
{
    char in{};
    std::cout << "(R)un or (F)ight:";
    while (in != 'f' && in != 'r' && in != 'F' && in != 'R') {
        std::cin >> in;
    }
    return in;
}

void attackPlayer(Player& p, Monster& m)
{
    p.reduceHealth(m.getAttackPower());
    std::cout << "The " << m.getName() << " hit you for " << m.getAttackPower() << " damage.\n";
}

bool attackMonster(Player& p, Monster& m)
{
    m.reduceHealth(p.getAttackPower());
    std::cout << "You hit the " << m.getName() << " for " << p.getAttackPower() << " damage.\n";

    if (m.isDeath()) {
        std::cout << "You killed the " << m.getName() << ".\n";
        p.levelUp();
        std::cout << "You are now level " << p.getLevel() << " .\n";
        p.addGold(m.getGold());
        std::cout << "You found " << m.getGold() << " gold.\n";
        return true;
    }
    return false;
}

void fightMonster(Player& p, Monster& m)
{
    while (!p.isDeath() && !p.hasWon()) {
        if (inputPlayer() == 'r') {
            if (getRandomNumber(1, 2) == 1) {
                std::cout << "You successfully fled.\n";
                return;
            }
            std::cout << "You failed to flee.\n";
            attackPlayer(p, m);
            continue;
        }
        bool isKilled = attackMonster(p, m);
        if (!isKilled) {
            attackPlayer(p, m);
        }
        else {
            return;
        }
    }
}

int main()
{
    srand(static_cast<unsigned int>(time(0))); // устанавливаем значение системных часов в качестве стартового числа
    rand(); // сбрасываем первый результат

    std::cout << "Enter your name: ";
    std::string playerName;
    std::cin >> playerName;
    std::cout << "Welcome, " << playerName << ".\n";

    char ansAgain{ 'y' };
    while (ansAgain == 'y')
    {
        {
            Player p(playerName);
            while (!p.isDeath() && !p.hasWon()) {
                Monster m = Monster::getRandomMonster();
                while (m.getAttackPower() > 3 && p.getLevel() < 5) {
                    m = Monster::getRandomMonster();
                }
                std::cout << "You have encountered a " << m.getName() << " (" << m.getSymbol() << ").\n";
                fightMonster(p, m);
            }
            if (p.hasWon())
                std::cout << "You have " << p.getHealth() << " health and are carrying " << p.getGold() << " gold.\n";
            else {
                std::cout << std::endl;
                std::cout << "You died at level " << p.getLevel() << " and with " << p.getGold() << " gold.\n";
                std::cout << "Too bad you can't take it with you!\n";
            }
        }
        std::cout << std::endl;
        std::cout << "You wish play again? (y/n) ";
        std::cin >> ansAgain;
    }

    return 0;
}
