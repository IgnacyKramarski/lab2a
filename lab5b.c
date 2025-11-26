#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct klucz {
    int indeks; // indeks w tablicy prostokatow badz kół
    char typ;   // 'P' lub 'K'
} klucz;

typedef struct punkt {
    double x;
    double y;
} punkt;

typedef struct prostokat {
    punkt lewyDolny;
    punkt prawyGorny;
} prostokat;

typedef struct prosta {
    punkt punkt1;
    punkt punkt2;
} prosta;

typedef struct prostokatZgiety {
    prostokat prostokat;
    prosta prosta;
    int indeksPrzedZagieciem;
} prostokatZgiety;

typedef struct kolo {
    double promien;
    punkt srodek;
} kolo;

typedef struct koloZgiete {
    kolo kolo;
    prosta prosta;
    int indeksPrzedZagieciem;
} koloZgiete;

koloZgiete *KolaZagiete = NULL;
klucz *Klucze = NULL;
prostokatZgiety *ProstokatyZagiete = NULL;

int czyRowne(double x, double y) {
    const double epsilon = 1e-9L;
    return fabsl(x - y) < epsilon;
}

int czyJestWKole(punkt P, kolo Figura) {
    double dx = P.x - Figura.srodek.x;
    double dy = P.y - Figura.srodek.y;
    double odleglosc = dx*dx + dy*dy;
    return odleglosc <= Figura.promien * Figura.promien;
}

int czyJestWProstokacie(punkt P, prostokat Figura) {
    return (Figura.lewyDolny.x <= P.x && P.x <= Figura.prawyGorny.x &&
            Figura.lewyDolny.y <= P.y && P.y <= Figura.prawyGorny.y);
}

// -1 prawa, 0 na prostej, +1 lewa
int stronaProstej(punkt P, prosta Prosta) {
    double vx = Prosta.punkt2.x - Prosta.punkt1.x;
    double vy = Prosta.punkt2.y - Prosta.punkt1.y;
    double wx = P.x - Prosta.punkt1.x;
    double wy = P.y - Prosta.punkt1.y;
    double cross = vx*wy - vy*wx;
    if (czyRowne(cross, 0.0L)) return 0;
    return (cross > 0) ? +1 : -1;
}

punkt odbicieSymetryczne(punkt P, prosta Prosta) {
    double dx = Prosta.punkt2.x - Prosta.punkt1.x;
    double dy = Prosta.punkt2.y - Prosta.punkt1.y;
    double vx = P.x - Prosta.punkt1.x;
    double vy = P.y - Prosta.punkt1.y;
    double t = (vx*dx + vy*dy) / (dx*dx + dy*dy);
    double rx = Prosta.punkt1.x + t*dx;
    double ry = Prosta.punkt1.y + t*dy;
    punkt Odbicie;
    Odbicie.x = 2*rx - P.x;
    Odbicie.y = 2*ry - P.y;
    return Odbicie;
}

void wejscie(int n) {
    KolaZagiete = malloc((size_t)n * sizeof(koloZgiete));
    ProstokatyZagiete = malloc((size_t)n * sizeof(prostokatZgiety));
    Klucze = malloc((size_t)n * sizeof(klucz));

    int iloscKol = 0, iloscProstokatow = 0;
    char typFigury;
    int indeksZagietejFigury;
    for (int i = 0; i < n; i++) {
        if (scanf(" %c", &typFigury) != 1) return;
        switch (typFigury) {
        case 'Z':
            if (scanf("%d", &indeksZagietejFigury) != 1) return;
            indeksZagietejFigury--;
            if (Klucze[indeksZagietejFigury].typ == 'P') {
                scanf("%lf %lf %lf %lf",
                      &ProstokatyZagiete[iloscProstokatow].prosta.punkt1.x,
                      &ProstokatyZagiete[iloscProstokatow].prosta.punkt1.y,
                      &ProstokatyZagiete[iloscProstokatow].prosta.punkt2.x,
                      &ProstokatyZagiete[iloscProstokatow].prosta.punkt2.y);
                ProstokatyZagiete[iloscProstokatow].prostokat =
                    ProstokatyZagiete[Klucze[indeksZagietejFigury].indeks].prostokat;
                ProstokatyZagiete[iloscProstokatow].indeksPrzedZagieciem =
                    indeksZagietejFigury;
                Klucze[i].indeks = iloscProstokatow;
                Klucze[i].typ = 'P';
                iloscProstokatow++;
            } else {
                scanf("%lf %lf %lf %lf",
                      &KolaZagiete[iloscKol].prosta.punkt1.x,
                      &KolaZagiete[iloscKol].prosta.punkt1.y,
                      &KolaZagiete[iloscKol].prosta.punkt2.x,
                      &KolaZagiete[iloscKol].prosta.punkt2.y);
                KolaZagiete[iloscKol].kolo =
                    KolaZagiete[Klucze[indeksZagietejFigury].indeks].kolo;
                KolaZagiete[iloscKol].indeksPrzedZagieciem =
                    indeksZagietejFigury;
                Klucze[i].indeks = iloscKol;
                Klucze[i].typ = 'K';
                iloscKol++;
            }
            break;
        case 'P':
            scanf("%lf %lf %lf %lf",
                  &ProstokatyZagiete[iloscProstokatow].prostokat.lewyDolny.x,
                  &ProstokatyZagiete[iloscProstokatow].prostokat.lewyDolny.y,
                  &ProstokatyZagiete[iloscProstokatow].prostokat.prawyGorny.x,
                  &ProstokatyZagiete[iloscProstokatow].prostokat.prawyGorny.y);
            ProstokatyZagiete[iloscProstokatow].indeksPrzedZagieciem = -1;
            Klucze[i].indeks = iloscProstokatow;
            Klucze[i].typ = 'P';
            iloscProstokatow++;
            break;
        case 'K':
            scanf("%lf %lf %lf",
                  &KolaZagiete[iloscKol].kolo.srodek.x,
                  &KolaZagiete[iloscKol].kolo.srodek.y,
                  &KolaZagiete[iloscKol].kolo.promien);
            KolaZagiete[iloscKol].indeksPrzedZagieciem = -1;
            Klucze[i].indeks = iloscKol;
            Klucze[i].typ = 'K';
            iloscKol++;
            break;
        }
    }
}

