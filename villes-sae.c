#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

// définitions des Constantes

#define PI 3.14159265359
#define R_TERRE 6371
#define MAX_LEN_CSV_FILENAME 50
#define MAX_LEN_CODE 15
#define MAX_LEN_NAME 30
#define MAX_LEN_LAT_LONG 11

// définitions des Types

typedef struct City
{
    char code[MAX_LEN_CODE];
    float latitude, longitude;
    char name[MAX_LEN_NAME];
    struct City *next;
}City;

// définitions des Fonctions & Procédures

/* retire la ville de la liste et déalloue la mémoire réservée pour la ville */
void free_City(City *city, City *list)
{
    City *current = list;
    City *tmp = NULL;
    if (city == NULL)
    {
        fprintf(stderr, "Error : invalid 'city' argument for 'free_City', proceed anyway\n");
        return;
    }
    while (current->next != city) // obtention de la ville précédant celle à supprimer
    {
        current = current->next;
    }
    tmp = current->next->next; // sauvegarde de la fin de la liste dans tmp
    free(current->next);
    current->next = tmp; // réattachement de la fin de la liste
}

/* supprime toutes les villes de la liste */
void free_Cities(City *list)
{
    while (list->next != NULL)
    {
        free_City(list->next, list);
    }
}
/* alloue un emplacement mémoire de la taille d'une ville et le remplit avec les données passées en paramètres */
void create_city(const char *code, const char *name, float lat, float lng, City *end_of_list, City *list)
{
    City *last = list;
    while (last->next != NULL) // obtention de la dernière ville de la liste
    {
        last = last->next;
    }
    last->next = malloc(sizeof(City));
    if (last->next == NULL) // si malloc échoue, alors on quitte le programme après avoir déalloué toute la mémoire
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
    last->next = end_of_list; // attachement de l'éventuelle fin de liste à la liste actuelle
}

/* charge dans une liste des données de villes issues d'un fichier csv */
void load_from_csv(const char *file_path, City *list)
{
    FILE *csv = NULL;
    City *current = NULL;
    int nb_of_scan;

    free_Cities(list); // si il y a déjà des données, alors on les supprime
    current = list;
    
    csv = fopen(file_path, "r");
    if (csv == NULL) // si l'ouverture du fichier échoue, alors on quite le programme
    {
        fprintf(stderr, "Error : can't open file '%s'\n", file_path);
        exit(EXIT_FAILURE);
    }
    
    fscanf(csv, "CODE,VILLE,LATITUDE,LONGITUDE\n"); // lecture de la ligne des titres
    do
    {
        current->next = malloc(sizeof(City));
        if (current->next == NULL) // si malloc échoue, alors on quitte le programme après avoir déalloué toute la mémoire
        {
            fprintf(stderr, "Error : out of memory\n");
            free_Cities(list);
            fclose(csv);
            exit(EXIT_FAILURE);
        }
        current = current->next;
        nb_of_scan = fscanf(csv, "%[^,],%[^,],%f,%f\n", current->code, current->name, &current->latitude, &current->longitude);
    } while (nb_of_scan != EOF);
    free_City(current, list); // libération de la dernière ville car elle ne contient aucunes données
    fclose(csv);
}

