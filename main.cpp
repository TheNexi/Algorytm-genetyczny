#include <iostream>
#include <cmath>
#include <fstream>
#include <random>

using namespace std;

const double M_E = 2.71828;

//Inicjalizacja generatora
random_device rd;
mt19937 gen(rd());

float S = 0.5, K = 1, T = 1;
double h = 0, g = 0;

void ustaw_dane(int* liczba_iter, float* prz_poszuk_k, float* prz_poszuk_t, int* liczba_pop, float* prawd_krzyz, float* prawd_mutacji)
{
    cout << "Podaj:\n"
        << "Liczba iteracji: ";
    cin >> *liczba_iter;
    cout << "Przestrzen poszukiwan K od 0 do: ";
    cin >> *prz_poszuk_k;
    cout << "Przestrzen poszukiwan T od 0 do: ";
    cin >> *prz_poszuk_t;
    cout << "Liczba populacji: ";
    cin >> *liczba_pop;
    cout << "Prawdopodobienstwo krzyzowania(od 0 do 1): ";
    cin >> *prawd_krzyz;
    cout << "Prawdopodobienstwo mutacji(od 0 do 1): ";
    cin >> *prawd_mutacji;
}

double skokowa(float t, double K, double T)
{
    h = K * (1 - pow(M_E, -(S * t / T))) / (sqrt(1 - pow(S, 2))) * sin(sqrt(1 - pow(S, 2)) * t / T + atan(sqrt(1 - pow(S, 2)) / S));
    return h;
}

double impulsowa(float t, double K, double T)
{
    g = (K / T * sqrt(1 - pow(S, 2)) * pow(M_E, -(S * t / T)) * sin(sqrt(1 - pow(S, 2)) * t));
    return g;
}

double obliczJ(double* imp, double* skok)
{
    double suma_imp = 0, suma_skok = 0, wynik = 0;
    int licznik = 0;
    double pierwszy_czlon = 0, drugi_czlon = 0;
    for (int i = 0; i < 200; i++)
    {
        suma_imp += imp[i];
        suma_skok += skok[i];

        pierwszy_czlon += pow((imp[i] - (suma_imp / (i + 1))), 2);
        drugi_czlon += pow((skok[i] - (suma_skok / (i + 1))), 2);
    }

    return pierwszy_czlon + drugi_czlon;
}

double obliczJ(const std::vector<double>& imp, const std::vector<double>& skok, int liczba_iteracji)
{
    double suma_imp = 0, suma_skok = 0, wynik = 0;
    int licznik = 0;
    double pierwszy_czlon = 0, drugi_czlon = 0;
    for (int i = 0; i < liczba_iteracji; i++)
    {
        suma_imp += imp[i];
        suma_skok += skok[i];

        pierwszy_czlon += pow((imp[i] - (suma_imp / (i + 1))), 2);
        drugi_czlon += pow((skok[i] - (suma_skok / (i + 1))), 2);
    }

    return pierwszy_czlon + drugi_czlon;
}

struct Osobnik {
    double K;
    double T;
    double J;
    vector<double> elementy_skok_osobnik;
    vector<double> elementy_imp_osobnik;
};

void mutacja(Osobnik& osobnik, float prawd_mutacji, float prz_poszuk_k, float prz_poszuk_t)
{
    uniform_real_distribution<double> dist(0.0, 1.0);

    // Mutacja parametru K
    if (dist(gen) < prawd_mutacji) {
        osobnik.K = uniform_real_distribution<double>(0.0, prz_poszuk_k)(gen);
    }

    // Mutacja parametru T
    if (dist(gen) < prawd_mutacji) {
        osobnik.T = uniform_real_distribution<double>(0.0, prz_poszuk_t)(gen);
    }
}

void krzyzowanie(Osobnik& osobnik1, Osobnik& osobnik2, float prawd_krzyz)
{
    uniform_real_distribution<double> dist(0.0, 1.0);
    double losowa_liczba = dist(gen);

    if (losowa_liczba > prawd_krzyz) 
    {
        double losowa_liczba_2 = dist(gen);

        if (losowa_liczba_2 < 0.5) 
        {
            double temp_K = osobnik1.K;
            osobnik1.K = osobnik2.K;
            osobnik2.K = temp_K;
        }
        else 
        {
            double temp_T = osobnik1.T;
            osobnik1.T = osobnik2.T;
            osobnik2.T = temp_T;
        }
    }
}



