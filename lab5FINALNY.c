#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* Jest to struktura, ktora mapuje globalny iddeks kartki (wczytywany przez uzytkownika)
na indeks w tablicy prostokatow zagietych badz kol zagietych. Dodatkowo, zawiera informacje
o typie kartki (czy jest to prostokat zagiety czy kolo zagiete) */
typedef struct klucz
{
    int indeks; // indeks w tablicy prostokatow badz kół
    char typ;   // 'P' lub 'K'

} klucz;

/* Po prostu punkt na plaszczyznie*/
typedef struct punkt
{
    double x;
    double y;

} punkt;

/* sklada sie z lewego dolnego punktu i prawego gornego w prostokacie, ktorgo boki sa
rownolegle badz prostopadle do osi ukladu wspolrzednych */
typedef struct prostokat
{
    punkt lewyDolny;
    punkt prawyGorny;

} prostokat;

/* sklada sie z pierwszego i drugiego punktu, przez ktory ta prosta przechodzi */
typedef struct prosta
{
    punkt punkt1;
    punkt punkt2;

} prosta;

/* Jest to struktura skladajaca sie z zagietego prostokata, prostej wedle ktorej
zostal on zagiety, prostokatu bazowego ktory zaczlismy zaginac oraz indeksu innego
prostokata w tablicy prostokatow zagietych (byc moze tez zagietego), ktory zostal zagiety
przez prosta zaginajaca. W naszej implementacji niezagiety prostokat ma indeks przed zagieciem
rowny -1, a prosta zaginajaca nie bedzie zainicjalizowana */
typedef struct prostokatZgiety
{
    prostokat prostokatBazowy;
    prosta prostaZaginajaca;
    int indeksPrzedZagieciem;

} prostokatZgiety;

/* sklada sie oczywiscie z promienia oraz srodka */
typedef struct kolo
{
    double promien;
    punkt srodek;

} kolo;

/* Analogicznie jak w prostokacie zagietym, tylko ze mamy tutaj kolo bazowe zamiast prostokata */
typedef struct koloZgiete
{
    kolo kolo;
    prosta prosta;
    int indeksPrzedZagieciem;

} koloZgiete;

// tablica zagietych kol
koloZgiete *KolaZagiete = NULL;

// tablica Kluczy do mapowania globalnych indeksow na indeksy w prostokatach i kolach
klucz *Klucze = NULL;

// tablica zagietych prostokatow
prostokatZgiety *ProstokatyZagiete = NULL;

const double epsilon = 1e-6; //  epsilon wykorzystywany do sprawdzania nierownosci

/* Funkcja porownujaca dwie zmienne typu double poprzez sprawdzenie, czy ich
roznica jest mniejsza od ustalonego epsilonu, tutaj rownego 1e-6 */
int czyRowne(double x, double y)
{
    return fabsl(x - y) < epsilon;
}

/* Sprawdzamy twierdzeniem pitagorasa, czy odleglosc danego punktu od srodku
okregu jest mniejsza badz rowna epsilonowi */
int czyJestWKole(punkt P, kolo Figura)
{
    double dx = P.x - Figura.srodek.x;
    double dy = P.y - Figura.srodek.y;
    double odleglosc = dx * dx + dy * dy;
    return odleglosc <= Figura.promien * Figura.promien + epsilon;
}

/* Sprawdzamy, czy punkt znajduje sie w prostokacie poprzez porownanie jego
wspolrzednych ze wspolrzednymi prostokata */
int czyJestWProstokacie(punkt P, prostokat Figura)
{
    return (Figura.lewyDolny.x <= P.x + epsilon && P.x <= Figura.prawyGorny.x + epsilon &&
            Figura.lewyDolny.y <= P.y + epsilon && P.y <= Figura.prawyGorny.y + epsilon);
}

/* Sprawdzamy po ktorej stronie prostej znajduje sie punkt, korzystajac
z pseudoiloczynu wektorowego. Zwracamy -1, jesli punkt jest po prawej stronie
prostej, 0 jesli znajduje sie na prostej, 1, jesli jest po lewej stronie prostej */
int stronaProstej(punkt P, prosta Prosta)
{
    double vx = Prosta.punkt2.x - Prosta.punkt1.x;
    double vy = Prosta.punkt2.y - Prosta.punkt1.y;
    double wx = P.x - Prosta.punkt1.x;
    double wy = P.y - Prosta.punkt1.y;
    double cross = vx * wy - vy * wx;
    if (czyRowne(cross, 0.0L))
        return 0;
    return (cross + epsilon > 0) ? +1 : -1;
}

