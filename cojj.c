#include <stdio.h>
#include <stdlib.h>

#define MAX_KNIGHT_NAME_LENGTH 100

// Funkcja wczytująca i wypisująca rycerzy z pliku
int read_knights(const char *file_name, const char *faction) {
    FILE *file = fopen(file_name, "r");
    
    if (file == NULL) {
        printf("%s have not arrived on the battlefield\n", faction);
        return 0; // Zwraca 0, jeśli plik nie został otwarty
    }

    int n;
    fscanf(file, "%d", &n); // Wczytanie liczby rycerzy

    for (int i = 0; i < n; i++) {
        char name[MAX_KNIGHT_NAME_LENGTH];
        int hp, attack;

        // Wczytanie danych rycerza
        if (fscanf(file, "%s %d %d", name, &hp, &attack) == 3) {
            printf("I am %s knight %s. I will serve my king with my %d HP and %d attack.\n", faction, name, hp, attack);
        }
    }

    fclose(file);
    return 1; // Zwraca 1, jeśli plik został poprawnie przetworzony
}

int main() {
    // Przetwarzanie plików, zakończenie w przypadku błędu
    if (!read_knights("franci.txt", "Frankish")) return 0;
    if (!read_knights("saraceni.txt", "Spanish")) return 0;

    return 0;
}