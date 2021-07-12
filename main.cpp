#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <string>
#include <ctime>

const int size = 15;

enum Motion {
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
};

struct Position {
    int x = 0;
    int y = 0;
};

struct Character {
    std::string name = "unknown";
    int health = 0;
    int armor = 0;
    int damage = 0;
    Position coord;
    bool alive = true;
};


void placement (char (&field)[size][size],std::vector<Character> &players) {
    for (int i=0;i < size;++i){
        for (int j=0;j < size;++j){
            field[i][j] = '.';
        }
    }
    for (int i=0;i < players.size()-1;++i) {
        if (players[i].alive) {
            field[players[i].coord.y][players[i].coord.x] = 'E';
        }
    }
    if (players[5].alive) {
        field[players[5].coord.y][players[5].coord.x] = 'P';
    }

    for (int i=0;i < size;++i) {
        for (int j=0;j < size;++j) {
            std::cout << field[i][j] << " ";
        } std::cout << std::endl;
    } std::cout << std::endl << std::endl;
}

void make_enemies (std::vector<Character> &enemies) {
    for (int i=0;i < 5;++i) {
        enemies[i].name = "Enemy #" + std::to_string(i);
        enemies[i].health = rand()%100 + 50;
        enemies[i].armor = rand()%50;
        enemies[i].damage = rand()%15 + 30;
        enemies[i].coord.x = rand()%size;
        enemies[i].coord.y = rand()%size;
    }
}

void make_player (std::vector<Character> &players) {
    std::cout << "Input character name:";
    std::cin >> players[5].name;
    std::cout << "Input character hp level:";
    std::cin >> players[5].health;
    std::cout << "Input character armor level:";
    std::cin >> players[5].armor;
    std::cout << "Input character damage level:";
    std::cin >> players[5].damage;
    players[5].coord.x = rand()%size;
    players[5].coord.y = rand()%size;
}

void save(std::vector<Character> &players) {
    std::ofstream file("saveGame.bin", std::ios::binary);
    for (int i=0;i < players.size();++i) {
        int len = players[i].name.length();
        file.write((char*)&len, sizeof(len));
        file.write(players[i].name.c_str(), len);
        file.write((char*)&players[i].alive, sizeof(players[i].alive));
        file.write((char*)&players[i].health, sizeof(players[i].health));
        file.write((char*)&players[i].armor, sizeof(players[i].armor));
        file.write((char*)&players[i].damage, sizeof(players[i].damage));
        file.write((char*)&players[i].coord.x, sizeof(players[i].coord.x));
        file.write((char*)&players[i].coord.y, sizeof(players[i].coord.y));
    }
    file.close();
    std::cout << "Game saved!" << std::endl;
}

void load(std::vector<Character> &players) {
    std::ifstream file("saveGame.bin", std::ios::binary);
    if (file.is_open()) {
        for (int i=0;i < players.size();++i) {
            int len;
            file.read((char*)&len, sizeof(len));
            players[i].name.resize(len);
            file.read((char*)players[i].name.c_str(), len);
            file.read((char*)&players[i].alive, sizeof(players[i].alive));
            file.read((char*)&players[i].health, sizeof(players[i].health));
            file.read((char*)&players[i].armor, sizeof(players[i].armor));
            file.read((char*)&players[i].damage, sizeof(players[i].damage));
            file.read((char*)&players[i].coord.x, sizeof(players[i].coord.x));
            file.read((char*)&players[i].coord.y, sizeof(players[i].coord.y));
        }
    } std::cout << "Game load." << std::endl;
}

bool check_motion (Character &players, int move) {
    if (move == LEFT) --players.coord.x;
    if (move == RIGHT) ++players.coord.x;
    if (move == TOP) --players.coord.y;
    if (move == BOTTOM) ++players.coord.y;

    if (players.coord.x < 0) {
        ++players.coord.x;
        return false;
    }
    if (players.coord.x >= size) {
        --players.coord.x;
        return false;
    }
    if (players.coord.y < 0) {
        ++players.coord.y;
        return false;
    }
    if (players.coord.y >= size) {
        --players.coord.y;
        return false;
    }
    return true;

}