/* renvoie un pointeur vers la ville ayant le nom demandé ou celle qui précède si c'est demandé */
City *search_City(const char *city_name, bool searching_previous, City *list)
{
    City *current = list;
    City *return_value = NULL;
    bool found = false;
    while (!found)
    {
        if (current->next == NULL) // si la ville recherchée n'est pas dans la liste donnée
        {
            printf("Nom de Ville invalide : saisissez 'p' pour afficher la liste des villes\n");
            found = true;
        }
        else if (!(strcmp(city_name, current->next->name))) // si on a trouvé la ville recherchée
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

/* renvoie la distance qui sépare les deux emplacements dont les coordonnées sont fournies */
int distance_cities(float lat1, float long1, float lat2, float long2)
{
    float phi_1 = lat1 * PI / 180;
    float phi_2 = lat2 * PI / 180;
    float delta_phi = phi_1 - phi_2;
    float lambda_1 = long1 * PI / 180;
    float lambda_2 = long2 * PI / 180;
    float delta_lambda = lambda_1 - lambda_2;
    return 2*R_TERRE * asinf(sqrtf(powf(sinf(delta_phi / 2), 2) + cosf(phi_1) * cosf(phi_2) * powf(sinf(delta_lambda / 2), 2)));
}

/* écrit les données d'une liste de villes dans un fichier csv et supprime les données de la liste */
void save_csv(const char *file_path, City *list)
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
    fprintf(csv, "CODE,VILLE,LATITUDE,LONGITUDE\n"); // écriture de la ligne des titres
    while (current != NULL)
    {
        fprintf(csv, "%s,%s,%f,%f\n", current->code, current->name, current->latitude, current->longitude);
        current = current->next;
    }
    fclose(csv);
    free_Cities(list);
}
/* demande à l'utilisateur la ville qu'il veut ajouter à la liste et l'ajoute */
void user_add_city(City *list)
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

/* demande à l'utilisateur la ville qu'il veut supprimer de la liste et la supprime */
void user_del_city(City *list)
{
    char input[MAX_LEN_NAME];
    City *city_to_del = NULL;
    printf("Saisissez le Nom de la ville à supprimer : ");
    scanf(" %[^\n]", input);
    city_to_del = search_City(input, false, list);
    if (city_to_del != NULL) // si la ville a été trouvée
    {
        free_City(city_to_del, list);
        printf("La Ville %s a bien été supprimée.\n", input);
    }
}

/* demande à l'utilisateur la ville qu'il veut modifier dans la liste et la modifie */
void user_chg_city(City *list)
{
    char input[MAX_LEN_NAME];
    City *previous_city = NULL;
    City *end = NULL;
    City *old = NULL;
    City tmp;
    printf("Saisissez le Nom de la ville à modifier : ");
    scanf(" %[^\n]", input);
    previous_city = search_City(input, true, list);
    if (previous_city != NULL) // si la ville précédante a été trouvée
    {
        old = previous_city->next;
        end = old->next; // sauvegarde de la fin de la liste
        printf("Anciennes données : %s / %s / %f / %f\n", old->code, old->name, old->latitude, old->longitude);
        free(old); // suppression de la ville
        previous_city->next = NULL;
        printf("Veuillez renseigner le nouveau code de %s : ", input);
        scanf(" %[^\n]", tmp.code);
        printf("Veuillez renseigner le nouveau nom de %s : ", input);
        scanf(" %[^\n]", tmp.name);
        printf("Veuillez renseigner la latitude puis la longitude en degrés (ex : 52.56898 -86.565325) : ");
        scanf("%f %f", &tmp.latitude, &tmp.longitude);
        create_city(tmp.code, tmp.name, tmp.latitude, tmp.longitude, end, list); // recréation de la ville au même endroit dans la liste et réattachement de la fin de la liste
        printf("La Ville %s a bien été modifiée.\n", input);
    }
}

/* demande à l'utilisateur la ville dont il veut obtenir les coordonnées et affiche ces coordonnées */
void user_coord_city(City *list)
{
    char input[MAX_LEN_NAME];
    City *city_to_aff = NULL;
    printf("Saisissez le Nom de la ville dont vous souhaitez les coordonnées : ");
    scanf(" %[^\n]", input);
    city_to_aff = search_City(input, false, list);
    if (city_to_aff != NULL) // si la ville a été trouvée
    {
        printf("%s se situe aux coordonnées (lat / long) : %f / %f .\n", input, city_to_aff->latitude, city_to_aff->longitude);
    }
}

/* demande à l'utilisateur les villes dont il veut connaitre la distance qui les sépare */
void user_distance_min_cities(City *list)
{
    char city_name1[MAX_LEN_NAME], city_name2[MAX_LEN_NAME];
    City *city1 = NULL, *city2 = NULL;
    int dist = 0;
    printf("Saisissez le Nom de la première ville : ");
    scanf(" %[^\n]", city_name1);
    city1 = search_City(city_name1, false, list);
    if (city1 == NULL)
    {
        return;
    }
    printf("Saisissez le Nom de la deuxième ville : ");
    scanf(" %[^\n]", city_name2);
    city2 = search_City(city_name2, false, list);
    if (city2 == NULL)
    {
        return;
    }
    dist = distance_cities(city1->latitude, city1->longitude, city2->latitude, city2->longitude);
    printf("La distance entre %s et %s est %d km.\n", city_name1, city_name2, dist);
}

/* demande à l'utilisateur ses coordonnées GPS et la ville dont il veu connaitre son éloignement */
void user_how_far_from_city(City *list)
{
    char city_name[MAX_LEN_NAME];
    float user_lat, user_long;
    City *city = NULL;
    int dist = 0;
    printf("Saisissez le Nom de la ville : ");
    scanf(" %[^\n]", city_name);
    city = search_City(city_name, false, list);
    if (city == NULL)
    {
        return;
    }
    printf("Veuillez saisir vos coordonnées GPS (latitude puis longitude en degrés) : ");
    scanf("%f %f", &user_lat, &user_long);
    dist = distance_cities(city->latitude, city->longitude, user_lat, user_long);
    printf("La distance qui vous sépare de %s est %d km.\n", city_name, dist);
}

/* affiche toutes les villes de la liste */
void print_cities(City *list)
{
    City *current = list->next;
    char tmp[MAX_LEN_CODE];
    printf("+-----------------+--------------------------------+-------------+-------------+\n");
    printf("| CODE            | VILLE                          | LATITUDE    | LONGITUDE   |\n");
    printf("+-----------------+--------------------------------+-------------+-------------+\n");
    while (current != NULL)
    {
        printf("| %s", current->code);
        for (int i = 0; i < MAX_LEN_CODE + 1 - strlen(current->code); i++)
        {
            printf(" ");
        }
        printf("| %s", current->name);
        for (int i = 0; i < MAX_LEN_NAME + 1 - strlen(current->name); i++)
        {
            printf(" ");
        }
        sprintf(tmp, "%f", current->latitude);
        printf("| %f", current->latitude);
        for (int i = 0; i < MAX_LEN_LAT_LONG + 1 - strlen(tmp); i++)
        {
            printf(" ");
        }
        sprintf(tmp, "%f", current->longitude);
        printf("| %f", current->longitude);
        for (int i = 0; i < MAX_LEN_LAT_LONG + 1 - strlen(tmp); i++)
        {
            printf(" ");
        }
        printf("|\n");
        current = current->next;
    }
    printf("+-----------------+--------------------------------+-------------+-------------+\n");
}

/* affiche la liste des commandes possibles */
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
    printf("d - permet d'afficher la distance entre deux villes\n");
    printf("f - permet d'afficher la distance qui vous sépare d'une ville\n");
    printf("e - enregistre les modifications et quitte le programme\n");
    printf("q - quitte le programme en abandonnant les modifications\n");
    printf("--------------------------------------------------------------\n");
}

