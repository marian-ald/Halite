#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <set>
#include <fstream>


#include <stdio.h>
#include <iostream>

using namespace std;
#include "hlt.hpp"
#include "networking.hpp"

/*
  Pentru claritatea codului, fiecare aparitie a lui "unsigned short"
  respectiv "unsigned char" vor fi inlocuite cu "us" si "uc"
*/
typedef unsigned short us;
typedef unsigned char uc;

/*
    movesMatrix   -> retine mutarile efectuate pentru fiecare casuta
    map           -> harta jocului
    height, width -> dimensiunile hartii
    myID          -> id-ul botului
*/
int movesMatrix[51][51] = {0};
hlt::GameMap map;
int height, width;
uc myID;

 
// Se verifica daca la DREAPTA casutei de la pozitia l se afla o celula neutra
bool neutralToRight(hlt::Location l) {
    return (l.x > 0 && map.getSite({(us)(l.x - 1), l.y}).owner != myID);
}

// Se verifica daca la STANGA casutei de la pozitia l se afla o celula neutra
bool neutralToLeft(hlt::Location l) {
    return (l.x < width - 1 && map.getSite({(us)(l.x + 1), l.y}).owner != myID);
}

 
// Se verifica daca DEASUPRA casutei de la pozitia l se afla o celula neutra
bool neutralToUpper(hlt::Location l) {
    return (l.y < height - 1 && map.getSite({l.x, (us)(l.y + 1)}).owner != myID);
}

 
// Se verifica daca DEDESUBTUl casutei de la pozitia l se afla o celula neutra
bool neutralToLower(hlt::Location l) {
    return (l.y > 0 && map.getSite({l.x, (us)(l.y - 1)}).owner != myID);
}

// Se verifica daca pe granita din DREAPTA se afla o celula neutra
bool neutralOnRightBorder(hlt::Location l) {
    return (l.x == width - 1 && map.getSite({(us)(0), l.y}).owner != myID);
}

// Se verifica daca pe granita din STANGA se afla o celula neutra
bool neutralOnLeftBorder(hlt::Location l) {
    return (l.x == 0 && map.getSite({(us)(width - 1), l.y}).owner != myID);
}

// Se verifica daca pe granita de SUS se afla o celula neutra
bool neutralOnUpperBorder(hlt::Location l) {
    return (l.y == 0 && map.getSite({l.x, (us)(height - 1)}).owner != myID);
}

// Se verifica daca pe granita de JOS se afla o celula neutra
bool neutralOnLowerBorder(hlt::Location l) {
    return (l.y == height - 1 && map.getSite({l.x, (us)(0)}).owner != myID);
}

/* 
  Functia returneaza "true" daca in dreapta, josul, susul sau stanga celulei
  de la pozitia l se afla o celula neutra
*/
bool isNextToNeutral(hlt::Location l) {

    return (neutralToRight(l) || neutralToLower(l) ||
        neutralToUpper(l) || neutralToLeft(l));
}


/*
  Se verifica daca la NORDUL celulei l se afla o celula neutra. 
  Spre deosebire de functia "neutralToUpper()" de mai sus, se uita si "prin"
  granita.    
*/
bool isNextToNeutralNorth(hlt::Location l) {
    if (neutralToLower(l)) {
        return true;
    }
    if (neutralOnUpperBorder(l)) {
        return true;
    }
    return false;
}

// Se verifica daca la ESTUL celulei l se afla o celula neutra. 
bool isNextToNeutralEast(hlt::Location l) {
    if (neutralToLeft(l)) {
        return true;
    }
    if (neutralOnRightBorder(l)) {
        return true;
    }
    return false;
}

// Se verifica daca la SUDUL celulei l se afla o celula neutra. 
bool isNextToNeutralSouth(hlt::Location l) {
    if (neutralToUpper(l)) {
        return true;
    }
    if (neutralOnLowerBorder(l)) {
        return true;
    }
    return false;
}

// Se verifica daca la VESTUL celulei l se afla o celula neutra. 
bool isNextToNeutralWest(hlt::Location l) {
    if (neutralToRight(l)) {
        return true;
    }
    if (neutralOnLeftBorder(l)) {
        return true;
    }
    return false;
}

