/**
 * IZU projekt 2012 
 * 
 * File:     main.c
 *
 * Author:   Horacek (ihoracek@fit.vutbr.cz) 
 *           Malacka (imalacka@fit.vutbr.cz)
 *           Samek (samejan@fit.vutbr.cz)
 *
 * Revision: $Id: main.c 6 2012-03-18 19:58:49Z samek $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "queue.h"

#define M_PI 3.14159265358979323846264338327950288
#define d2r (M_PI / 180.0)

#define MAX_SPEED 130.0

double g_full = 0;
double g_prime = 0;
int g_founded = 0;

/***
 * Struktura ktera urcuje polohu mesta (uzel)
 *
 * name     ukazatel na nazev mesta
 * lat      zemepisna sirka
 * lon      zemepisna delka
 * g        ohodnoceni stavu od pocateniho uzlu
 * h        heuristicka funkce
 * inQueue  priznak pro rozgenerovani
 * path     cesta
 */
typedef struct {
    char* name;
    double lat;
    double lon;
    double g;
    double h;
    int inQueue;
    queueItem *path;
} city;

/***
 * Struktura ktera urcuje spojnici mezi mesty (hranu)
 *
 * city1     nazev mesta 1
 * city2     nazev mesta 2
 * distance  vzdalenost
 * max_speed maximalni rychlost na silnici
 */
typedef struct {
    int city1;
    int city2;
    int distance;
    int max_speed;
} road;

/**
 * Globalni promenne pro reprezentaci grafu (mesta + spojnice)
 */
city** cities;
int    cities_count = 0; // pocet mest v promenne cities

road** roads;
int    roads_count = 0;  // pocet cest v promenne roads

/**
 * Vytiskne posloupnost mest na zaklade predane datove struktury
 *
 * @param mestaId
 */
void printRoute(queueItem *mestaId) {
    if (mestaId == NULL) {
        fprintf(stderr,"Cesta neobsahuje zadna mesta!\n");
    } else {
        //printf("Vysledna cesta: ");
        while (mestaId != NULL) {
            printf("%s", cities[mestaId->dataitem]->name);
            mestaId = mestaId->link;
            if (mestaId != NULL) {
                printf(",");
            }
        }
        printf("\n");
    }
}

/**
 * Spocita primou (leteckou) vzdalenost mezi mesty pomoci jejich GPS souradnic
 * 
 * @param lat1   z.sirka mesta 1
 * @param long1  z.delka mesta 1
 * @param lat2   z.sirka mesta 2
 * @param long2  z.delka mesta 2
 * @return       vzdalenost v [km]
 */
