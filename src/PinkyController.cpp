/*
 * PinkyController.cpp
 */

#include "PinkyController.h"
#include "BTGhostController.h"

#define MIN 100000

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

Move PinkyController::getMove(const GameState& game) {
    Info::getInfo()->in_character = character;
    Info::getInfo()->in_gamestate = &game;
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


//  Huida movimientos aleatorios
PinkyFrightened::PinkyFrightened(): Behavior(), e(rand()),uniform_dist(0,3){

}

Status PinkyFrightened::update() {
    auto character = Info::getInfo()->in_character;
    auto gs = Info::getInfo()->in_gamestate;

    std::vector<Move> moves = gs->getMaze().getGhostLegalMoves(character->getPos(),character->getDirection());
    bool Valido=false;
    for(auto movimiento : moves){
        if(movimiento != PASS){
            Valido = true;
            break; 
        }
    }
    if(!Valido){moves= gs->getMaze().getPossibleMoves(character->getPos());}


    Info::getInfo()->out_move = moves[rand() % moves.size()];
    return BH_SUCCESS;
}

// Dispersion para dirigirse a una esquina durante 7 segundos
PinkyScatter::PinkyScatter() : Behavior() {
    target = std::make_pair(-1, -1);
}

Status PinkyScatter::update() {
    if (target == std::pair{-1,-1}) {
        auto SuperPastillas = Info::getInfo()->in_gamestate->getMaze().getPowerPillPositions();
        target = SuperPastillas[0];
    }

    auto character = Info::getInfo()->in_character;
    auto gs = Info::getInfo()->in_gamestate;

    std::vector<Move> moves = gs->getMaze().getGhostLegalMoves(character->getPos(),character->getDirection());
    bool Valido=false;
    for(auto movimiento : moves){
        if(movimiento != PASS){
            Valido = true;
            break; 
        }
    }
    if(!Valido){moves= gs->getMaze().getPossibleMoves(character->getPos());}



    float DistanciaMinima = MIN;
    Move  minMove = PASS;
    for (auto Movimiento : moves) {
        if (Movimiento == PASS) break;
        float Distancia = euclid2(
            target,
            gs->getMaze().getNodePos(
                gs->getMaze().getNeighbour(character->getPos(), Movimiento)
            )
        );
        if (Distancia < DistanciaMinima) { DistanciaMinima = Distancia; minMove = Movimiento; }
    }

    if (minMove == PASS && !moves.empty()) minMove = moves[0];
    Info::getInfo()->out_move = minMove;
    return BH_SUCCESS;
}

// Pinky Chase para cortarle el camino a Pacman
Status PinkyChase::update() {
    auto Personaje = Info::getInfo()->in_character;
    auto gs = Info::getInfo()->in_gamestate;

    // Revisamos posicion de Pacman
    int  PosicionPacman = gs->getPacmanPos();
    Move PacmanDirecciones = static_cast<Move>(gs->getPacmanDir());

    // calculamos el movimiento de pacman hacia 4 pasos adelante

    int PosicionObjetivo = PosicionPacman;
    if (PacmanDirecciones != PASS) {
        for (int i = 0; i < 4; i++) {
            int siguiente = gs->getMaze().getNeighbour(PosicionObjetivo, PacmanDirecciones);
            if (siguiente == -1) break;
            auto siguienteMovimiento = gs->getMaze().getPossibleMoves(siguiente);
            bool esValido = false;
            for (auto nm : siguienteMovimiento){
             if (nm != PASS) { 
                esValido = true; 
                break; 
                }
            }
            if (!esValido) break;
            PosicionObjetivo = siguiente;
        }
    }
    auto target = gs->getMaze().getNodePos(PosicionObjetivo);

    // Movimiento del Fantasma
    std::vector<Move> moves = gs->getMaze().getGhostLegalMoves(Personaje->getPos(),Personaje->getDirection());
    bool Valido=false;
    for(auto movimiento : moves){
        if(movimiento != PASS){
            Valido = true;
            break; 
        }
    }
    if(!Valido){moves= gs->getMaze().getPossibleMoves(Personaje->getPos());}

    // Movimiento hacia Pacman
    float DistanciaMinima = MIN;
    Move  minMove = PASS;
    for (auto Movimiento : moves) {
        if (Movimiento == PASS) break;
        float Distancia = euclid2(
            target,
            gs->getMaze().getNodePos(
                gs->getMaze().getNeighbour(Personaje->getPos(), Movimiento)
            )
        );
        if (Distancia < DistanciaMinima) { 
            DistanciaMinima = Distancia; 
            minMove = Movimiento; 
        }
    }

    if (minMove == PASS && !moves.empty()) minMove = moves[0];
    Info::getInfo()->out_move = minMove;
    return BH_SUCCESS;
}