// Functia returneaza pozitia celulei neutre cu rezistenta minima
hlt::Location theWeakestNeutral(hlt::Location l) {

    int minimumStrength = 255;
    hlt::Location minimumLocation;

    if (neutralToUpper(l) &&  map.getSite({l.x, (us)(l.y + 1)}).strength < minimumStrength) {
        minimumStrength =  map.getSite({l.x, (us)(l.y + 1)}).strength;
        minimumLocation = hlt::Location({l.x, (us)(l.y + 1)});
    }
    if (neutralOnLowerBorder(l) &&  map.getSite({l.x, (us)(0)}).strength < minimumStrength) {
        minimumStrength =  map.getSite({l.x, (us)(0)}).strength;
        minimumLocation = hlt::Location({l.x, (us)(0)});
    }


    if (neutralToRight(l) && map.getSite({(us)(l.x - 1), l.y}).strength < minimumStrength) {
        minimumStrength = map.getSite({(us)(l.x - 1), l.y}).strength;
        minimumLocation = hlt::Location({(us)(l.x - 1), l.y});
    }
    if (neutralOnLeftBorder(l) && map.getSite({(us)(width - 1), l.y}).strength < minimumStrength) {
        minimumStrength = map.getSite({(us)(width - 1), l.y}).strength;
        minimumLocation = hlt::Location({(us)(width - 1), l.y});
    }


    if (neutralToLeft(l) && map.getSite({(us)(l.x + 1), l.y}).strength < minimumStrength) {
        minimumStrength = map.getSite({(us)(l.x + 1), l.y}).strength;
        minimumLocation = hlt::Location({(us)(l.x + 1), l.y});
    }
    if (neutralOnRightBorder(l) && map.getSite({(us)(0), l.y}).strength < minimumStrength) {
        minimumStrength = map.getSite({(us)(0), l.y}).strength;
        minimumLocation = hlt::Location({(us)(0), l.y});
    }


    if (neutralToLower(l) && map.getSite({l.x, (us)(l.y - 1)}).strength < minimumStrength) {
        minimumStrength = map.getSite({l.x, (us)(l.y - 1)}).strength;
        minimumLocation = hlt::Location({l.x, (us)(l.y - 1)});
    }
    if (neutralOnUpperBorder(l) && map.getSite({l.x, (us)(height - 1)}).strength < minimumStrength) {
        minimumStrength = map.getSite({l.x, (us)(height - 1)}).strength;
        minimumLocation = hlt::Location({l.x, (us)(height - 1)});
    }

    return minimumLocation;
}

/*
  Dandu-se o pozitie initiala, "l1" si una finala "l2", se va returna
  directia in care se va deplasa celula
*/
int changePosition(hlt::Location l1, hlt::Location l2) {
    int x = (int)l1.x - (int)l2.x;
    int y = (int)l1.y - (int)l2.y;

    if (x == 0 && y == 1) {
        return 1;
    }
    if (x == -1 && y == 0) {
        return 2;
    }
    if (x == 0 && y == -1) {
        return 3;
    }
    if (x == 1 && y == 0) {
        return 4;
    }
    return 5;
}

/* Dandu-se o locatie "l", se vor parcurge cele 4 directii in cautarea 
  distantei minime pana la prima celula neutra intalnita. Celula se va deplasa 
  ulterior pe directia unde a fost gasita distanta minima.
*/
int theShortestDistanceToNeutral(hlt::Location l) {
    us distanceNorth = 0;
    us distanceEast = 0;
    us distanceSouth = 0;
    us distanceWest = 0;
    hlt::Location seeker = hlt::Location({l.x, l.y});


    while (!isNextToNeutralNorth(seeker) && distanceNorth <= height) {
        if (seeker.y > 0)
            seeker = hlt::Location({seeker.x, (us)(seeker.y - 1)});
        else
            seeker = hlt::Location({seeker.x, (us)(height - 1)});
        distanceNorth++;
    }

    seeker = hlt::Location({l.x, l.y});
    while (!isNextToNeutralEast(seeker) && distanceEast <= width) {
        if (seeker.x < width - 1)
            seeker = hlt::Location({(us)(seeker.x + 1), seeker.y});
        else
            seeker = hlt::Location({0, seeker.y});
        distanceEast++;
    }

    seeker = hlt::Location({l.x, l.y});
    while (!isNextToNeutralSouth(seeker) && distanceSouth <= height) {
        if (seeker.y < height - 1)
            seeker = hlt::Location({seeker.x, (us)(seeker.y + 1)});
        else
            seeker = hlt::Location({seeker.x, 0});
        distanceSouth++;
    }

    seeker = hlt::Location({l.x, l.y});
    while (!isNextToNeutralWest(seeker) && distanceWest <= width) {
        if (seeker.x > 0)
            seeker = hlt::Location({(us)(seeker.x - 1), seeker.y});
        else
            seeker = hlt::Location({(us)(width - 1), seeker.y});
        distanceWest++;
    }

    // Daca pe cele 4 directii nu exista celule neutra, returnez STILL
    if(distanceNorth >= height - 1 && distanceSouth >= height - 1 && distanceWest >= width - 1 && distanceEast >= width - 1){
        movesMatrix[(int) (l.x)][(int) (l.y)] = 5;
        return 0;
    }

    // Daca distanta minima este spre nord, returnez NORTH
    if (distanceNorth <= distanceEast && distanceNorth <= distanceWest && distanceNorth <= distanceSouth) {
        movesMatrix[(int) (l.x)][(int) (l.y)] = 1;
        return 1;
    }

    // Daca distanta minima este spre est, returnez EAST
    if (distanceEast <= distanceNorth && distanceEast <= distanceWest && distanceEast <= distanceSouth) {
        movesMatrix[(int) (l.x)][(int) (l.y)] = 2;
        return 2;
    }

    // Daca distanta minima este spre sud, returnez SOUTH
    if (distanceSouth <= distanceNorth && distanceSouth<= distanceWest && distanceSouth <= distanceNorth) {
        movesMatrix[(int) (l.x)][(int) (l.y)] = 3;
        return 3;
    }

    // Daca distanta minima este spre vest, returnez WEST
    if (distanceWest <= distanceNorth && distanceWest <= distanceEast && distanceWest <= distanceSouth) {
        movesMatrix[(int) (l.x)][(int) (l.y)] = 4;
        return 4;
    }
}