/* Rzutujemy punkt na prosta, a potem odbijamy go symetrycznie wzgledem tego punktu.
Do rzutowania punktu wykorzystujemy fakt, ze iloczyn wektorowy prostopadlych wektorow
jest zawsze rowny zero */
punkt odbicieSymetryczne(punkt P, prosta Prosta)
{
    double dx = Prosta.punkt2.x - Prosta.punkt1.x;
    double dy = Prosta.punkt2.y - Prosta.punkt1.y;
    double vx = P.x - Prosta.punkt1.x;
    double vy = P.y - Prosta.punkt1.y;

    /* stosunek mowiacy nam "jak bardzo" chcemy przesunac punkt P1 na prostej.
    Wyprowadzamy go z omowionej wczesniej zaleznosci */
    double t = (vx * dx + vy * dy) / (dx * dx + dy * dy);
    double rx = Prosta.punkt1.x + t * dx;
    double ry = Prosta.punkt1.y + t * dy;
    punkt Odbicie;
    Odbicie.x = 2 * rx - P.x;
    Odbicie.y = 2 * ry - P.y;
    return Odbicie;
}

/* Funkcja uzupelniajaca tablice prostokatow zagietych, kluczy oraz kol zagietych, przyjmuje
jako argument "n", mowiacy nam o tym, kartek wczytujemy */
void wejscie(int n)
{
    /* alokowanie tablic */
    KolaZagiete = malloc((size_t)n * sizeof(koloZgiete));
    ProstokatyZagiete = malloc((size_t)n * sizeof(prostokatZgiety));
    Klucze = malloc((size_t)n * sizeof(klucz));

    int iloscKol = 0, iloscProstokatow = 0; // indeksy odpowiednio tablicach zagietych kol i prostokatow
    char typFigury;
    int indeksZagietejFigury;
    for (int i = 0; i < n; i++)
    {
        if (scanf(" %c", &typFigury) != 1)
            return;

        switch (typFigury) // switch w zaleznosci od typu figury jaka wczytujemy
        {
        case 'Z': // wczytujemy zagieta kartke

            if (scanf("%d", &indeksZagietejFigury) != 1)
                return;

            indeksZagietejFigury--; // indeksujemy od zera

            if (Klucze[indeksZagietejFigury].typ == 'P') // jesli mamy do czynienia z prostokatem
            {
                if (scanf("%lf %lf %lf %lf",
                          &ProstokatyZagiete[iloscProstokatow].prostaZaginajaca.punkt1.x,
                          &ProstokatyZagiete[iloscProstokatow].prostaZaginajaca.punkt1.y,
                          &ProstokatyZagiete[iloscProstokatow].prostaZaginajaca.punkt2.x,
                          &ProstokatyZagiete[iloscProstokatow].prostaZaginajaca.punkt2.y) == 0)
                {
                    free(KolaZagiete);
                    free(ProstokatyZagiete);
                    free(Klucze);
                    return;
                }

                ProstokatyZagiete[iloscProstokatow].prostokatBazowy =
                    ProstokatyZagiete[Klucze[indeksZagietejFigury].indeks].prostokatBazowy;
                ProstokatyZagiete[iloscProstokatow].indeksPrzedZagieciem =
                    indeksZagietejFigury;
                Klucze[i].indeks = iloscProstokatow;
                Klucze[i].typ = 'P';
                iloscProstokatow++;
            }
            else // w przeciwnym wypadku, jesli mamy do czynienia z kolem
            {
                if (scanf("%lf %lf %lf %lf",
                          &KolaZagiete[iloscKol].prosta.punkt1.x,
                          &KolaZagiete[iloscKol].prosta.punkt1.y,
                          &KolaZagiete[iloscKol].prosta.punkt2.x,
                          &KolaZagiete[iloscKol].prosta.punkt2.y) == 0)
                {
                    free(KolaZagiete);
                    free(ProstokatyZagiete);
                    free(Klucze);
                    return;
                }

                KolaZagiete[iloscKol].kolo = KolaZagiete[Klucze[indeksZagietejFigury].indeks].kolo;
                KolaZagiete[iloscKol].indeksPrzedZagieciem = indeksZagietejFigury;
                Klucze[i].indeks = iloscKol;
                Klucze[i].typ = 'K';
                iloscKol++;
            }

            break;
        case 'P': // wczytujemy niezagiety prostokat
            if (scanf("%lf %lf %lf %lf",
                      &ProstokatyZagiete[iloscProstokatow].prostokatBazowy.lewyDolny.x,
                      &ProstokatyZagiete[iloscProstokatow].prostokatBazowy.lewyDolny.y,
                      &ProstokatyZagiete[iloscProstokatow].prostokatBazowy.prawyGorny.x,
                      &ProstokatyZagiete[iloscProstokatow].prostokatBazowy.prawyGorny.y) == 0)
            {
                free(KolaZagiete);
                free(ProstokatyZagiete);
                free(Klucze);
                return;
            }

            ProstokatyZagiete[iloscProstokatow].indeksPrzedZagieciem = -1; // figura niezagieta - indeks to -1
            Klucze[i].indeks = iloscProstokatow;
            Klucze[i].typ = 'P';
            iloscProstokatow++;

            break;
        case 'K': // wczytujemy niezagiete kolo
            if (scanf("%lf %lf %lf",
                      &KolaZagiete[iloscKol].kolo.srodek.x,
                      &KolaZagiete[iloscKol].kolo.srodek.y,
                      &KolaZagiete[iloscKol].kolo.promien) == 0)
            {
                free(KolaZagiete);
                free(ProstokatyZagiete);
                free(Klucze);
                return;
            }

            KolaZagiete[iloscKol].indeksPrzedZagieciem = -1; // figura niezagieta - indeks to -1
            Klucze[i].indeks = iloscKol;
            Klucze[i].typ = 'K';
            iloscKol++;

            break;
        }
    }
}

