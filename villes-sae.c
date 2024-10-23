#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

// define Types

typedef struct City
{
    int code;
    float latitude, longitude;
    char name[80];
}City;

typedef struct Tab_Cities
{
    int nb_cities;
    City *cities;
}Tab_Cities;

// define Functions & Procedures

void load_from_csv(const char *file_path, Tab_Cities *tab)
{
    FILE *csv = NULL;
    csv = fopen("cities.csv", "r");
    if (csv != NULL)
    {
        tab->cities = realloc(tab->cities, sizeof(tab->cities)+sizeof(City));
        tab->nb_cities += 1;
    }
    else
    {
        printf("Error : can't open file %s\n", file_path);
    }
}

void save_csv(const char *file_path, Tab_Cities *tab)
{
    FILE *csv = NULL;
    free(tab->cities);
}

void add_city(Tab_Cities *tab)
{
    ;
}

void del_city(Tab_Cities *tab)
{
    ;
}

void chg_city(Tab_Cities *tab)
{
    ;
}

void print_cities(Tab_Cities *tab)
{
    printf("CODE / VILLE / Latitude / Longitude\n");
    for (int i = 0; i < tab->nb_cities; i++)
    {
        printf("%d / %s / %f / %f\n", tab->cities[i].code, tab->cities[i].name, tab->cities[i].latitude, tab->cities[i].longitude);
    }
}

void show_help(void)
{
    printf("--------------------------------------------------------------\n");
    printf("Commandes disponibles :\n");
    printf("h - affiche cette liste de commandes\n");
    printf("p - affiche les données de toutes les villes\n");
    printf("a - permet d'ajouter une ville\n");
    printf("s - permet de supprimer une ville\n");
    printf("m - permet de modifier une ville\n");
    printf("c - permet d'afficher les coordonnées GPS d'une ville\n");
    printf("w - enregistre les modifications et quitte le programme\n");
    printf("q - quitte le programme en abandonnant les modifications\n");
    printf("--------------------------------------------------------------\n");
}

bool execute_command(char cmd, bool *exit, Tab_Cities *tab_cities)
{
    switch (cmd)
    {
        case 'h':
        {
            show_help();
            break;
        }
        case 'p':
        {
            print_cities(tab_cities);
            break;
        }
        case 'a':
        {
            add_city(tab_cities);
            break;
        }
        case 's':
        {
            del_city(tab_cities);
            break;
        }
        case 'm':
        {
            chg_city(tab_cities);
            break;
        }
        case 'c':
        {
            break;
        }
        case 'w':
        {
            save_csv("cities.csv", tab_cities);
            *exit = true;
            break;
        }
        case 'q':
        {
            free(tab_cities->cities);
            *exit = true;
            break;
        }
        default:
        {
            printf("Commande non reconnue : saisissez 'h' pour afficher la liste des commandes valides\n");
            break;
        }
    }
}

// define Variables

Tab_Cities tab_cities;
char command;
bool quit;

// main Function

int main(void)
{
    // initializing variables

    tab_cities.nb_cities = 0;
    tab_cities.cities = NULL;
    quit = false;

    // loading data & showing commands

    load_from_csv("cities.csv", &tab_cities);
    show_help();
    
    // main loop

    while (!quit)
    {
        scanf("%c", &command);
        execute_command(command, &quit, &tab_cities);
    }

    // quitting program

    return EXIT_SUCCESS;
}
