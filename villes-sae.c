#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265359

/* define Types */

typedef struct City
{
    char code[15];
    float latitude, longitude;
    char name[30];
    struct City *next;
}City;

/* define Functions & Procedures */

void free_City(City *city, City *list) // removes the city from the list and deallocates the memory reserved for the city
{
    City *current = list;
    City *tmp = NULL;
    if (city == NULL)
    {
        fprintf(stderr, "Error : invalid 'city' argument for 'free_City', proceed anyway\n");
        return;
    }
    while (current->next != city) // finding the city preceding the one to delete
    {
        current = current->next;
    }
    tmp = current->next->next; // saving the end of the list in tmp
    free(current->next);
    current->next = tmp; // reattaching the end of the list    
}

void free_Cities(City *list) // removes all cities in the list
{
    while (list->next != NULL)
    {
        free_City(list->next, list);
    }
}

void create_city(const char *code, const char *name, float lat, float lng, City *end_of_list, City *list) // allocates a city-sized chunk of memory and fills it with data
{
    City *last = list;
    while (last->next != NULL) // finding the last city of the list
    {
        last = last->next;
    }
    last->next = malloc(sizeof(City));
    if (last->next == NULL) // if malloc fail, then we just quit the program after deallocating all memory
    {
        fprintf(stderr, "Error : out of memory\n");
        last->next = end_of_list;
        free_Cities(list);
        exit(EXIT_FAILURE);
    }
    last = last->next;
    strcpy(last->code, code);
    strcpy(last->name, name);
    last->latitude = lat;
    last->longitude = lng;
    last->next = end_of_list; // attaching the possible end of list to the current list
}

void load_from_csv(const char *file_path, City *list) // loads cities data from a csv file into a list
{
    FILE *csv = NULL;
    City *current = NULL;
    int nb_of_scan;

    free_Cities(list); // if there is already any data, then delete it
    current = list;
    
    csv = fopen(file_path, "r");
    if (csv == NULL)
    {
        fprintf(stderr, "Error : can't open file '%s'\n", file_path);
        exit(EXIT_FAILURE);
    }
    
    fscanf(csv, "CODE,VILLE,LATITUDE,LONGITUDE\n"); // reading the title line
    do
    {
        current->next = malloc(sizeof(City));
        if (current->next == NULL) // if malloc fail, then we just quit the program after deallocating all memory
        {
            fprintf(stderr, "Error : out of memory\n");
            free_Cities(list);
            fclose(csv);
            exit(EXIT_FAILURE);
        }
        current = current->next;
        nb_of_scan = fscanf(csv, "%[^,],%[^,],%f,%f\n", current->code, current->name, &current->latitude, &current->longitude);
    } while (nb_of_scan != EOF);
    free_City(current, list); // freeing the last city as it wasn't filled with any data
    fclose(csv);
}

City *search_City(const char *city_name, bool searching_previous, City *list) // returns a pointer to the city with the given city_name or the previous one if requested
{
    City *current = list;
    City *return_value = NULL;
    bool found = false;
    while (!found)
    {
        if (current->next == NULL) // if the searched city isn't is the given list
        {
            printf("Nom de Ville invalide : saisissez 'p' pour afficher la liste des villes\n");
            found = true;
        }
        else if (!(strcmp(city_name, current->next->name))) // if we found the searched city
        {
            if (searching_previous)
            {
                return_value = current;
            }
            else
            {
                return_value = current->next;
            }
            found = true;
        }
        else
        {
            current = current->next;
        }
    }
    return return_value;
}

int distance_cities(City *city_1, City *city_2)
{
    float r = 6371;
    float phi_1 = city_1->latitude * PI / 180;
    float phi_2 = city_2->latitude * PI / 180;
    float delta_phi = phi_1 - phi_2;
    float lambda_1 = city_1->longitude * PI / 180;
    float lambda_2 = city_2->longitude * PI / 180;
    float delta_lambda = lambda_1 - lambda_2;
    return 2*r * asinf(sqrtf(powf(sinf(delta_phi / 2), 2) + cosf(phi_1) * cosf(phi_2) * powf(sinf(delta_lambda / 2), 2)));
}

