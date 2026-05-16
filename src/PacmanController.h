#pragma once

#include "Controller.h"
using namespace std;

class PacmanController: public Controller {

	float CalcularDistancia(pair<int,int> PacmanCords, pair<int,int>GhostCords) const;
	Move AcercarsePoint(const GameState& game, std::pair<int,int> target) const;
	Move AlejarsePoint(const GameState& game, std::pair<int,int> target) const;

public:
	PacmanController(std::shared_ptr<Character> character);
	virtual ~PacmanController();
    virtual Move getMove(const GameState &game) override;
};