int main()
{
    int liczba_iter = 0, liczba_pop = 0;
    float t = 0, prawd_krzyz = 0, prawd_mutacji = 0, prz_poszuk_k = 0, prz_poszuk_t = 0;
    double elementy_skok[300], elementy_imp[300];
    
    ofstream file1("skokowa_wyniki.csv");
    ofstream file2("impulsowa_wyniki.csv");
    ofstream file3("wyniki_osobnikow.csv");

    if (!file1.is_open() || !file2.is_open() || !file3.is_open()) 
    {
        cerr << "Nie można otworzyć jednego z plików\n";
        return 1;
    }

    file1 << "Czas,Skokowa_h\n";
    file2 << "Czas,Impulsowa_g\n";
    file3 << "K,T,J\n";
    

    ustaw_dane(&liczba_iter, &prz_poszuk_k, &prz_poszuk_t, &liczba_pop, &prawd_krzyz, &prawd_mutacji);

    if (prawd_krzyz < 0 || prawd_krzyz > 1)
    {
        cout << "Bledna wartosc dla prawdopodobieństwa krzyzowania <0.0:1.0>\n";
        return 1;
    }
    if (prawd_mutacji < 0 || prawd_mutacji > 1)
    {
        cout << "Bledna wartosc dla prawdopodobieństwa mutacji <0.0:1>\n";
        return 1;
    }
    if (prz_poszuk_k < 0 || prz_poszuk_k > 500)
    {
        cout << "Bledna wartosc dla przestrzeni poszukiwan k <0.0:500.0>\n";
        return 1;
    }
    if (prz_poszuk_t < 0 || prz_poszuk_t > 500)
    {
        cout << "Bledna wartosc dla przestrzeni poszukiwan t <0.0:500.0>\n";
        return 1;
    }

    double skokowa_h = 0;
    double impulsowa_g = 0;
    
    // Skokowa populacji
    //cout << "\nSkokowa_h:\n";
    for (int i = 0; i < 300; i++)
    {
        skokowa_h = skokowa(t, K, T);
        //cout << skokowa_h << "\n";
        elementy_skok[i] = skokowa_h;
        file1 << t << "," << skokowa_h << "\n";
        t += 0.016667;
    }

    // Impulsowa populacji
    t = 0;
    //cout << "\n\nImpulsowa_g:\n";
    for (int i = 0; i < 300; i++)
    {
        impulsowa_g = impulsowa(t, K, T);
        //cout << impulsowa_g << "\n";
        elementy_imp[i] = impulsowa_g;
        file2 << t << "," << impulsowa_g << "\n";
        t += 0.016667; 
    }
    t = 0;

    Osobnik najlepszy;
    vector<Osobnik> populacja(liczba_pop);
    najlepszy.J = numeric_limits<double>::max(); //maksymalna wartość początkowa

    for (int i = 0; i < liczba_pop; i++)
    {
        Osobnik osobnik;
        osobnik.elementy_skok_osobnik.resize(liczba_iter);
        osobnik.elementy_imp_osobnik.resize(liczba_iter);
        uniform_real_distribution<double> distK(0.0, prz_poszuk_k);
        uniform_real_distribution<double> distT(0.0, prz_poszuk_t);
        
        osobnik.K = distK(gen);
        osobnik.T = distT(gen);

        //mutacja
        mutacja(osobnik, prawd_mutacji, prz_poszuk_k, prz_poszuk_t);

        //krzyżowanie
        for (int i = 0; i < populacja.size() - 1; i++)
        {
            Osobnik& aktualny_osobnik = populacja[i];
            Osobnik& kolejny_osobnik = populacja[i + 1];
            krzyzowanie(aktualny_osobnik, kolejny_osobnik, prawd_krzyz);
        }

        for (int iter = 0; iter < liczba_iter; iter++)
        {
            osobnik.elementy_skok_osobnik[iter] = skokowa(t, osobnik.K, osobnik.T);
            osobnik.elementy_imp_osobnik[iter] = impulsowa(t, osobnik.K, osobnik.T);
            t += 0.016667;
        }
        t = 0;

        osobnik.J = obliczJ(osobnik.elementy_skok_osobnik, osobnik.elementy_imp_osobnik, liczba_iter);
        file3 << round(osobnik.K * 1000) / 1000.0 << "," << round(osobnik.T * 1000) / 1000.0 << "," << round(osobnik.J * 1000) / 1000.0 << "\n";

        // Aktualizacja najlepszego osobnika
        if (osobnik.J < najlepszy.J)
        {
            najlepszy = osobnik;
        }
    }

    cout << "Najlepszy osobnik:\nK: " << najlepszy.K << "\nT: " << najlepszy.T << "\nJ: " << najlepszy.J << "\n";

    file1.close();
    file2.close();
    file3.close();

    cout << "Dane zapisane do plikow skokowa_wyniki.csv, impulsowa_wyniki.csv oraz wyniki_osobnikow.csv\n";

    return 0;
}
