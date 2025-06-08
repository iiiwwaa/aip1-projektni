#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

// najveca dozvoljena velicina ploce
#define MAX_SIZE 20

// kodovi boja koristeni za brojeve
#define COLOR_BLUE "\x1b[34m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_RED "\x1b[31m"
#define COLOR_PURPLE "\x1b[35m"
#define COLOR_MAROON "\x1b[38;5;88m"
#define COLOR_TEAL "\x1b[36m"
#define COLOR_BLACK "\x1b[30m"
#define COLOR_GRAY "\x1b[37m"
#define COLOR_RESET "\x1b[0m"

int velicina; // igrac podesava velicinu po volji
int mine;     // igrac podesava broj mina po volji

char polje[MAX_SIZE][MAX_SIZE];
int otkriveno[MAX_SIZE][MAX_SIZE];
int oznaceno[MAX_SIZE][MAX_SIZE];
int gameover = 0;
int gamewon = 0;

// postavljanje ploce
void intPloca()
{
    for (int i = 0; i < velicina; i++)
    {
        for (int j = 0; j < velicina; j++)
        {
            polje[i][j] = '0';
            otkriveno[i][j] = 0;
            oznaceno[i][j] = 0;
        }
    }
}

// nasumicno postavi mine
void mjMina()
{
    int postMine = 0;
    while (postMine < mine)
    {
        int x = rand() % velicina;
        int y = rand() % velicina;
        if (polje[x][y] != '*')
        {
            polje[x][y] = '*';
            postMine++;
        }
    }
}

// racunanje susjednih mina za svako mjesto u polju
void calHint()
{
    const int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    const int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    for (int i = 0; i < velicina; i++)
    {
        for (int j = 0; j < velicina; j++)
        {
            if (polje[i][j] == '*')
                continue;
            int brojac = 0;
            for (int k = 0; k < 8; k++)
            {
                int ni = i + dx[k], nj = j + dy[k];
                if (ni >= 0 && ni < velicina && nj >= 0 && nj < velicina && polje[ni][nj] == '*')
                {
                    brojac++;
                }
            }
            if (brojac > 0)
                polje[i][j] = '0' + brojac;
        }
    }
}

// dobivanje boje broja
const char *bojaBroja(char num)
{
    switch (num)
    {
    case '1':
        return COLOR_BLUE;
    case '2':
        return COLOR_GREEN;
    case '3':
        return COLOR_RED;
    case '4':
        return COLOR_PURPLE;
    case '5':
        return COLOR_MAROON;
    case '6':
        return COLOR_TEAL;
    case '7':
        return COLOR_BLACK;
    case '8':
        return COLOR_GRAY;
    default:
        return COLOR_RESET;
    }
}

// ispis pocetnog polja za igranje
void printPolje()
{
    printf("\n   ");
    for (int i = 0; i < velicina; i++)
        printf(" %2d", i);
    printf("\n   +");
    for (int i = 0; i < velicina; i++)
        printf("---");
    printf("+\n");

    for (int i = 0; i < velicina; i++)
    {
        printf("%2d | ", i);
        for (int j = 0; j < velicina; j++)
        {
            if (gameover && polje[i][j] == '*')
            {
                printf("💣 "); // <-------------- ispisuje emotikon mine ako je igrac izgubio
            }
            else if (oznaceno[i][j] == 1)
            {
                printf("🚩 "); // <-------------- ispisuje emotikon zastave ako je igrac oznacio polje
            }
            else if (!otkriveno[i][j] == 1)
            {
                printf("⬛ "); // <-------------- ispisuje emotikon kocke ako igrac nije otvorio polje
            }
            else if (polje[i][j] == '0')
            {
                printf("   "); // <-------------- prazno otkriveno polje
            }
            else
            {
                printf("%s %c %s", bojaBroja(polje[i][j]), polje[i][j], COLOR_RESET);
            }
        }
        printf("|\n");
    }
    printf("   +");
    for (int i = 0; i < velicina; i++)
        printf("---");
    printf("+\n");
}

// otvaranje polja (i otvaranje praznih polja u blizini)
void otkri(int x, int y)
{
    if (x < 0 || x >= velicina || y < 0 || y >= velicina || otkriveno[x][y] == 1 || oznaceno[x][y] == 1)
        return;
    otkriveno[x][y] = 1;
    if (polje[x][y] == '*')
    {
        gameover = 1;
        return;
    }
    if (polje[x][y] == '0')
    {
        const int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
        const int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};
        for (int k = 0; k < 8; k++)
            otkri(x + dx[k], y + dy[k]);
    }
}

