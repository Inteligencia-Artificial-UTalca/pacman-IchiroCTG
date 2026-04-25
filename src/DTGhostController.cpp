#include "DTGhostController.h"
#include "Ghost.h"
#include <limits>

DTGhostController::DTGhostController(std::shared_ptr<Character> character)
    : Controller(character) {}

DTGhostController::~DTGhostController() {}

Move DTGhostController::getMove(const GameState& game) {
    const Maze& maze = game.getMaze();

    int ghostPos  = character->getPos();
    Move lastMove = character->getDirection();

    Ghost* ghost = dynamic_cast<Ghost*>(character.get());
    bool edible  = ghost && ghost->isEdible();

    std::pair<int,int> pacmanCoord = maze.getNodePos(game.getPacmanPos());

    // ← CLAVE: igual que BTGhostController y SimpleController
    std::vector<Move> legalMoves;
    if (lastMove == PASS) {
        legalMoves = maze.getPossibleMoves(ghostPos);
    } else {
        legalMoves = maze.getGhostLegalMoves(ghostPos, lastMove);
    }

    if (legalMoves.empty()) return lastMove;

    Move  bestMove = legalMoves[0];
    float bestDist = -1.0f;

    for (Move m : legalMoves) {
        int neighborIdx = maze.getNeighbour(ghostPos, m);
        if (neighborIdx < 0) continue;

        std::pair<int,int> nPos = maze.getNodePos(neighborIdx);
        float dist = euclid2(nPos, pacmanCoord);

        if (edible) {
            if (dist > bestDist) {
                bestDist = dist;
                bestMove = m;
            }
        } else {
            if (bestDist < 0 || dist < bestDist) {
                bestDist = dist;
                bestMove = m;
            }
        }
    }

    return bestMove;
}