/* exécute la commande donnée */
void execute_command(char cmd, bool *exit, const char *file_path, City *list_cities)
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
        case 'f':
        {
            user_how_far_from_city(list_cities);
            break;
        }
        case 'e':
        {
            save_csv(file_path, list_cities);
            printf("Les modifications ont bien été enregistrées dans %s.\n", file_path);
            *exit = true;
            break;
        }
        case 'q':
        {
            free_Cities(list_cities);
            printf("Les modifications ont été abandonnées.\n");
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

// définition des Variables

City list_cities;
char csv_file_name[MAX_LEN_CSV_FILENAME];
char command;
bool quit;

/* Fonction principale */
int main(void)
{
    // initialisation des variables

    list_cities.next = NULL;
    quit = false;

    // obtention du chemin du fichier csv

    printf("Saisissez le nom du fichier csv : ");
    strcpy(csv_file_name, "cities.csv");
    printf("%s\n", csv_file_name);
    // scanf("%s", csv_file_name);

    // chargement des données & affichage des commandes

    load_from_csv(csv_file_name, &list_cities);
    show_help();
    
    // boucle principale

    while (!quit)
    {
        printf(">> ");
        scanf(" %c", &command);
        execute_command(command, &quit, csv_file_name, &list_cities);
    }

    // fin de l'exécution du programme

    return EXIT_SUCCESS;
}