// provjeravanje dali je igrac pobijedio
int provjeri()
{
    int otkSafe = 0;
    for (int i = 0; i < velicina; i++)
    {
        for (int j = 0; j < velicina; j++)
        {
            if (!otkriveno[i][j] == 1 && polje[i][j] != '*')
                otkSafe++;
        }
    }
    return (otkSafe == 0);
}

int game()
{
    int brKoraka = 0;
    srand(time(0));

    // igrac unosi zeljenu velicinu polja
    printf("Unesite zeljenu velicinu polja (npr. 8 -> 8x8 polje):");
    scanf("%d", &velicina);
    if (velicina < 2 || velicina > MAX_SIZE)
    {
        printf("Nevaljana velicina! Koristenje uobicajene velicine 8x8.");
        velicina = 8;
    }

    // kalkuliranje najveceg dozvoljenog broja mina (35% od svih polja)
    int maxMina = velicina * velicina * 0.35;
    printf("Unesite zeljeni broj mina (1-%d): ", maxMina);
    scanf("%d", &mine);
    if (mine < 1 || mine > maxMina)
    {
        printf("Unesen nevaljani broj mina! Uneseno je %d mina.\n", velicina);
        mine = velicina;
    }

    intPloca();
    mjMina();
    calHint();

    while (!gameover && !gamewon)
    {
        printPolje();

        printf("\nOpcije:\n 1. Otvaranje polja\n 2. Postavljanje/micanje zastavice\n\n");
        printf("Unesite svoj odabir (1 ili 2): ");

        int odabir;
        scanf("%d", &odabir);
        if (odabir != 1 && odabir != 2)
        {
            printf("Nevaljani odabir! Pokusajte ponovno.\n");
            continue;
        }
        printf("Unesite redak i stupac (npr. 2 3): ");
        int x, y;
        scanf("%d%d", &x, &y);
        if (x < 0 || x >= velicina || y < 0 || y >= velicina)
        {
            printf("Nevaljano polje! Pokusajte ponovno.\n");
            continue;
        }
        if (odabir == 1)
        {
            if (oznaceno[x][y] == 1)
            {
                printf("Polje je oznaceno!");
            }
            else
            {
                otkri(x, y);
                if (gameover)
                {
                    printPolje();
                    printf("Pogodili ste minu, GAME OVER!\n");
                    brKoraka++;
                    printf("Vaš broj koraka je: %d", brKoraka);
                    break;
                }
                else if (provjeri())
                {
                    gamewon = 1;
                    printPolje();
                    printf("Sva sigurna polja su otkrivena, POBIJEDILI STE!\n");
                    brKoraka++;
                    printf("Vaš broj koraka je: %d", brKoraka);
                    break;
                }
            }
            brKoraka++;
        }
        else if (odabir == 2)
        {
            if (otkriveno[x][y] == 1)
            {
                printf("Ne mozete postaviti zastavicu na otkriveno polje!\n");
            }
            else
            {
                oznaceno[x][y] = !oznaceno[x][y]; //<------ maknuti zastavicu sa prethodno oznacenog polja
            }
            brKoraka++;
        }
    }
    return 0;
}

int main()
{
    int izbor;
    while (1)
    {
        printf("DOBRODOŠLI U MINOLOVAC\nAutori projekta: Ivano Fresl, Jan Jaić Krasnik\n\n");
        printf("1. Započni \n2. Upute \n3. Izlaz\n\nUnesite svoj izbor:");
        scanf("%d", &izbor);
        if (izbor == 1)
        {
            gameover = 0;
            gamewon = 0;
            printf("Započeli ste projektni predložak.\n\n");
            game();
        }
        else if (izbor == 2)
        {
            printf("Upute za korištenje:\n\n");
            printf("Kada započnete igru, odabirete veličinu svojeg polja (minimalno 3), zatim odabirete koliko se mina generira u polje.\n");
            printf("Nakon odabiranja veličina polja i količine mina, vaše polje se generira i ponuđeni ste sa dvije opcije.\n");
            printf("Opcija 1 je otkrivanje polja, opcija 2 je postavljanje zastavice na polje.\n");
            printf("Kada odaberete jednu od opcija, morate upisati koordinatu na kojoj želite obaviti tu opciju (npr. 2(redak) 3(stupac)).\n");
            printf("Sretno!");
        }
        else if (izbor == 3)
        {
            printf("Izlaz iz programa.\n");
            break;
        }
        else
        {
            printf("Nepoznat izbor, pokušajte ponovno.\n");
        }
        printf("\n\n");
    }
    return 0;
}
