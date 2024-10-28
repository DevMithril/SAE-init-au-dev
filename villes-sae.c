#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

/* define Types */

typedef struct City
{
    int code;
    float latitude, longitude;
    char name[85];
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

void create_city(int code, const char *name, float lat, float lng, City *end_of_list, City *list) // allocates a city-sized chunk of memory and fills it with data
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
    last->code = code;
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
    
    fscanf(csv, "CODE,VILLE,LATITUDE,LONGITUDE"); // reading the title line
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
        nb_of_scan = fscanf(csv, "%d,%[^,],%f,%f", &current->code, current->name, &current->latitude, &current->longitude);
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
        fprintf(csv, "%d,%s,%f,%f\n", current->code, current->name, current->latitude, current->longitude);
        current = current->next;
    }
    fclose(csv);
    free_Cities(list);
}

void user_add_city(City *list) // requests the user to add the city he wants to the list
{
    City tmp;
    printf("Veuillez renseigner les données de la ville à ajouter (format : code *nom de la ville* latitude longitude)\n");
    scanf("%d *%[^*]* %f %f", &tmp.code, tmp.name, &tmp.latitude, &tmp.longitude);
    create_city(tmp.code, tmp.name, tmp.latitude, tmp.longitude, NULL, list);
    printf("La Ville %s a bien été ajoutée.\n", tmp.name);
}

void user_del_city(City *list) // requests the user to delete the city he wants of the list
{
    char input[85];
    City *city_to_del = NULL;
    printf("Saisissez le Nom de la ville à supprimer : \n");
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
    City tmp;
    printf("Saisissez le Nom de la ville à modifier : \n");
    scanf(" %[^\n]", input);
    previous_city = search_City(input, true, list);
    if (previous_city != NULL) // if the previous city has been found
    {
        end = previous_city->next->next; // saving the end of list
        free(previous_city->next); // deleting the city
        previous_city->next = NULL; 
        printf("Veuillez renseigner les nouvelles données pour %s (format : code *nom de la ville* latitude longitude')\n", input);
        scanf("%d *%[^*]* %f %f", &tmp.code, tmp.name, &tmp.latitude, &tmp.longitude);
        create_city(tmp.code, tmp.name, tmp.latitude, tmp.longitude, end, list); // recreating the city at the same place in the list and reattaching the end of list
        printf("La Ville %s a bien été modifiée.\n", input);
    }
}

void user_coord_city(City *list) // requests the user to show him the coords of the city he wants
{
    char input[85];
    City *city_to_aff = NULL;
    printf("Saisissez le Nom de la ville dont vous souhaitez les coordonnées : \n");
    scanf(" %[^\n]", input);
    city_to_aff = search_City(input, false, list);
    if (city_to_aff != NULL) // if the city has been found
    {
        printf("%s se situe aux coordonnées (lat / long) : %f / %f .\n", input, city_to_aff->latitude, city_to_aff->longitude);
    }
}

void print_cities(City *list) // prints all cities from the list
{
    City *current = list->next;
    printf("CODE / VILLE / LATITUDE / LONGITUDE\n");
    while (current != NULL)
    {
        printf("%d / %s / %f / %f\n", current->code, current->name, current->latitude, current->longitude);
        current = current->next;
    }
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
    scanf("%s", csv_file_name);

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
