/**
 * @file main.cpp
 * @brief Plik glowny programu obliczajacego liczbe PI metoda calkowania numerycznego.
 *
 * Program wykorzystuje wielowatkowosc (std::thread) oraz wyrazenia lambda
 * do rownoleglego obliczania calki oznaczonej z funkcji 4/(1+x^2).
 * Czas obliczen jest mierzony i wyswietlany na standardowym wyjsciu.
 *
 * @author Autor Projektu
 * @date 2026-02-03
 */

#include <iostream>
#include <vector>
#include <thread>
#include <cmath>
#include <chrono>   // Do mierzenia czasu
#include <iomanip>  // Do formatowania wyjscia (setprecision)
#include <functional>

/**
 * @brief Glowna funkcja programu.
 *
 * Pobiera od uzytkownika liczbe krokow oraz liczbe watkow,
 * a nastepnie uruchamia obliczenia w trybie wielowatkowym.
 * Na koncu sumuje wyniki czastkowe i wyswietla wynik koncowy oraz czas obliczen.
 *
 * @return Zwraca 0 w przypadku powodzenia, -1 w przypadku bledu danych wejsciowych.
 */
int main() {
    /// Zmienna przechowujaca calkowita ilosc krokow podzialu calki.
    long long num_steps; 
    
    /// Zmienna przechowujaca zadana przez uzytkownika ilosc watkow.
    int num_threads;     

    [cite_start]// 1. Pobranie danych od uzytkownika [cite: 3]
    std::cout << "Podaj liczbe krokow (np. 1000000000): ";
    if (!(std::cin >> num_steps)) return -1;
    
    std::cout << "Podaj liczbe watkow (np. 1-50): ";
    if (!(std::cin >> num_threads)) return -1;

    // Zabezpieczenie przed liczba watkow wieksza niz liczba krokow
    if (num_threads > num_steps) num_threads = num_steps;
    if (num_threads < 1) num_threads = 1;

    /// Szerokosc pojedynczego prostokata w metodzie calkowania.
    double step = 1.0 / (double)num_steps;
    
    // Wektor na wyniki czesciowe z kazdego watku
    // Dzieki temu unikamy uzywania Mutexow w petli (co spowolniloby program)
    /// Wektor przechowujacy sumy czesciowe obliczone przez poszczegolne watki.
    std::vector<double> partial_sums(num_threads, 0.0);
    
    /// Wektor obiektow watkow.
    std::vector<std::thread> threads;

    // Rozpoczecie pomiaru czasu
    auto start_time = std::chrono::high_resolution_clock::now();

    // 2. Definicja wyrazenia LAMBDA dla pracy watku 
    /**
     * @brief Wyrazenie lambda realizujace obliczenia dla pojedynczego watku.
     * * Funkcja oblicza sume wartosci funkcji w zadanym przedziale indeksow
     * stosujac metode punktu srodkowego (Midpoint Rule).
     * * @param thread_id Identyfikator watku (indeks w wektorze wynikow).
     * @param start_index Indeks poczatkowy dla danego watku.
     * @param end_index Indeks koncowy dla danego watku.
     */
    auto worker = [&](int thread_id, long long start_index, long long end_index) {
        double local_sum = 0.0;
        for (long long i = start_index; i < end_index; ++i) {
            // Obliczenie x dla srodka przedzialu (i + 0.5)
            double x = (i + 0.5) * step;
            local_sum += 4.0 / (1.0 + x * x);
        }
        partial_sums[thread_id] = local_sum;
    };

    // 3. Rozdzielenie pracy i uruchomienie watkow 
    long long chunk_size = num_steps / num_threads;
    long long remainder = num_steps % num_threads;
    long long current_start = 0;

    for (int i = 0; i < num_threads; ++i) {
        long long current_end = current_start + chunk_size;
        
        // Dodajemy reszte z dzielenia do ostatniego lub po trochu do pierwszych watkow
        // Zapewnia to rownomierne rozlozenie obciazenia
        if (i < remainder) {
            current_end++;
        }
        
        // Tworzenie watku z uzyciem lambdy i przekazanie argumentow
        threads.emplace_back(worker, i, current_start, current_end);
        
        current_start = current_end;
    }

    // 4. Oczekiwanie na zakonczenie watkow (join)
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    // Sumowanie wynikow czesciowych
    double pi = 0.0;
    for (double val : partial_sums) {
        pi += val;
    }
    pi = pi * step;

    // Zakonczenie pomiaru czasu
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    // 5. Wypisanie wynikow 
    std::cout << std::fixed << std::setprecision(15); // Ustawienie precyzji wyswietlania
    std::cout << "Obliczone PI: " << pi << std::endl;
    std::cout << "Czas obliczen: " << elapsed.count() << " s" << std::endl;
    std::cout << "Liczba watkow: " << num_threads << std::endl;
    std::cout << "Liczba krokow: " << num_steps << std::endl;

    return 0;
}