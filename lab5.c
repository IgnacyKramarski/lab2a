#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct klucz
{
    int indeks; // indeks w tablicy prostokatow badz kół
    char typ;   // przyjmuje 'P' bądz 'K' w zaleznosci od figury (prostokat/koło)

    /*
    klucz mowi nam do ktorej tablicy nalezy obiekt o indeksie "i"
    */
} klucz;

typedef struct punkt
{
    long double x;
    long double y;
} punkt;

typedef struct prostokat
{
    punkt lewyDolny;
    punkt prawyGorny;
} prostokat;

typedef struct prosta
{
    punkt punkt1;
    punkt punkt2;
} prosta;

typedef struct prostokatZgiety
{
    prostokat prostokat; // prostokat
    prosta prosta;       // prosta
    int indeksPrzedZagieciem;
} prostokatZgiety;

typedef struct kolo
{
    long double promien;
    punkt srodek;
} kolo;

typedef struct koloZgiete
{
    kolo kolo;
    prosta prosta;
    int indeksPrzedZagieciem;
} koloZgiete;

koloZgiete *KolaZagiete = NULL;
klucz *Klucze = NULL;
prostokatZgiety *ProstokatyZagiete = NULL;

int czyRowne(long double x, long double y)
{
    long double epsilon = 1e-6;
    return fabsl(x - y) < epsilon;
}
int czyJestWKole(punkt P, kolo Figura)
{
    Figura.promien *= Figura.promien; // mozna bo w c argument jest przekazywany przez wartosc
    long double odleglosc = (P.x - Figura.srodek.x) * (P.x - Figura.srodek.x) + (P.y - Figura.srodek.y) * (P.y - Figura.srodek.y);
    if (odleglosc <= (long double)Figura.promien)
        return 1;
    else
        return 0;
}

int czyJestWProstokacie(punkt P, prostokat Figura)
{
    if (Figura.lewyDolny.x <= P.x && P.x <= Figura.prawyGorny.x && Figura.lewyDolny.y <= P.y && P.y <= Figura.prawyGorny.y)
        return 1;
    else
        return 0;
}

int czyPoDobrejStronieProstej(punkt P, prosta Prosta)
{

    long double pseudoIloczynWektorowy = (Prosta.punkt2.x - Prosta.punkt1.x) * (P.y - Prosta.punkt1.y) -
                                         (Prosta.punkt2.y - Prosta.punkt1.y) * (P.x - Prosta.punkt1.x);

    if (czyRowne(pseudoIloczynWektorowy, (long double)0)) // pole = 0 - punkt lezy na prostej
        return 2;
    else
        return pseudoIloczynWektorowy < 0; // jesli punkt lezy po prawej
}

punkt odbicieSymetryczne(punkt P, prosta Prosta)
{
    long double dx = Prosta.punkt2.x - Prosta.punkt1.x; // tworzymy wektor z naszym odcinkiem - wsp. x-owa
    long double dy = Prosta.punkt2.y - Prosta.punkt1.y; // tworzymy wektor z naszym odcinkiem - wsp. y-owa

    long double vx = Prosta.punkt1.x - P.x;
    long double vy = Prosta.punkt1.y - P.y;

    /*
    szukany punkt bedzie postaci (x1 + t * dx, y1 + t * dy)
    wyznaczamy t, zauwazajac, ze iloczyn wektorowy danego odcinka tworzacego prosta
    oraz odcinka powstalego przez rzut punktu na te prosta bedzie rowny zero
    */

    long double t = (vx * dx + vy * dy) / (dx * dx + dy * dy);

    long double rx = Prosta.punkt1.x + t * dx;
    long double ry = Prosta.punkt1.y + t * dy;

    // teraz robimy finalne odbicie

    punkt Odbicie;
    Odbicie.x = 2 * rx - P.x;
    Odbicie.y = 2 * ry - P.y;
    return Odbicie;
}