void save_csv(const char *file_path, City *list) // writes data from a list onto a csv file and deletes data of the list
{
    FILE *csv = NULL;
    City *current = list->next;
    csv = fopen(file_path, "w+");
    if (csv == NULL)
    {
        fprintf(stderr, "Error : can't open/create file '%s'\n", file_path);
        free_Cities(list);
        exit(EXIT_FAILURE);
    }
    fprintf(csv, "CODE,VILLE,LATITUDE,LONGITUDE\n"); // writing the title line
    while (current != NULL)
    {
        fprintf(csv, "%s,%s,%f,%f\n", current->code, current->name, current->latitude, current->longitude);
        current = current->next;
    }
    fclose(csv);
    free_Cities(list);
}

void user_add_city(City *list) // requests the user to add the city he wants to the list
{
    City tmp;
    printf("Veuillez renseigner le nom de la ville à ajouter : ");
    scanf(" %[^\n]", tmp.name);
    printf("Veuillez renseigner le code de la ville à ajouter : ");
    scanf(" %[^\n]", tmp.code);
    printf("Veuillez renseigner la latitude puis la longitude en degrés (ex : 52.56898 -86.565325) : ");
    scanf("%f %f", &tmp.latitude, &tmp.longitude);
    create_city(tmp.code, tmp.name, tmp.latitude, tmp.longitude, NULL, list);
    printf("La Ville %s a bien été ajoutée.\n", tmp.name);
}

void user_del_city(City *list) // requests the user to delete the city he wants of the list
{
    char input[85];
    City *city_to_del = NULL;
    printf("Saisissez le Nom de la ville à supprimer : ");
    scanf(" %[^\n]", input);
    city_to_del = search_City(input, false, list);
    if (city_to_del != NULL) // if the city has been found
    {
        free_City(city_to_del, list);
        printf("La Ville %s a bien été supprimée.\n", input);
    }
}

void user_chg_city(City *list) // requests the user to change the city he wants in the list
{
    char input[85];
    City *previous_city = NULL;
    City *end = NULL;
    City *old = NULL;
    City tmp;
    printf("Saisissez le Nom de la ville à modifier : ");
    scanf(" %[^\n]", input);
    previous_city = search_City(input, true, list);
    if (previous_city != NULL) // if the previous city has been found
    {
        old = previous_city->next;
        end = old->next; // saving the end of list
        printf("Anciennes données : %s / %s / %f / %f\n", old->code, old->name, old->latitude, old->longitude);
        free(old); // deleting the city
        previous_city->next = NULL;
        printf("Veuillez renseigner le nouveau code de %s : ", input);
        scanf(" %[^\n]", tmp.code);
        printf("Veuillez renseigner le nouveau nom de %s : ", input);
        scanf(" %[^\n]", tmp.name);
        printf("Veuillez renseigner la latitude puis la longitude en degrés (ex : 52.56898 -86.565325) : ");
        scanf("%f %f", &tmp.latitude, &tmp.longitude);
        create_city(tmp.code, tmp.name, tmp.latitude, tmp.longitude, end, list); // recreating the city at the same place in the list and reattaching the end of list
        printf("La Ville %s a bien été modifiée.\n", input);
    }
}

void user_coord_city(City *list) // requests the user to show him the coords of the city he wants
{
    char input[85];
    City *city_to_aff = NULL;
    printf("Saisissez le Nom de la ville dont vous souhaitez les coordonnées : ");
    scanf(" %[^\n]", input);
    city_to_aff = search_City(input, false, list);
    if (city_to_aff != NULL) // if the city has been found
    {
        printf("%s se situe aux coordonnées (lat / long) : %f / %f .\n", input, city_to_aff->latitude, city_to_aff->longitude);
    }
}