bool encounter (Character &attacker, Character &defending) {
    return defending.alive && attacker.coord.x == defending.coord.x
        && attacker.coord.y == defending.coord.y;
}

void turn_back (Character &players, int move) {
    if (move == LEFT) ++players.coord.x;
    if (move == RIGHT) --players.coord.x;
    if (move == TOP) ++players.coord.y;
    if (move == BOTTOM) --players.coord.y;
}

void damage_dealt (Character &attacker, Character &defending){
    defending.armor -= attacker.damage;
    if (defending.armor < 0) {
        defending.health += defending.armor;
        defending.armor = 0;
    }
    if (defending.health <= 0) {
        defending.health = 0;
        defending.alive = false;
    }
}

int main() {

    std::srand(std::time(nullptr));

    char playingField[size][size];
    std::vector<Character> players (6);

    make_enemies (players);
    make_player (players);

    bool matchCoord = true;
    while (matchCoord) {
        matchCoord = false;
        for (int i=0;i < players.size() - 1;++i) {
            if ((players[i].coord.x == players[i+1].coord.x) && (players[i].coord.y == players[i+1].coord.y)) {
                players[i].coord.x = rand()%size;
                players[i].coord.y = rand()%size;
                matchCoord = true;
            }
        }
    }

    bool aliveEnemies = true;
    while (players[5].alive && aliveEnemies) {
        placement(playingField,players);

        const std::vector<std::string> actionVariables = {"left", "right", "top", "bottom", "save", "load"};
        const std::vector<std::string> mainMenu = {"save", "load"};
        std::string action;

        std::cout << "Enter one of the following:" << std::endl;
        for (int i=0;i < actionVariables.size();++i) {
            std::cout << "~" << actionVariables[i] << "~  ";
        }
        std::cout << std::endl << std::endl << "Or" << std::endl << std::endl;

        for (int i=0;i < mainMenu.size();++i) {
            std::cout << "~" << mainMenu[i] << "~  ";
        }
        std::cout << std::endl;

        bool find = false;
        int move;
        bool findMove = false;

        do {
            std::cin >> action;
            for (int i=0;i < actionVariables.size();++i) {
                if (action == actionVariables[i] || (i < 2 && action == mainMenu[i])) {
                    if (i < 4 && action == actionVariables[i]) {
                        move = i;
                        findMove = true;
                    }
                    find = true;
                    break;
                }
            }
            if (find == false) std::cout << "Incorrect action.Repeat:";
        } while (!find);

        if (!findMove) {
            for (int i=0;i < mainMenu.size();++i) {
                if (action == mainMenu[i]) {
                    (i == 0) ? save(players) : load(players);
                }
            }
        } else {
            if (check_motion(players[5], move)) {
                for (int i=0;i < players.size()-1; ++i) {
                    if (encounter(players[5], players[i])) {
                        turn_back(players[5], move);
                        damage_dealt(players[5], players[i]);
                    }
                }
            }

            int i;
            do {
                i = rand() % 4;
            } while (!players[i].alive);
            move = rand() % 4;
            if (check_motion(players[i], move)) {
                if (encounter(players[i], players[5])) {
                    turn_back(players[i], move);
                    damage_dealt(players[i], players[5]);
                }
            }
            for (int j=0;j < players.size()-1;++j) {
                if ((j != i) && encounter(players[i], players[j])) {
                    turn_back(players[i], move);
                }
            }
        }
        aliveEnemies = players[0].alive || players[1].alive || players[2].alive || players[3].alive || players[4].alive;
    }

    placement(playingField,players);
    if (players[5].alive) std::cout << "Player win!!!";
    else std::cout << "Enemies win.";

    return 0;
}
