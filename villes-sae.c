#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

// define Types

typedef struct City
{
    int code;
    float latitude, longitude;
    char name[85];
    struct City *next;
}City;

// define Functions & Procedures

void free_City(City *city, City *list)
{
    City *current = list;
    City *tmp = NULL;
    if (city == NULL)
    {
        printf("Error : invalid argument for 'free_City'\nProceed anyway\n");
        return;
    }
    while (current->next != city)
    {
        current = current->next;
    }
    tmp = current->next->next;
    free(current->next);
    current->next = tmp;    
}

void free_Cities(City *list)
{
    while (list->next != NULL)
    {
        free_City(list->next, list);
    }
}

void load_from_csv(const char *file_path, City *list)
{
    FILE *csv = NULL;
    City *current = NULL;
    int nb_of_scan;

    if (list->next != NULL)
    {
        free_Cities(list);
    }
    current = list;
    
    csv = fopen(file_path, "r");
    if (csv == NULL)
    {
        printf("Error : can't open file '%s'\n", file_path);
        exit(EXIT_FAILURE);
    }
    
    fscanf(csv, "CODE,VILLE,LATITUDE,LONGITUDE");
    do
    {
        current->next = malloc(sizeof(City));
        if (current->next == NULL)
        {
            printf("Error : out of memory\n");
            free_Cities(list);
            fclose(csv);
            exit(EXIT_FAILURE);
        }
        current = current->next;
        nb_of_scan = fscanf(csv, "%d,%[^,],%f,%f", &current->code, current->name, &current->latitude, &current->longitude);
    } while (nb_of_scan != EOF);
    free_City(current, list);
    fclose(csv);
}

void save_csv(const char *file_path, City *list)
{
    FILE *csv = NULL;
    free_Cities(list);
}

void add_city(City *list)
{
    City *last = list;
    while (last->next != NULL)
    {
        last = last->next;
    }
    printf("Veuillez renseigner les données de la ville à ajouter (format 'code nom latitude longitude')\n");
    last->next = malloc(sizeof(City));
    if (last->next == NULL)
    {
        printf("Error : out of memory\n");
        free_Cities(list);
        exit(EXIT_FAILURE);
    }
    scanf("%d %s %f %f", &last->next->code, last->next->name, &last->next->latitude, &last->next->longitude);
}

void del_city(City *list)
{
    char input[85];
    City *current = list->next;
    bool found = false;
    printf("Saisissez le Nom de la ville à supprimer : \n");
    scanf(" %[^\n]", input);
    while (!found)
    {
        if (current == NULL)
        {
            printf("Nom de Ville invalide : saisissez 'p' pour afficher la liste des villes\n");
            found = true;
        }
        else if (!(strcmp(input, current->name)))
        {
            free_City(current, list);
            printf("La Ville %s a bien été supprimée.\n", input);
            found = true;
        }
        else
        {
            current = current->next;
        }
    }
}

void chg_city(City *list)
{
    ;
}

void print_cities(City *list)
{
    City *current = list->next;
    printf("CODE / VILLE / LATITUDE / LONGITUDE\n");
    while (current != NULL)
    {
        printf("%d / %s / %f / %f\n", current->code, current->name, current->latitude, current->longitude);
        current = current->next;
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

bool execute_command(char cmd, bool *exit, City *list_cities)
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
            print_cities(list_cities);
            break;
        }
        case 'a':
        {
            add_city(list_cities);
            break;
        }
        case 's':
        {
            del_city(list_cities);
            break;
        }
        case 'm':
        {
            chg_city(list_cities);
            break;
        }
        case 'c':
        {
            break;
        }
        case 'w':
        {
            save_csv("cities.csv", list_cities);
            *exit = true;
            break;
        }
        case 'q':
        {
            free_Cities(list_cities);
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

City list_cities;
char command;
bool quit;

// main Function

int main(void)
{
    // initializing variables

    list_cities.next = NULL;
    quit = false;

    // loading data & showing commands

    load_from_csv("cities.csv", &list_cities);
    show_help();
    
    // main loop

    while (!quit)
    {
        printf(">> ");
        scanf(" %c", &command);
        execute_command(command, &quit, &list_cities);
    }

    // quitting program

    return EXIT_SUCCESS;
}