/* Odbijamy punkt wzgledem prostej (chyba ze jest na prostej, to nie) zaginajacej i liczymy
ile "odbitych" punktow lacznie z niodbitymi znajduje sie w niezagietym prostokacie*/
int solveDlaProstokata(punkt Punkt, prostokatZgiety figura)
{

    if (figura.indeksPrzedZagieciem == -1) // jesli wbijamy szpile w niezagiety prostokat
        return czyJestWProstokacie(Punkt, figura.prostokatBazowy);

    int side = stronaProstej(Punkt, figura.prostaZaginajaca);
    if (side == -1)
        return 0; // prawa strona → nic nie przebija

    int prev = figura.indeksPrzedZagieciem;

    if (side == 0) // punkt lezy na prostej
        return solveDlaProstokata(Punkt, ProstokatyZagiete[Klucze[prev].indeks]);

    // lewa strona → suma warstw w punkcie i w odbitym punkcie
    return solveDlaProstokata(Punkt, ProstokatyZagiete[Klucze[prev].indeks]) +
           solveDlaProstokata(odbicieSymetryczne(Punkt, figura.prostaZaginajaca), ProstokatyZagiete[Klucze[prev].indeks]);
}

/*Zasada dzialania analogiczna jak solveDlaProstokata*/
int solveDlaKola(punkt Punkt, koloZgiete figura)
{

    if (figura.indeksPrzedZagieciem == -1) // jesli wbijamy szpile w niezagiete kolo
        return czyJestWKole(Punkt, figura.kolo);

    int side = stronaProstej(Punkt, figura.prosta);
    if (side == -1)
        return 0; // prawa strona → nic nie przebija

    int prev = figura.indeksPrzedZagieciem;

    if (side == 0)                                                    // punkt lezy na prostej
        return solveDlaKola(Punkt, KolaZagiete[Klucze[prev].indeks]); // na prostej → jak przed zgięciem

    // lewa strona → suma warstw w punkcie i w odbitym punkcie
    return solveDlaKola(Punkt, KolaZagiete[Klucze[prev].indeks]) +
           solveDlaKola(odbicieSymetryczne(Punkt, figura.prosta), KolaZagiete[Klucze[prev].indeks]);
}
int main(void)
{
    int n, q;   
    if (scanf("%d %d", &n, &q) == 0) // weryfikacja inputu
        return -1;

    wejscie(n);

    for (int i = 0; i < q; i++)
    {
        int numerZapytania; // w ktora kartke wbijamy szpile
        punkt zapytanie;    // punkt w ktory wbijamy szpile
        if (scanf("%d %lf %lf", &numerZapytania, &zapytanie.x, &zapytanie.y) == 0)
        {
            free(Klucze);
            free(KolaZagiete);
            free(ProstokatyZagiete);
            return -1;
        }
        numerZapytania--; // u nas kartki indeksowane sa od zera

        if (Klucze[numerZapytania].typ == 'P') // jesli wbijamy szpile w prostokat
        {
            int wynik = solveDlaProstokata(zapytanie, ProstokatyZagiete[Klucze[numerZapytania].indeks]);
            printf("%d\n", wynik);
        }
        else if (Klucze[numerZapytania].typ == 'K') // jesli wbijamy szpile w kolo
        {
            int wynik = solveDlaKola(zapytanie, KolaZagiete[Klucze[numerZapytania].indeks]);
            printf("%d\n", wynik);
        }
    }

    free(Klucze);
    free(KolaZagiete);
    free(ProstokatyZagiete);
    return 0;
}
