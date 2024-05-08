#include <iostream>
#define _USE_MATH_DEFINES // for C++
#include <math.h>
#include <fstream>

using namespace std;

float S = 0.5, K = 1, T = 1;
double h = 0, g = 0;

void ustaw_dane(int* liczba_iter, int* prz_poszuk, int* liczba_pop, float* prawd_krzyz, float* prawd_mutacji)
{
    cout << "Podaj:\n"
         << "Liczba iteracji: ";
    cin >> *liczba_iter;
    cout << "Przestrzen poszukiwan: ";
    cin >> *prz_poszuk;
    cout << "Liczba populacji: ";
    cin >> *liczba_pop;
    cout << "Prawdopodobienstwo krzyzowania: ";
    cin >> *prawd_krzyz;
    cout << "Prawdopodobienstwo mutacji: ";
    cin >> *prawd_mutacji;
}

double skokowa(int liczba_iter, int prz_poszuk, int liczba_pop, float prawd_krzyz, float prawd_mutacji, float t)
{
    h = K * (1 - pow(M_E, -(S * t / T)) / (sqrt(1 - pow(S, 2))) * sin(sqrt(1 - pow(S, 2)) * t / T + atan(sqrt(1 - pow(S, 2)) / S)));
    return h;
}

double impulsowa(int liczba_iter, int prz_poszuk, int liczba_pop, float prawd_krzyz, float prawd_mutacji, float t)
{
    g = (K / T * sqrt(1 - pow(S, 2)) * pow(M_E, -(S * t / T)) * sin(sqrt(1 - pow(S, 2) * t)));
    return g;
}

int main()
{
    ofstream file1("skokowa_wyniki.csv");
    ofstream file2("impulsowa_wyniki.csv");

    if (!file1.is_open() || !file2.is_open()) {
        cerr << "Nie można otworzyć jednego z plików\n";
        return 1;
    }

    file1 << "Czas,Skokowa_h\n";
    file2 << "Czas,Impulsowa_g\n";

    int liczba_iter = 0, prz_poszuk = 0, liczba_pop = 0;
    float t = 0, prawd_krzyz = 0, prawd_mutacji = 0;

    ustaw_dane(&liczba_iter, &prz_poszuk, &liczba_pop, &prawd_krzyz, &prawd_mutacji);

    double skokowa_h = 0;
    double impulsowa_g = 0;

    // Skokowa
    cout << "\nSkokowa_h:\n";
    for (int i = 0; i <= 500; i++)
    {
        t += 0.016667;
        skokowa_h = skokowa(liczba_iter, prz_poszuk, liczba_pop, prawd_krzyz, prawd_mutacji, t);
        cout << skokowa_h << "\n";
        file1 << t << "," << skokowa_h << "\n";
    }

    // Impulsowa
    t = 0;
    cout << "\n\nImpulsowa_g:\n";
    for (int i = 0; i <= 230; i++)
    {
        t += 0.016667;
        impulsowa_g = impulsowa(liczba_iter, prz_poszuk, liczba_pop, prawd_krzyz, prawd_mutacji, t);
        cout << impulsowa_g << "\n";
        file2 << t << "," << impulsowa_g << "\n";
    }

    file1.close();
    file2.close();

    cout << "Dane zapisane do plikow skokowa_data.csv oraz impulsowa_data.csv\n";

    return 0;
}
