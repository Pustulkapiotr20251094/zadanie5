#include <iostream>
#include <vector>
#include <thread>
#include <cmath>
#include <chrono>   // Do mierzenia czasu
#include <iomanip>  // Do formatowania wyjścia (setprecision)
#include <functional>

// Funkcja główna
int main() {
    long long num_steps; // Ilość liczb podziału (używamy long long dla dużych liczb)
    int num_threads;     // Ilość wątków

    // 1. Pobranie danych od użytkownika [cite: 3]
    std::cout << "Podaj liczbe krokow (np. 1000000000): ";
    if (!(std::cin >> num_steps)) return -1;
    
    std::cout << "Podaj liczbe watkow (np. 1-50): ";
    if (!(std::cin >> num_threads)) return -1;

    // Zabezpieczenie przed liczbą wątków większą niż liczba kroków
    if (num_threads > num_steps) num_threads = num_steps;
    if (num_threads < 1) num_threads = 1;

    double step = 1.0 / (double)num_steps;
    
    // Wektor na wyniki częściowe z każdego wątku
    // Dzięki temu unikamy używania Mutexów w pętli (co spowolniłoby program)
    std::vector<double> partial_sums(num_threads, 0.0);
    std::vector<std::thread> threads;

    // Rozpoczęcie pomiaru czasu
    auto start_time = std::chrono::high_resolution_clock::now();

    // 2. Definicja wyrażenia LAMBDA dla pracy wątku 
    // Przejmuje zmienne przez referencję [&], ale argumenty dostaje własne
    auto worker = [&](int thread_id, long long start_index, long long end_index) {
        double local_sum = 0.0;
        for (long long i = start_index; i < end_index; ++i) {
            double x = (i + 0.5) * step;
            local_sum += 4.0 / (1.0 + x * x);
        }
        partial_sums[thread_id] = local_sum;
    };

    // 3. Rozdzielenie pracy i uruchomienie wątków 
    long long chunk_size = num_steps / num_threads;
    long long remainder = num_steps % num_threads;
    long long current_start = 0;

    for (int i = 0; i < num_threads; ++i) {
        long long current_end = current_start + chunk_size;
        // Dodajemy resztę z dzielenia do ostatniego lub po trochu do pierwszych wątków
        if (i < remainder) {
            current_end++;
        }
        
        // Tworzenie wątku z użyciem lambdy
        threads.emplace_back(worker, i, current_start, current_end);
        
        current_start = current_end;
    }

    // 4. Oczekiwanie na zakończenie wątków (join)
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    // Sumowanie wyników częściowych
    double pi = 0.0;
    for (double val : partial_sums) {
        pi += val;
    }
    pi = pi * step;

    // Zakończenie pomiaru czasu
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    // 5. Wypisanie wyników 
    std::cout << std::fixed << std::setprecision(15); // Ustawienie precyzji wyświetlania
    std::cout << "Obliczone PI: " << pi << std::endl;
    std::cout << "Czas obliczen: " << elapsed.count() << " s" << std::endl;
    std::cout << "Liczba watkow: " << num_threads << std::endl;
    std::cout << "Liczba krokow: " << num_steps << std::endl;

    return 0;
}