void wejscie(int n) // uzupelnia tablice z KolaZagiete, ProstokatyZagiete, Klucze
{
    KolaZagiete = malloc((size_t)n * sizeof(koloZgiete));
    ProstokatyZagiete = malloc((size_t)n * sizeof(prostokatZgiety));
    Klucze = malloc((size_t)n * sizeof(klucz));

    int iloscKol = 0, iloscProstokatow = 0;
    char typFigury;
    int indeksZagietejFigury;
    for (int i = 0; i < n; i++)
    {
        if (!scanf(" %c", &typFigury))
        {
            free(Klucze);
            free(KolaZagiete);
            free(ProstokatyZagiete);
            printf("skill issue\n");
            return;
        }
        switch (typFigury)
        {
        case 'Z':
            if (!scanf("%d", &indeksZagietejFigury))
            {
                free(Klucze);
                free(KolaZagiete);
                free(ProstokatyZagiete);
                printf("skill issue\n");
                return;
            }
            if (Klucze[indeksZagietejFigury].typ == 'P')
            {
                if (!scanf("%Lf", &ProstokatyZagiete[iloscProstokatow].prosta.punkt1.x) || !scanf("%Lf", &ProstokatyZagiete[iloscProstokatow].prosta.punkt1.y) || !scanf("%Lf", &ProstokatyZagiete[iloscProstokatow].prosta.punkt2.x) || !scanf("%Lf", &ProstokatyZagiete[iloscProstokatow].prosta.punkt2.y))
                {
                    free(Klucze);
                    free(KolaZagiete);
                    free(ProstokatyZagiete);
                    printf("skill issue\n");
                    return;
                }

                ProstokatyZagiete[iloscProstokatow].prostokat = ProstokatyZagiete[Klucze[indeksZagietejFigury].indeks].prostokat;
                ProstokatyZagiete[iloscProstokatow].indeksPrzedZagieciem = Klucze[indeksZagietejFigury].indeks;

                Klucze[i].indeks = iloscProstokatow;
                Klucze[i].typ = 'P';

                iloscProstokatow++;
            }
            else if (Klucze[indeksZagietejFigury].typ == 'K')
            {
                if (!scanf("%Lf", &KolaZagiete[iloscKol].prosta.punkt1.x) || !scanf("%Lf", &KolaZagiete[iloscKol].prosta.punkt1.y) || !scanf("%Lf", &KolaZagiete[iloscKol].prosta.punkt2.x) || !scanf("%Lf", &KolaZagiete[iloscKol].prosta.punkt2.y))
                {
                    free(Klucze);
                    free(KolaZagiete);
                    free(ProstokatyZagiete);
                    printf("skill issue\n");
                    return;
                }

                KolaZagiete[iloscKol].kolo = KolaZagiete[Klucze[indeksZagietejFigury].indeks].kolo;
                KolaZagiete[iloscKol].indeksPrzedZagieciem = Klucze[indeksZagietejFigury].indeks;

                Klucze[i].indeks = iloscKol;
                Klucze[i].typ = 'K';

                iloscKol++;
            }

            break;
        case 'P':
            if (!scanf("%Lf", &ProstokatyZagiete[iloscProstokatow].prostokat.lewyDolny.x) || !scanf("%Lf", &ProstokatyZagiete[iloscProstokatow].prostokat.lewyDolny.y) || !scanf("%Lf", &ProstokatyZagiete[iloscProstokatow].prostokat.prawyGorny.x) || !scanf("%Lf", &ProstokatyZagiete[iloscProstokatow].prostokat.prawyGorny.y))
            {
                free(Klucze);
                free(KolaZagiete);
                free(ProstokatyZagiete);
                printf("skill issue\n");
                return;
            }

            ProstokatyZagiete[iloscProstokatow].prosta.punkt1.x = (long double)0; // jesli mmay do czynienia z niezagietym prostokatem, to ustawiamy,
            ProstokatyZagiete[iloscProstokatow].prosta.punkt1.y = (long double)0; // ze jego prosta jest wyznaczona przez punkty (0,0), (0,0) - to bedzie
            ProstokatyZagiete[iloscProstokatow].prosta.punkt2.x = (long double)0; // oznaczało, ze ta figura to niezgiety prostokat, uwzgledniany w rekurencji
            ProstokatyZagiete[iloscProstokatow].prosta.punkt2.y = (long double)0;
            ProstokatyZagiete[iloscProstokatow].indeksPrzedZagieciem = iloscProstokatow; // juz nie jest nic zagiete przed, wiec ten indeks to bedzie i

            Klucze[i].indeks = iloscProstokatow;
            Klucze[i].typ = 'P';

            iloscProstokatow++;
            break;
        case 'K':
            if (!scanf("%Lf", &KolaZagiete[iloscKol].kolo.srodek.x) || !scanf("%Lf", &KolaZagiete[iloscKol].kolo.srodek.y) || !scanf("%Lf", &KolaZagiete[iloscKol].kolo.promien))
            {
                free(Klucze);
                free(KolaZagiete);
                free(ProstokatyZagiete);
                printf("skill issue\n");
                return;
            }

            KolaZagiete[iloscKol].prosta.punkt1.x = (long double)0; // jesli mmay do czynienia z niezagietym kołem, to ustawiamy,
            KolaZagiete[iloscKol].prosta.punkt1.y = (long double)0; // ze jego prosta jest wyznaczona przez punkty (0,0), (0,0) - to bedzie
            KolaZagiete[iloscKol].prosta.punkt2.x = (long double)0; // oznaczało, ze ta figura to niezgiety prostokat, uwzgledniany w rekurencji
            KolaZagiete[iloscKol].prosta.punkt2.y = (long double)0;
            KolaZagiete[iloscKol].indeksPrzedZagieciem = iloscKol; // analogicznie jak w prostokatach

            Klucze[i].indeks = iloscKol;
            Klucze[i].typ = 'K';

            iloscKol++;
            break;
        default:
            break;
        }
    }
    koloZgiete *tmp1 = realloc(KolaZagiete, (size_t)iloscKol * sizeof(koloZgiete));
    if (tmp1 != NULL)
        KolaZagiete = tmp1;

    prostokatZgiety *tmp2 = realloc(ProstokatyZagiete, (size_t)iloscProstokatow * sizeof(prostokatZgiety));
    if (tmp2 != NULL)
        ProstokatyZagiete = tmp2;
}
int solveDlaProstokata(punkt Punkt, prostokatZgiety figura)
{
    if (czyRowne(figura.prosta.punkt1.x, (long double)0) && czyRowne(figura.prosta.punkt1.y, (long double)0) &&
        czyRowne(figura.prosta.punkt2.x, (long double)0) && czyRowne(figura.prosta.punkt2.y, (long double)0)) // oznacza ze nie ma co zginac
    {
        if (czyJestWProstokacie(Punkt, figura.prostokat) == 0)
            return 0;
        else if (czyJestWProstokacie(Punkt, figura.prostokat) == 1)
            return 1;
    }

    if (czyPoDobrejStronieProstej(Punkt, figura.prosta) == 0) // punkt nie jest po dobrej stronie
        return 0;
    else if (czyPoDobrejStronieProstej(Punkt, figura.prosta) == 2) // punkt lezy na prostej
        return solveDlaProstokata(Punkt, ProstokatyZagiete[figura.indeksPrzedZagieciem]);

    return solveDlaProstokata(Punkt, ProstokatyZagiete[figura.indeksPrzedZagieciem]) + solveDlaProstokata(odbicieSymetryczne(Punkt, figura.prosta), ProstokatyZagiete[figura.indeksPrzedZagieciem]);
}
int solveDlaKola(punkt Punkt, koloZgiete figura)
{
    if (czyRowne(figura.prosta.punkt1.x, (long double)0) && czyRowne(figura.prosta.punkt1.y, (long double)0) &&
        czyRowne(figura.prosta.punkt2.x, (long double)0) && czyRowne(figura.prosta.punkt2.y, (long double)0)) // oznacza ze nie ma co zginac
    {
        if (czyJestWKole(Punkt, figura.kolo) == 0)
            return 0;
        else if (czyJestWKole(Punkt, figura.kolo) == 1)
            return 1;
    }

    if (czyPoDobrejStronieProstej(Punkt, figura.prosta) == 0) // punkt nie jest po dobrej stronie prostej
        return 0;

    else if (czyPoDobrejStronieProstej(Punkt, figura.prosta) == 2) // punkt lezy na prostej
        return solveDlaKola(Punkt, KolaZagiete[figura.indeksPrzedZagieciem]);

    // tutaj punkt jest po dobrej stronie prostej
    return solveDlaKola(Punkt, KolaZagiete[figura.indeksPrzedZagieciem]) + solveDlaKola(odbicieSymetryczne(Punkt, figura.prosta), KolaZagiete[figura.indeksPrzedZagieciem]);
}
int main()
{
    int n, q;
    if (!scanf("%d", &n))
        return -1;
    if (!scanf("%d", &q))
        return -1;
    wejscie(n);

    int numerZapytania;
    punkt zapytanie;

    for (int i = 0; i < q; i++)
    {
        if (!scanf("%d", &numerZapytania) || !scanf("%Lf", &zapytanie.x) || !scanf("%Lf", &zapytanie.y))
        {
            free(Klucze);
            free(KolaZagiete);
            free(ProstokatyZagiete);
            return -1;
        }
        if (Klucze[numerZapytania].typ == 'P')
            printf("%d\n", solveDlaProstokata(zapytanie, ProstokatyZagiete[Klucze[numerZapytania].indeks]));
        else
            printf("%d", solveDlaKola(zapytanie, KolaZagiete[Klucze[numerZapytania].indeks]));
    }
    free(Klucze);
    free(KolaZagiete);
    free(ProstokatyZagiete);
}