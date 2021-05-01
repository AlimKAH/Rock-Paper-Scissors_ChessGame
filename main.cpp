#include <iostream>
#include <tuple>
#include <map>
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
private:
    std::tuple<int, int> pos;
public:
    Position(int x, int y);
    int getX() const {
        return std::get<0>(this->pos);
    }
    int getY() const {
        return std::get<1>(this->pos);
    }
    void setX(int value) {
        this->pos = std::make_tuple(value, std::get<1>(this->pos));
    }
    void setY(int value) {
        this->pos = std::make_tuple(std::get<0>(this->pos), value);
    }
};

Position::Position(int x, int y) {
    this->pos = std::make_tuple(x, y);
}

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
 * @param isMovable the movability of an object
 */
class Unit {
protected:
    bool isMovable;
    char symbol;
    UnitType type;
    Player* owner;
    Position* position{};
public:
    Unit(bool isMovable, char symbol, UnitType type, Player* owner);
    virtual ~Unit() = default;
    /**
     * ColisionResult - Collision after player makes move and meets a unit of gaming board
     * @method collision - returns variables of specific unit
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

Unit::Unit(bool isMovable, char symbol, UnitType type, Player* owner) {
    this->isMovable = isMovable;
    this->symbol = symbol;
    this->type = type;
    this->owner = owner;
}

/**
 * Player-specific unit types(Rock-Paper-Scissors)
 */
class RPS : public Unit {
public:
    RPS(bool isMovable, char symbol, UnitType type, Player* owner): Unit(isMovable, symbol, type, owner) {};
    CollisionResult collision(Unit *collidedUnit) override;
};
/**
 *  CollisionResult - collision result for a situation, where 2 units under player control are collided
 * @return rollback - 2 similar units of the same player collided
 * @return battle - 2 non-similar units of different players collided
 */
CollisionResult RPS::collision(Unit *collidedUnit) {
    if (this->owner->getId() == collidedUnit->getOwner()->getId()) {
        return rollback;
    } else {
        return battle;
    }
}
/**
 * Mountain - type of Unit on the board
 */
class Mountain : public Unit {
public:
    Mountain(bool isMovable, char symbol, UnitType type): Unit(isMovable, symbol, type, nullptr) {};
    CollisionResult collision(Unit *collidedUnit) override;
};
/**
 * CollisionResult Mountain collision of a player unit with Mountain unit
 * @return rollback - returns rollback
 */
CollisionResult Mountain::collision(Unit *collidedUnit) {
    return rollback;
}
/**
 * Flag - type of Unit on the board
 */
class Flag : public Unit {
public:
    Flag(bool isMovable, char symbol, UnitType type, Player* owner): Unit(isMovable, symbol, type, owner) {};
    CollisionResult collision(Unit *collidedUnit) override;
};
/**
 * CollisionResult Flag collision of a player unit with Flag unit
 * @return win - returns winner
 */
CollisionResult Flag::collision(Unit *collidedUnit) {
    return win;
}

struct UnitData { char symbol; UnitType type; };
/**
 * World - initialization of a gaming board
 */
class World {
private:
    unsigned int sizeX;
    unsigned int sizeY;
    std::map<std::shared_ptr<Position>, Unit*> player0Units; // Units of 0Player, including flag
    std::map<std::shared_ptr<Position>, Unit*> player1Units; // Units of 1Player, including flag
    std::map<Position*, Mountain*> mountains; // Mountain unit on the board
    /**
     * Generate units of both players
     */
    static void generatePlayerUnits(
            std::tuple<int, int> xBorders,  // Allowed positions for player units on x-axis
            std::tuple<int, int> yBorders,  // Allowed positions for player units on y-axis
            UnitData indexedSymbols[3],
            Position* flagPosition,
            Player* player,
            std::map<std::shared_ptr<Position>, Unit*>* mapToInsert
            );
    /**
     * Generate mountain units
     */
    /**
     * Check the validity of generated mountain
     * @param mountainPosition - coordinates of generated mountain
     * @return - returns validity true/false
     */
    bool isMountainPositionInvalid(Position* mountainPosition);
    /**
     * Generate mountains on a gaming board
     * @param amount - amount of mountains
     */
    void generateMountains(unsigned int amount);
public:
    World(unsigned int x, unsigned int y);
    /**
    * Generate world
    */
    void generateWorld(Player* player0, Player* player1, unsigned int mountainAmount);
    std::tuple<int, int> getWorldSize() const {
        return std::make_tuple(this->sizeX, this->sizeY);
    }
};
/*
 *  Generate players units in world
 */
void World::generatePlayerUnits(
        std::tuple<int, int> xBorders,
        std::tuple<int, int> yBorders,
        UnitData indexedSymbols[3],
        Position* flagPosition,
        Player* player,
        std::map<std::shared_ptr<Position>, Unit*>* mapToInsert
        ) {
    /*
     * Cycle determines positions for different unit types such as, rock/paper/scissors
     * For arrangement using next rule:
     * y mod 3 == 0 -> Rock
     * y mod 3 == 1 -> Paper
     * y mod 3 == 2 -> Scissors
     * Invert for second player
     */

    for (int x = std::get<0>(xBorders); x < std::get<1>(xBorders); x++) {
        for (int y = std::get<0>(yBorders); y < std::get<1>(yBorders); y++) {
            auto* pos = new Position(x, y);
            int index = y % 3;
            auto unitData = indexedSymbols[index];
            auto* playerUnit = new RPS(true, unitData.symbol, unitData.type, player);
            mapToInsert->insert(
                    std::pair<
                            std::shared_ptr<Position>,
                            Unit*
                            >(std::shared_ptr<Position>(pos), playerUnit));
        }
    }
    Flag* f = new Flag(false, 'f', flag, player); // Generate flag for a player
    mapToInsert->insert(
            std::pair<
                    std::shared_ptr<Position>,
                    Unit*
                    >(std::shared_ptr<Position>(flagPosition), f));
}

bool World::isMountainPositionInvalid(Position* mountainPosition) {
    int posX = mountainPosition->getX();
    int posY = mountainPosition->getY();
    if ((posX < 7 && posY < 7) || (posX > this->sizeX - 7 && posY > this->sizeY - 7)) {
        return true;
    } else if (this->mountains[mountainPosition] != nullptr) {
        return true;
    }
    return false;
};

void World::generateMountains(unsigned int amount) {
    while (this->mountains.size() != amount) {
        Position* position;
        do {
            unsigned int x = random() % (this->sizeX - 1);
            unsigned int y = random() % (this->sizeY - 1);
            position = new Position((int)x, (int)y);
        } while(this->isMountainPositionInvalid(position));
        this->mountains.insert(
                std::pair<
                        Position*,
                        Mountain*
                        >(position, new Mountain(false, 'M', mountain))
                        );
    }

}

void World::generateWorld(Player* player0, Player* player1, unsigned int mountainAmount) {
    UnitData unitData0 [3] = { {'r', rock}, {'p', paper}, {'s', scissors} };
    UnitData unitData1 [3] = { {'S', scissors}, {'P', paper}, {'R', rock} };
    this->generatePlayerUnits(
            std::make_tuple(1, 6),
            std::make_tuple(0, 6),
            unitData0,
            new Position(0, 0),
            player0,
            &this->player0Units
            );
    this->generatePlayerUnits(
            std::make_tuple(this->sizeX - 6, this->sizeX - 1),
            std::make_tuple(this->sizeY - 6, this->sizeY),
            unitData1,
            new Position((int)this->sizeX - 1, (int)this->sizeY - 1),
            player1,
            &this->player1Units
            );
    this->generateMountains(mountainAmount);
}

World::World(unsigned int x, unsigned int y) {
    this->sizeX = x;
    this->sizeY = y;
}

enum Difficulty { easy, medium, hard }; // Set of difficulties

class GameController {
private:
    Player* player0;
    Player* player1;
    Difficulty difficulty;
    World* world;
public:
    void askWorldSize();
    void askDifficulty();
    void createWorld();
    void waitPlayers() {};
    RPS battle(RPS *unit1, RPS *unit2) {};
    void commitGameState() {};
    void draw() {};
    void checkWinner() {};
};

void GameController::askWorldSize() {
    std::string input;
    std::cout << "Enter the size of the maze" << std::endl;
    std::cout << "Input style: <size_x> <size_y>" << std::endl;
    std::cout << "World size: ";
    std::cin >> input;
    std::vector<int> sizes;
    while(sizes.size() != 2) {
        std::string currentSize;
        for (auto ch : input) {
            if (ch == ' ') {
                // TODO add to sizes currentSize.
            } else {
                currentSize += ch;
            }
        }
    }
}
/*
 * Before staring simulation, ask to insert a preferred difficulty
 */
void GameController::askDifficulty() {
    int chosen;
    /*
     * Difficulty 0 - 10% of tiles are mountains
     * Difficulty 1 - 30% of tiles are mountains, Player1 uses some base strategies too
     * Difficulty 2 - 50% of tiles are mountains, Player1 only progresses in optimal manner
     */
    std::cout << "Please, define the difficulty." << std::endl;
    std::cout << "Enter '0' for easy walkthrough" << std::endl;
    std::cout << "Enter '1' so you can challenge yourself" << std::endl;
    std::cout << "Enter '2' Only supercomputers can compete there, be prepared" << std::endl;
    std::cout << "Your choice: ";
    bool isAsked = false;
    do {
        std::cin >> chosen;
        switch (chosen) {
            case 0:
                this->difficulty = easy;
                isAsked = true;
                break;
            case 1:
                this->difficulty = medium;
                isAsked = true;
                break;
            case 2:
                this->difficulty = hard;
                isAsked = true;
                break;
            default:
                std::cout << "You may define only specified values!" << std::endl;
                break;
        }
    } while(!isAsked);
}

void GameController::createWorld() {

}

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
    std::srand(time(nullptr));
    auto* gc = new GameController();
    gc->askDifficulty();
    return 0;
}