int solveDlaProstokata(punkt Punkt, prostokatZgiety figura) {
    //printf("\n");
    //printf("%d\n", figura.indeksPrzedZagieciem);


    if (figura.indeksPrzedZagieciem == -1){
        //printf("twoja stara to szmata\n");
        //printf("%d\n", czyJestWProstokacie(Punkt, figura.prostokat));
        //printf("twoja stara to ciagle szmata\n");
        //printf("%d\n", figura.prostokat.lewyDolny.x);
        //printf("twoja stara to ciagle szmata2\n");
        //printf("%d\n", figura.prostokat.lewyDolny.y);
        //printf("twoja stara to ciagle szmata2\n");

        return czyJestWProstokacie(Punkt, figura.prostokat);
    }

    int side = stronaProstej(Punkt, figura.prosta);
    if (side == -1) return 0;
    int prev = figura.indeksPrzedZagieciem;

    //punkt lezy na prostej
    if (side == 0) return solveDlaProstokata(Punkt, ProstokatyZagiete[Klucze[prev].indeks]);
    return solveDlaProstokata(Punkt, ProstokatyZagiete[Klucze[prev].indeks])
         + solveDlaProstokata(odbicieSymetryczne(Punkt, figura.prosta), ProstokatyZagiete[Klucze[prev].indeks]);
}


int solveDlaKola(punkt Punkt, koloZgiete figura) {
    //printf("\n");
    //printf("%d\n", figura.indeksPrzedZagieciem);

    if (figura.indeksPrzedZagieciem == -1)
        return czyJestWKole(Punkt, figura.kolo);

    int side = stronaProstej(Punkt, figura.prosta);
    if (side == -1)
        return 0; // prawa strona → nic nie przebija

    int prev = figura.indeksPrzedZagieciem;
    //punkt lezy na prostej
    if (side == 0)
        return solveDlaKola(Punkt, KolaZagiete[Klucze[prev].indeks]); // na prostej → jak przed zgięciem

    // lewa strona → suma warstw w punkcie i w odbitym punkcie
    return solveDlaKola(Punkt, KolaZagiete[Klucze[prev].indeks])
         + solveDlaKola(odbicieSymetryczne(Punkt, figura.prosta),
                        KolaZagiete[Klucze[prev].indeks]);
}
int main(void) {
    int n, q;
    if (scanf("%d %d", &n, &q) != 2) {
        fprintf(stderr, "Błąd wczytywania n i q\n");
        return 1;
    }

    // wczytanie opisów kartek
    wejscie(n);

    for (int i = 0; i < q; i++) {
        int numerZapytania;
        punkt zapytanie;
        if (scanf("%d %lf %lf", &numerZapytania, &zapytanie.x, &zapytanie.y) != 3) {
            fprintf(stderr, "Błąd wczytywania zapytania\n");
            free(Klucze);
            free(KolaZagiete);
            free(ProstokatyZagiete);
            return 1;
        }
        numerZapytania--; // indeksujemy od zera

        if (Klucze[numerZapytania].typ == 'P') {
            int wynik = solveDlaProstokata(zapytanie,
                                           ProstokatyZagiete[Klucze[numerZapytania].indeks]);
            printf("%d\n", wynik);
            //printf("nizej wyniki");
            //printf("%d\n", Klucze[numerZapytania].indeks);
            //printf("%nizej prostokat\n");
            //printf("%lf\n", ProstokatyZagiete[0].prostokat.lewyDolny.x);
        } else if (Klucze[numerZapytania].typ == 'K') {
            int wynik = solveDlaKola(zapytanie,
                                     KolaZagiete[Klucze[numerZapytania].indeks]);
            printf("%d\n", wynik);
        }
    }

    free(Klucze);
    free(KolaZagiete);
    free(ProstokatyZagiete);
    return 0;
}


