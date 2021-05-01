#include <iostream>
#include <tuple>
#include <stack>
#include <vector>
#include <cstdlib>

const int TURN_MAX_TIME = 400; /// Maximum time for a turn of a player
/**
 * Initialization of a player
 */
class Player {
private:
    int id;
public:
    /**
     * Getter of player id {0,1}
     * @return the id of a player
     */
    int getId() const {
        return this->id;
    }
};
/**
 * Position of player
 */
class Position {
public:
    std::tuple<int, int> pos;
};

/**
 * Contains set of unit types
 */
enum UnitType { rock, scissors, paper, mountain, flag };
enum CollisionResult { win, rollback, battle };

/**
 * Contains parameters and method for unit to follow
 * @param symbol the 'sprite' of an unit
 * @param UnitType the type of an unit
 * @param Player the owner of an unit
 * @param Position the position of an unit
 */
class Unit {
protected:
    char symbol;
    UnitType type;
    Player* owner;
    Position* position{};
public:
    Unit(char symbol, UnitType type, Player *owner);
    virtual ~Unit() = default;
    /**
     * Collision after player makes move and meets a unit of gaming board
     *
     * @param collidedUnit the type of unit collided with
     */
    virtual CollisionResult collision(Unit *collidedUnit) = 0;
    char getSymbol() const {
        return this->symbol;
    }
    UnitType getType() const {
        return this->type;
    }
    Player* getOwner() const {
        return this->owner;
    }
    void setPosition(Position* pos) {
        this->position = pos;
    }
    Position* getPosition() const {
        return this->position;
    }
};

Unit::Unit(char symbol, UnitType type, Player *owner) {
    this->symbol = symbol;
    this->type = type;
    this->owner = owner;
}

/**
 * Initializing Rock type of a unit
 */
class RPS : public Unit {
public:
    CollisionResult collision(Unit *collidedUnit) override;
};

CollisionResult RPS::collision(Unit *collidedUnit) {
    if (this->owner->getId() == collidedUnit->getOwner()->getId()) {
        return rollback;
    } else {
        return battle;
    }
}

class Mountain : public Unit {
public:
    CollisionResult collision(Unit *collidedUnit) override;
};

CollisionResult Mountain::collision(Unit *collidedUnit) {
    return rollback;
}

class Flag : public Unit {
public:
    CollisionResult collision(Unit *collidedUnit) override;
};

CollisionResult Flag::collision(Unit *collidedUnit) {
    return win;
}

class World {};
class GameController {
private:
    std::vector<Player*> players;
public:
    void waitPlayers() {};
    RPS battle(RPS *unit1, RPS *unit2) {};
    void commitGameState() {};
    void draw() {};
    void checkWinner() {};
};

/**
 * Clears console after each step, allows
 * to spectate game live
 */
void clearConsole() {
#ifdef WINDOWS
    std::system("cls")
#else
    std::system("clear");
#endif
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