int main() {
    srand(time(NULL));

    std::cout.sync_with_stdio(0);

    getInit(myID, map);
    sendInit("Boticelli");
    height = map.height;
    width = map.width;

    std::set<hlt::Move> moves;
    int ok = 0;

    while(true) {
        moves.clear();

        getFrame(map);

        // Parcurgere harta
        for(us a = 0; a < map.height; a++) {
            for(us b = 0; b < map.width; b++) {
                
                // Este a mea celula de la locatia {a, b}?
                if (map.getSite({ b, a }).owner == myID) {

                    // Daca puterea e 0, stau pe loc
                    if (map.getSite({ b, a }).strength == 0) {
                        moves.insert({ { b, a }, (uc)(0)});
                    } else {
                        // Este langa o celula neutra
                        if (isNextToNeutral(hlt::Location({b,a}))) {

                            // Calculez pozitia celulei neutre
                            hlt::Location loc = theWeakestNeutral(hlt::Location({b, a}));

                            // Am putere mai mare decat a celulei neutre
                            if ((int)map.getSite({loc.x, loc.y}).strength < (int)map.getSite({ b, a }).strength) {

                                // O atac
                                moves.insert({{ b, a }, (uc) abs((changePosition(hlt::Location({b, a}),
                                    hlt::Location({loc.x, loc.y}))))});

                            } else {

                                moves.insert({ { b, a }, (uc)(0)});
                            }
                        } else { // In jur am doar celule aliate

                            // Pentru putere mai mica decat 21 si valori de 5 in matrice, stau pe loc
                            if ((int)map.getSite({ b, a }).strength < 21) {
                                moves.insert({ { b, a }, (uc)(0)});
                            } else {
                                
                                if(movesMatrix[b][a] == 5){
                                    moves.insert({ { b, a }, (uc)(0)});
                                }
                                /* Pentru a nu face prea multe calcule la fiecare pas, pentru valoarea 
                                    ok = 0 calculez distantele doar pentru celulele de pe pozitiile pare
                                    si pentru ok = 1, distantele pentru celulele de pe pozitii impare
                                */
                                else {
                                    if (ok == 0) {
                                       if ((a + b) % 2 == 0)
                                            moves.insert({ { b, a }, (uc)(theShortestDistanceToNeutral(hlt::Location({ b, a })))});
                                        else
                                            moves.insert({ { b, a }, (uc)(movesMatrix[b][a])});
                                    }
                                    if (ok == 1) {
                                        if ((a + b) % 2 == 1)
                                            moves.insert({ { b, a }, (uc)(theShortestDistanceToNeutral(hlt::Location({ b, a })))});
                                        else
                                            moves.insert({ { b, a }, (uc)(movesMatrix[b][a])});
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (ok == 0)
            ok = 1;
        else
            ok = 0;

        sendFrame(moves);
    }

    return 0;
}
