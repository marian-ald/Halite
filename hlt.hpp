#ifndef HLT_H
#define HLT_H

#include <list>
#include <vector>
#include <random>

#define STILL 0
#define NORTH 1
#define EAST 2
#define SOUTH 3
#define WEST 4

const int DIRECTIONS[] = {STILL, NORTH, EAST, SOUTH, WEST};
const int CARDINALS[] = {NORTH, EAST, SOUTH, WEST};

namespace hlt{
    /*
      Coordonatele pe harta
    */
    struct Location{
        unsigned short x, y;
    };

    /*
      Operator "<" supraincarcat pentru compararea a doua locatii de pe harta
    */
    static bool operator<(const Location& l1, const Location& l2) {
        return ((l1.x + l1.y)*((unsigned int)l1.x + l1.y + 1) / 2) + l1.y < ((l2.x + l2.y)*((unsigned int)l2.x + l2.y + 1) / 2) + l2.y;
    }

    /* 
      Un patratel de pe harta caracterizat de: detinator(initial e nutru),
      putere si putere de productie.
    */
    struct Site{
        unsigned char owner;
        unsigned char strength;
        unsigned char production;

        void operator=(Site s){
            owner = s.owner;
            strength = s.strength;
            production = s.production;
        }
    };


    class GameMap{
    public:
        // O matrice care contine patratelele
        std::vector< std::vector<Site> > contents;
        
        //Number of rows & columns, NOT maximum index.
        unsigned short width, height;

        /* 
          Constructor default
        */
        GameMap() {
            width = 0;
            height = 0;
            contents = std::vector< std::vector<Site> >(height, std::vector<Site>(width, { 0, 0, 0 }));
        }

        /*
          Constructor cu parametri(se primeste o alta harta)
        */
        GameMap(const GameMap &otherMap) {
            width = otherMap.width;
            height = otherMap.height;
            contents = otherMap.contents;
        }

        /*
          Constructor cu parametri(se primesc dimensiunile)
        */
        GameMap(int w, int h) {
            width = w;
            height = h;
            contents = std::vector< std::vector<Site> >(height, std::vector<Site>(width, { 0, 0, 0 }));
        }

        /*
          Casuta l se afla in interiorul hartii
        */
        bool inBounds(Location l) {
            return l.x < width && l.y < height;
        }

        /*
            Distanta dintre doua casute. Nu se calculeaza distanta
            pe ipotenuza cu Pitagora, ci se aduna cele 2 catete.
        */    
        float getDistance(Location l1, Location l2) {
            short dx = abs(l1.x - l2.x), dy = abs(l1.y - l2.y);
            if(dx > width / 2) dx = width - dx;
            if(dy > height / 2) dy = height - dy;
            return dx + dy;
        }

        /*
          Se calculeaza un unghi. Doamne ajuta ce ne-o folosi.
        */
        float getAngle(Location l1, Location l2) {
            short dx = l2.x - l1.x, dy = l2.y - l1.y;
            if(dx > width - dx) dx -= width;
            else if(-dx > width + dx) dx += width;
            if(dy > height - dy) dy -= height;
            else if(-dy > height + dy) dy += height;
            return atan2(dy, dx);
        }

        /*
          Dupa ce se efectueaza o mutare, se returneaza noua locatie
        */
        Location getLocation(Location l, unsigned char direction) {
            if(direction != STILL) {
                if(direction == NORTH) {
                    if(l.y == 0) l.y = height - 1;
                    else l.y--;
                }
                else if(direction == EAST) {
                    if(l.x == width - 1) l.x = 0;
                    else l.x++;
                }
                else if(direction == SOUTH) {
                    if(l.y == height - 1) l.y = 0;
                    else l.y++;
                }
                else if(direction == WEST) {
                    if(l.x == 0) l.x = width - 1;
                    else l.x--;
                }
            }
            return l;
        }

        /*
          Dupa ce se efectueaza o mutare, se returneaza informatia patratului
        */
        Site& getSite(Location l, unsigned char direction = STILL) {
            l = getLocation(l, direction);
            return contents[l.y][l.x];
        }
    };

    /*
      Reprezinta o mutare care va fi transmisa.
      Contine locatia initiala, respectiv directia in care se deplaseaza
    */
    struct Move{
        Location loc; unsigned char dir;
    };

    /*
      Comparator pentru 2 mutari. Se procedeaza asemanator ca la 
      "<" supraincarcat pentru Location
    */
    static bool operator<(const Move& m1, const Move& m2) {
        unsigned int l1Prod = ((m1.loc.x + m1.loc.y)*((unsigned int)m1.loc.x + m1.loc.y + 1) / 2) + m1.loc.y,
                     l2Prod = ((m2.loc.x + m2.loc.y)*((unsigned int)m2.loc.x + m2.loc.y + 1) / 2) + m2.loc.y;

        return ((l1Prod + m1.dir)*(l1Prod + m1.dir + 1) / 2) + m1.dir < ((l2Prod + m2.dir)*(l2Prod + m2.dir + 1) / 2) + m2.dir;
    }
}

#endif