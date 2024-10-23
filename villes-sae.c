#include <stdio.h>
#include <stdlib.h>

typedef struct Ville
{
    int code;
    float latitude, longitude;
    char nom[80];
}Ville;

int main(void)
{
    Villes *liste_villes = malloc(sizeof(Ville));
    FILE *csv = NULL;
    csv = fopen("cities.csv", "r+");
    if (csv != NULL)
    {
        liste_villes = realloc(liste_villes, sizeof(liste_villes)+sizeof(Ville));
        fclose(csv);
    }
    free(liste_villes);
    return 0;
}
