#include <stdio.h>
#include <stdlib.h>
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

// define Functions

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

void print_cities(Tab_Cities *cities)
{
    printf("CODE / VILLE / Latitude / Longitude\n");
    for (int i = 0; i < cities->nb_cities; i++)
    {
        printf("%d / %s / %f / %f\n", cities->cities[i].code, cities->cities[i].name, cities->cities[i].latitude, cities->cities[i].longitude);
    }
}

void show_help(void)
{
    printf("Commandes disponibles :\n");
    printf("h - affiche cette liste de commandes\n");
    printf("p - affiche les données de toutes les villes\n");
    printf("a - permet d'ajouter une ville\n");
    printf("s - permet de supprimer une ville\n");
    printf("m - permet de modifier une ville\n");
    printf("c - permet d'afficher les coordonnées GPS d'une ville\n");
    printf("w - enregistre les modifications et quitte le programme\n");
    printf("q - quitte le programme en abandonnant les modifications\n");
}

// define Variables

Tab_Cities tab_cities = {.nb_cities = 0, .cities = NULL};
char command;

// main Function

int main(void)
{
    load_from_csv("cities.csv", &tab_cities);
    show_help();
    scanf("%c", &command);
    switch (command)
    {
        case 'h':
        {
            show_help();
            break;
        }
        case 'p':
        {
            print_cities(&tab_cities);
            break;
        }
        case 'a':
        {
            break;
        }
        default:
        {
            break;
        }
    }

    save_csv("cities.csv", &tab_cities);
    return EXIT_SUCCESS;
}
