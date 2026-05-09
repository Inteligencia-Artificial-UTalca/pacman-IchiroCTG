/*
 * PinkyController.cpp
 */

#include "PinkyController.h"
#include "BTGhostController.h"

PinkyController::PinkyController(std::shared_ptr<Character> character)
    : Controller(character), root(std::make_shared<Selector>())
{
    auto filter = std::make_shared<Filter>();
    filter->addCondition(std::make_shared<PinkyPowerpill>());
    filter->addAction(std::make_shared<PinkyFrightened>());
    root->addChild(filter);

    auto filter2 = std::make_shared<Filter>();
    filter2->addCondition(std::make_shared<PinkyTimeOut>());
    filter2->addAction(std::make_shared<PinkyScatter>());
    root->addChild(filter2);

    root->addChild(std::make_shared<PinkyChase>());
}

PinkyController::~PinkyController() {}

Move PinkyController::getMove(const GameState& gs) {
    Info::getInfo()->in_character = character;
    Info::getInfo()->in_gamestate = &gs;
    root->tick();
    return Info::getInfo()->out_move;
}

//Condicion al Consumir una Powerpill
Status PinkyPowerpill::update() {
    auto character = Info::getInfo()->in_character;
    auto ghost = dynamic_cast<Ghost*>(character.get());
    if (ghost != nullptr && ghost->isEdible()) {
        return BH_SUCCESS;
    }
    return BH_FAILURE;
}

// Ciclo de 27 segundos Condicion
PinkyTimeOut::PinkyTimeOut() : Behavior() {
    lastTime = std::chrono::high_resolution_clock::now();
}

Status PinkyTimeOut::update() {
    std::chrono::duration<float> timeStamp =
        std::chrono::high_resolution_clock::now() - lastTime;
    if ((int)timeStamp.count() % 27 < 7) {
        return BH_SUCCESS;
    }
    return BH_FAILURE;
}


//  Movimiento aleatorio cuando Pac-Man tiene powerpill
Status PinkyFrightened::update() {
    auto character = Info::getInfo()->in_character;
    auto gs = Info::getInfo()->in_gamestate;

    std::vector<Move> moves;
    if (character->getDirection() == PASS) {
        moves = gs->getMaze().getPossibleMoves(character->getPos());
    } else {
        moves = gs->getMaze().getGhostLegalMoves(
            character->getPos(), character->getDirection()
        );
    }
    Info::getInfo()->out_move = moves[rand() % moves.size()];
    return BH_SUCCESS;
}

// Pinky Scatter para dirigirse directamente a la esquina los primeros 7 segundos del ciclo
PinkyScatter::PinkyScatter() : Behavior() {
    target = std::make_pair(-1, -1);
}

Status PinkyScatter::update() {
    if (target.first == -1) {
        auto pills = Info::getInfo()->in_gamestate->getMaze().getPowerPillPositions();
        target = pills[0];
    }

    auto character = Info::getInfo()->in_character;
    auto gs        = Info::getInfo()->in_gamestate;

    std::vector<Move> moves;
    if (character->getDirection() == PASS) {
        moves = gs->getMaze().getPossibleMoves(character->getPos());
    } else {
        moves = gs->getMaze().getGhostLegalMoves(
            character->getPos(), character->getDirection()
        );
        bool hasValid = false;
        for (auto m : moves) if (m != PASS) { hasValid = true; break; }
        if (!hasValid) moves = gs->getMaze().getPossibleMoves(character->getPos());
    }

    float min = 100000000;
    Move  minMove = PASS;
    for (auto move : moves) {
        if (move == PASS) break;
        float dist = euclid2(
            target,
            gs->getMaze().getNodePos(
                gs->getMaze().getNeighbour(character->getPos(), move)
            )
        );
        if (dist < min) { min = dist; minMove = move; }
    }

    if (minMove == PASS && !moves.empty()) minMove = moves[0];
    Info::getInfo()->out_move = minMove;
    return BH_SUCCESS;
}

// Pinky Chase para cortarle el camino a Pacman
Status PinkyChase::update() {
    auto character = Info::getInfo()->in_character;
    auto gs = Info::getInfo()->in_gamestate;

    // Revisamos posicion de Pacman
    int  pacPos = gs->getPacmanPos();
    Move pacDir = static_cast<Move>(gs->getPacmanDir());

    // calculamos el movimiento de pacman hacia 4 pasos adelante

    int targetPos = pacPos;
    if (pacDir != PASS) {
        for (int i = 0; i < 4; i++) {
            int next = gs->getMaze().getNeighbour(targetPos, pacDir);
            if (next == -1) break;
            auto nextMoves = gs->getMaze().getPossibleMoves(next);
            bool hasValid = false;
            for (auto nm : nextMoves) if (nm != PASS) { hasValid = true; break; }
            if (!hasValid) break;
            targetPos = next;
        }
    }
    auto target = gs->getMaze().getNodePos(targetPos);

    // revision movimientos legales con fallback
    std::vector<Move> moves;
    if (character->getDirection() == PASS) {
        moves = gs->getMaze().getPossibleMoves(character->getPos());
    } else {
        moves = gs->getMaze().getGhostLegalMoves(
            character->getPos(), character->getDirection()
        );
        bool hasValid = false;
        for (auto m : moves) if (m != PASS) { hasValid = true; break; }
        if (!hasValid) moves = gs->getMaze().getPossibleMoves(character->getPos());
    }

    // moverse hacia el movimiento de pacman predicho
    float min = 1000000000;
    Move  minMove = PASS;
    for (auto move : moves) {
        if (move == PASS) break;
        float dist = euclid2(
            target,
            gs->getMaze().getNodePos(
                gs->getMaze().getNeighbour(character->getPos(), move)
            )
        );
        if (dist < min) { min = dist; minMove = move; }
    }

    if (minMove == PASS && !moves.empty()) minMove = moves[0];
    Info::getInfo()->out_move = minMove;
    return BH_SUCCESS;
}