double distance(double lat1, double long1, double lat2, double long2) {
    double dlong = (long2 - long1) * d2r;
    double dlat = (lat2 - lat1) * d2r;
    double a = pow(sin(dlat / 2.0), 2) + cos(lat1 * d2r) * cos(lat2 * d2r) * pow(sin(dlong / 2.0), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double d = 6367 * c;
    return d;
}

/**
 * Funkce, ktera vrati vzdalenost mezi mesty from a to (ktera jsou zadana pomoci jejich ID)
 *
 * @param from  ID mesta 1
 * @param to    ID mesta 2
 * @retun       vzdalenost v [km]
 */
double getDistance(int from, int to) {
    return distance(cities[from]->lat, cities[from]->lon, cities[to]->lat, cities[to]->lon);
}

/**
 * Vrati ID (index) mesta v grafu dle jeho nazvu
 *
 * @param name jmeno mesta
 * @return     ID (index) mesta, v pripade ze mesto neexistuje vrati -1
 */
int gedIdByName(char *name) {
    int i;
    for (i = 0; i < cities_count; i++) {
        if (strcmp(cities[i]->name, name) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * Vrati nazev mesta dle jeho ID (indexu) v grafu
 *
 * @param id ID (index) mesta
 * @return   jmeno mesta, v pripade ze ID neexistuje vrati NULL
 */
char* getNameById(int id) {
    if (cities_count > id && id >= 0) {
        return cities[id]->name;
    }
    return NULL;
}

/***
 * Nacteni datove struktury grafu ze souboru
 *
 * @param file nazev souboru s definici grafu
 * @return     pocet zpracovanych radku souboru (pocet mest + pocet cest)
 */
int loadCitiesAndRoads(char *file) {
    FILE *f = fopen(file, "r");
    char line[256]; // max. delka radku omezena na 255
    int count = 0;
    while (fgets(line, sizeof (line), f) && line[0] != '-') {
        cities = (city **) realloc(cities, (count + 1) * sizeof (city *));
        char* pom = strtok(line, ",");
        char* city1 = (char *) malloc(sizeof (char) * (strlen(pom) + 1));
        strcpy(city1, pom);
        cities[count] = (city *) malloc(sizeof (city));
        cities[count]->name = city1;
        cities[count]->lat = atof(strtok(NULL, ","));
        cities[count]->lon = atof(strtok(NULL, ","));
        cities[count]->g = -1.0;
        cities[count]->h = 0.0;
        cities[count]->inQueue = 0;
        cities[count]->path = NULL;
        count++;
    }
    cities_count = count;
    count = 0;
    while (fgets(line, sizeof(line), f)) {
        roads = (road **) realloc(roads, (count + 1) * sizeof (road *));
        roads[count] = (road *) malloc(sizeof (road));
        roads[count]->city1 = gedIdByName(strtok(line, ","));
        roads[count]->city2 = gedIdByName(strtok(NULL, ","));
        roads[count]->distance = atoi(strtok(NULL, ","));
        roads[count]->max_speed = atoi(strtok(NULL, ","));
        count++;
    }
    roads_count = count;
    fclose(f);
    return (cities_count + roads_count);
}

// add path to city
void addPath(city *temp,city *start)
{
    // initialize/clear path
    clearQueue(temp->path);
    temp->path = NULL;

    // add path from previous city
    queueItem *path = start->path;
    while(path != NULL)
    {
        temp->path = addItem(temp->path,path->dataitem);
        path = path->link;
    }

    // add name of actual city to path
    temp->path = addItem(temp->path,gedIdByName(temp->name));    
}

/**
 * Vyhleda cestu pomoci algoritmu A*
 * !! TUTO FUNKCI IMPLEMENTUJTE !!
 *
 * @param from ID mesta 1
 * @param to   ID mesta 2
 */
void findRoute(int from, int to) {
    // pro ulozeni vysledne cesty muzete pouzit polozku 'path' ve strukture 'city'
    // - vyslednou cestu je treba vypsat take ve funkci 'main'!

    // Author: Michal Lukac, xlukac05@stud.fit.vutbr.cz
    // Subject: project for IZU at BUT FIT, 2011/2012
    // start item or actual item
    city *start = cities[from];
    start->g = 0;
    start->inQueue = 1;
    start->path = addItem(start->path,gedIdByName(start->name));

    // temporary city
    city *temp = NULL;
    int temp_g = 0;

    // choosing city with min f(x)
    int state = 0;

    g_prime = start->h;

    // A* algorithm cycle
    while(start->inQueue != -1)
    {
        // go throug all neighbour cities
        int id1 = gedIdByName(start->name);
        for(int i = 0; i < roads_count; i++)
        {
            // clear the temp
            temp = NULL;

            // get all city1 roads and get city1 or city2
            if(roads[i]->city2 == id1)
                temp = cities[roads[i]->city1];
            else if(roads[i]->city1 == id1)
                temp = cities[roads[i]->city2];

            // if city was founded
            if(temp != NULL)
            {
                // if city has better g(x)
                if(temp->g < ((((double)roads[i]->distance)/(double)roads[i]->max_speed) + start->g)) temp_g = 0;
                else temp_g = 1;

                // if city has better g(x) or is not yet computed
                if(temp->g == -1 || temp_g)
                {
                    temp->g = start->g + (((double)roads[i]->distance)/(double)roads[i]->max_speed);
                    temp->inQueue = 1;

                    // create path
                    addPath(temp,start);
                }
            }
        }
        // end of for cycle
        start->inQueue = -1;
        state = 0;

        // get new city with minimal value f(x) = g(x) + h(x)
        for(int i = 0; i < cities_count; i++)
        {
            // get minimum
            if(cities[i]->inQueue == 1 && state == 1)
            {
                // if we had a better f(x)
                if((start->g + start->h) > (cities[i]->g + cities[i]->h))
                    start = cities[i];
            }
            else if(cities[i]->inQueue == 1 && state == 0)
            {
                state = 1;
                start = cities[i];
            }
        }

        // if founded, end function
        if(start->h == 0)
        {
            g_full = start->g;
            g_founded = 1;
            return;
        }
    }
}

/**
 * Vypocet heuristicke funkce h(f) na zaklade vzdalenost mezi mesty a maximalni rychlosti
 * - jedna se o vzdalenost vzdusnou carou pri uvazeni MAX_SPEED (130 km/h)
 *
 * @param to ID ciloveho mesta
 */
void computeHeuristic(int to) {
    for (int i = 0; i < cities_count; i++) {
        cities[i]->h = getDistance(i, to) / MAX_SPEED;
    }
}

int main(int argc, char *argv[]) {

    char *map_filename;
    char *city_from;
    char *city_to;

    // zpracovani parametru prikazove radky
    switch (argc) {
        case 3:
            // argv[1] - city from
            // argv[2] - city to
            // soubor s mesty: implicitne "mesta.txt"
            map_filename = "mesta.txt";
            city_from = argv[1];
            city_to = argv[2];
            break;

        case 4:
            // argv[1] - soubor s mesty
            // argv[2] - city from
            // argv[3] - city to
            map_filename = argv[1];
            city_from = argv[2];
            city_to = argv[3];
            break;

        default:
            printf("Neplatne parametry prikazoveho radku!\n");
            return 1;
            break;
    }

    // overeni vstupniho souboru
    FILE *fr;
    if ((fr = fopen(map_filename, "r")) == NULL) {
        printf("Nelze otevrit vstupni soubor mapy '%s'!\n", map_filename);
        return 2;
    }
    fclose(fr);

    // nacteni vstupniho souboru
    int loaded = loadCitiesAndRoads(map_filename);
    if (!loaded) {
        printf("Mapovy soubor '%s' neobsahuje platna data!\n", map_filename);
        return 3;
    }

    // overeni parametru mest
    int from = gedIdByName(city_from);
    int to = gedIdByName(city_to);
    int city_error = 0;
    if (from < 0) {
        printf("Pocatecni mesto '%s' neexistuje!\n", city_from);
        city_error++;
    }
    if (to < 0) {
        printf("Cilove mesto '%s' neexistuje!\n", city_to);
        city_error++;
    }
    if (city_error)
        return 4;

    // nalezeni cesty
    computeHeuristic(to);
    findRoute(from, to);
    // --> zde doplnte vypis nalezene cesty
	printRoute(cities[to]->path);
    /*if(g_full)
    {
        printf("Celkovy cas nalezene cesty: %.1lf hodin\n",g_full);
        printf("Cas vzdusnou carou (pri 130 km/h): %.1lf hodin\n",g_prime);
    }*/
    // <-- 


    // uvolneni pameti
    for (int i = 0; i < cities_count; i++) {
        clearQueue(cities[i]->path);
        xfree(cities[i]->name);
        xfree(cities[i]);
    }

    for (int i = 0; i < roads_count; i++) {
        xfree(roads[i]);
    }
    xfree(cities);
    xfree(roads);

    return 0;
}
