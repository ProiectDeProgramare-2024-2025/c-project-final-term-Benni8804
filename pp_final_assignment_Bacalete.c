#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 50
#define MAX_GAMES 100
#define TOTAL_MEMORY 1024

// ANSI Color Codes
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define CYAN    "\033[1;36m"
#define GREY    "\033[1;90m"
#define PURPLE "\e[0;35m"
#define RESET   "\033[0m"

typedef struct {
    char name[MAX_NAME_LENGTH];
    float size;
} Game;

Game availableGames[MAX_GAMES] = {
    {"Game A", 50}, {"Game B", 80}, {"Game C", 120}, {"Game D", 200},
    {"Game E", 10}, {"Game F", 90}, {"Game G", 300}, {"Game H", 40}
};
int availableGameCount = 8;

Game installedGames[MAX_GAMES];
int installedGameCount = 0;
float usedMemory = 0;

void clearConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void saveInstalledGamesToFile() {
    FILE *file = fopen("installed_games.txt", "w");
    if (!file) {
        printf(RED "Error saving installed games data.\n" RESET);
        return;
    }
    for (int i = 0; i < installedGameCount; i++) {
        fprintf(file, "%s, %.2f\n", installedGames[i].name, installedGames[i].size);
    }
    fclose(file);
}

void readInstalledGamesFromFile() {
    FILE *file = fopen("installed_games.txt", "r");
    if (!file) {
        return;
    }
    installedGameCount = 0;
    usedMemory = 0;
    while (fscanf(file, "%49[^,], %f\n", installedGames[installedGameCount].name, &installedGames[installedGameCount].size) == 2) {
        usedMemory += installedGames[installedGameCount].size;
        installedGameCount++;
    }
    fclose(file);
}

void viewInstalledGames() {
    printf("\n" PURPLE "Installed Games:\n" PURPLE "--------------------\n" RESET);
    if (installedGameCount == 0) {
        printf(RED "No games are currently installed.\n" RESET);
        return;
    }
    printf(CYAN "Name\t\tSize (GB)\n" CYAN "----------------------\n" RESET);
    for (int i = 0; i < installedGameCount; i++) {
        printf(CYAN "%s" RESET "\t\t" YELLOW "%.2f GB\n" RESET, installedGames[i].name, installedGames[i].size);
    }
    printf("\n" GREY "Total Memory Used: " RESET YELLOW "%.2f GB" RESET GREY " / " RESET CYAN "%d GB" RESET "\n", usedMemory, TOTAL_MEMORY);
}

void viewAvailableGames() {
    printf("\n" PURPLE "Available Games:\n" PURPLE "--------------------\n" RESET);
    printf(CYAN "Name\t\tSize (GB)\n" CYAN "----------------------\n" RESET);
    int found = 0;
    for (int i = 0; i < availableGameCount; i++) {
        if (!isGameInstalled(availableGames[i].name)) {
            printf(CYAN "%s" RESET "\t\t" YELLOW "%.2f GB\n" RESET, availableGames[i].name, availableGames[i].size);
            found = 1;
        }
    }
    if (!found) {
        printf(RED "No available games left to install.\n" RESET);
    }
}

int isGameInstalled(const char* gameName) {
    for (int i = 0; i < installedGameCount; i++) {
        if (strcmp(installedGames[i].name, gameName) == 0) {
            return 1;
        }
    }
    return 0;
}

void installGame() {
    char gameName[MAX_NAME_LENGTH];
    printf(PURPLE "Enter the name of the game to install: " RESET);
    scanf(" %49[^\n]", gameName);

    for (int i = 0; i < availableGameCount; i++) {
        if (strcmp(availableGames[i].name, gameName) == 0) {
            if (isGameInstalled(gameName)) {
                printf(RED "%s is already installed.\n" RESET, gameName);
                return;
            }
            if (usedMemory + availableGames[i].size > TOTAL_MEMORY) {
                printf(RED "Not enough memory to install %s (%.2f GB needed, %.2f GB available).\n" RESET,
                       gameName, availableGames[i].size, TOTAL_MEMORY - usedMemory);
                return;
            }
            installedGames[installedGameCount++] = availableGames[i];
            usedMemory += availableGames[i].size;
            saveInstalledGamesToFile();
            printf(GREEN "%s installed successfully (%.2f GB used).\n" RESET, gameName, availableGames[i].size);
            return;
        }
    }
    printf(RED "Game '%s' not found in available games.\n" RESET, gameName);
}

void uninstallGame() {
    if (installedGameCount == 0) {
        printf(RED "No games are currently installed.\n" RESET);
        return;
    }

    char gameName[MAX_NAME_LENGTH];
    printf(PURPLE "Enter the name of the game to uninstall: " RESET);
    scanf(" %49[^\n]", gameName);

    for (int i = 0; i < installedGameCount; i++) {
        if (strcmp(installedGames[i].name, gameName) == 0) {
            float freedSize = installedGames[i].size;
            usedMemory -= freedSize;
            for (int j = i; j < installedGameCount - 1; j++) {
                installedGames[j] = installedGames[j + 1];
            }
            installedGameCount--;
            saveInstalledGamesToFile();
            printf(GREEN "%s uninstalled successfully (%.2f GB freed).\n" RESET, gameName, freedSize);
            return;
        }
    }
    printf(RED "Game '%s' not found in installed games.\n" RESET, gameName);
}

int compareByName(const void *a, const void *b) {
    return strcmp(((Game *)a)->name, ((Game *)b)->name);
}

int compareBySize(const void *a, const void *b) {
    float sizeA = ((Game *)a)->size;
    float sizeB = ((Game *)b)->size;
    return (sizeA > sizeB) - (sizeA < sizeB);  // Return 1, 0, or -1
}

void sortInstalledGames() {
    if (installedGameCount == 0) {
        printf(RED "No games to sort.\n" RESET);
        return;
    }

    int choice;
    printf(GREY "Sort by:\n1. Name\n2. Size\nChoose an option: " RESET);
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            qsort(installedGames, installedGameCount, sizeof(Game), compareByName);
            break;
        case 2:
            qsort(installedGames, installedGameCount, sizeof(Game), compareBySize);
            break;
        default:
            printf(RED "Invalid choice. Sorting cancelled.\n" RESET);
            return;
    }
    saveInstalledGamesToFile();
    printf(GREEN "Games sorted successfully.\n" RESET);
}

int main() {
#ifdef _WIN32
    system("color");
#endif

    readInstalledGamesFromFile();
    int choice;

    while (1) {
        clearConsole();
        printf("\n=== " CYAN "Game Management System" RESET " ===\n");
        printf("1. View Installed Games\n");
        printf("2. View Available Games\n");
        printf("3. Install Game\n");
        printf("4. Uninstall Game\n");
        printf("5. Sort Installed Games\n");
        printf("6. Exit\n");
        printf(GREY "Choose an option: " RESET);
        scanf("%d", &choice);

        clearConsole();
        switch (choice) {
            case 1:
                viewInstalledGames();
                break;
            case 2:
                viewAvailableGames();
                break;
            case 3:
                installGame();
                break;
            case 4:
                uninstallGame();
                break;
            case 5:
                sortInstalledGames();
                break;
            case 6:
                printf(GREEN "Exiting...\n" RESET);
                return 0;
            default:
                printf(RED "Invalid option!\n" RESET);
        }

        printf(GREY "\nPress Enter to continue..." RESET);
        while (getchar() != '\n');
        getchar();
    }

    return 0;
}