void user_distance_min_cities(City *list)
{
    char input1[85], input2[85];
    City *city1 = NULL, *city2 = NULL;
    int dist = 0;
    printf("Saisissez le Nom de la première ville : \n");
    scanf(" %[^\n]", input1);
    city1 = search_City(input1, false, list);
    if (city1 == NULL)
    {
        return;
    }
    printf("Saisissez le Nom de la deuxième ville : \n");
    scanf(" %[^\n]", input2);
    city2 = search_City(input2, false, list);
    if (city2 == NULL)
    {
        return;
    }
    dist = distance_cities(city1, city2);
    printf("La distance entre %s et %s est %d km.\n", input1, input2, dist);
}

void print_cities(City *list) // prints all cities from the list
{
    City *current = list->next;
    char tmp[15];
    printf("+-----------------+--------------------------------+-------------+-------------+\n");
    printf("| CODE            | VILLE                          | LATITUDE    | LONGITUDE   |\n");
    printf("+-----------------+--------------------------------+-------------+-------------+\n");
    while (current != NULL)
    {
        printf("| %s", current->code);
        for (int i = 0; i < 16 - strlen(current->code); i++)
        {
            printf(" ");
        }
        printf("| %s", current->name);
        for (int i = 0; i < 31 - strlen(current->name); i++)
        {
            printf(" ");
        }
        sprintf(tmp, "%f", current->latitude);
        printf("| %f", current->latitude);
        for (int i = 0; i < 12 - strlen(tmp); i++)
        {
            printf(" ");
        }
        sprintf(tmp, "%f", current->longitude);
        printf("| %f", current->longitude);
        for (int i = 0; i < 12 - strlen(tmp); i++)
        {
            printf(" ");
        }
        printf("|\n");
        current = current->next;
    }
    printf("+-----------------+--------------------------------+-------------+-------------+\n");
}

void show_help(void) // prints the list of possible commands
{
    printf("--------------------------------------------------------------\n");
    printf("Commandes disponibles :\n");
    printf("h - affiche cette liste de commandes\n");
    printf("p - affiche les données de toutes les villes\n");
    printf("a - permet d'ajouter une ville\n");
    printf("s - permet de supprimer une ville\n");
    printf("m - permet de modifier une ville\n");
    printf("c - permet d'afficher les coordonnées GPS d'une ville\n");
    printf("d - permet d'afficher la distance entre deux villes\n");
    printf("e - enregistre les modifications et quitte le programme\n");
    printf("q - quitte le programme en abandonnant les modifications\n");
    printf("--------------------------------------------------------------\n");
}

void execute_command(char cmd, bool *exit, const char *file_path, City *list_cities) // executes the given command
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
            user_add_city(list_cities);
            break;
        }
        case 's':
        {
            user_del_city(list_cities);
            break;
        }
        case 'm':
        {
            user_chg_city(list_cities);
            break;
        }
        case 'c':
        {
            user_coord_city(list_cities);
            break;
        }
        case 'd':
        {
            user_distance_min_cities(list_cities);
            break;
        }
        case 'e':
        {
            save_csv(file_path, list_cities);
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

/* define Variables */

City list_cities;
char csv_file_name[50];
char command;
bool quit;

/* main Function */

int main(void)
{
    // initializing variables

    list_cities.next = NULL;
    quit = false;

    // obtain csv file path

    printf("Saisissez le nom du fichier csv : ");
    strcpy(csv_file_name, "cities.csv");
    printf("%s\n", csv_file_name);
    // scanf("%s", csv_file_name);

    // loading data & showing commands

    load_from_csv(csv_file_name, &list_cities);
    show_help();
    
    // main loop

    while (!quit)
    {
        printf(">> ");
        scanf(" %c", &command);
        execute_command(command, &quit, csv_file_name, &list_cities);
    }

    // quitting program

    return EXIT_SUCCESS;
}
