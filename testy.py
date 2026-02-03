## @file testy.py
## @brief Skrypt automatyzujacy testy wydajnosciowe programu C++.
##
## Skrypt uruchamia zewnetrzny program obliczajacy PI dla roznych liczby watkow
## i krokow, zbiera czasy wykonania, a nastepnie generuje wykres wydajnosci.
##
## @author Autor Projektu
## @date 2026-02-03

import subprocess
import matplotlib.pyplot as plt
import re
import sys
import os

## === KONFIGURACJA ===
## Sciezka do pliku wykonywalnego programu C++
exe_path = r"C:\Users\123pi\Desktop\zadanie5-program\zad5\x64\Debug\zad5.exe"

## Sprawdzenie czy plik wykonywalny istnieje
if not os.path.exists(exe_path):
    print(f"BLAD: Nie znaleziono pliku: {exe_path}")
    print("Upewnij sie, ze skompilowales projekt w Visual Studio!")
    sys.exit(1)

## Parametry testowe zgodne z zadaniem
## Lista krokow (dokladnosci), dla ktorych beda przeprowadzane testy.
## Zmniejszylem nieco najwieksza liczbe, zeby test nie trwal wieki, 
## ale do ostatecznej dokumentacji uzyj 3000000000.
steps_list = [100000000, 1000000000, 3000000000] 

## Maksymalna liczba watkow do przetestowania.
max_threads = 50 

## Slownik do przechowywania wynikow testow.
results = {} 

print(f"Rozpoczynam testy programu: {exe_path}")
print("To moze potrwac kilka minut. Nie zamykaj okna...")

## Glowna petla iterujaca po liscie krokow
for steps in steps_list:
    times = []
    threads_x = []
    print(f"\n--- Rozpoczynam serie dla {steps} krokow ---")
    
    ## Petla iterujaca po liczbie watkow od 1 do max_threads
    for thread_count in range(1, max_threads + 1):
        ## Dane wejsciowe dla programu C++ (liczba krokow i liczba watkow)
        input_str = f"{steps}\n{thread_count}\n"
        
        try:
            ## Uruchomienie procesu zewnetrznego
            process = subprocess.Popen(
                [exe_path],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )
            
            ## Wyslanie danych i odebranie wynikow
            stdout, stderr = process.communicate(input=input_str)
            
            ## Szukanie czasu w tym co wypisal program
            ## Wykorzystanie wyrazenia regularnego do znalezienia liczby po "Czas obliczen:"
            match = re.search(r"Czas obliczen:\s*([0-9.]+)", stdout)
            
            if match:
                elapsed_time = float(match.group(1))
                times.append(elapsed_time)
                threads_x.append(thread_count)
                
                ## Wypisujemy postep co 5 watkow, zeby nie zasmiecac ekranu
                if thread_count % 5 == 0 or thread_count == 1:
                    print(f"  Watki: {thread_count} -> Czas: {elapsed_time:.4f} s")
            else:
                print(f"  Blad odczytu dla {thread_count} watkow. Program wypisal: {stdout}")
                
        except Exception as e:
            print(f"  Krytyczny blad uruchomienia: {e}")
            break
    
    ## Zapisanie wynikow dla danej liczby krokow
    results[steps] = (threads_x, times)

## === RYSOWANIE WYKRESU ===
print("\nRysuje wykres...")
plt.figure(figsize=(10, 6))

## Iteracja po wynikach i dodawanie serii danych do wykresu
for steps, (threads, times) in results.items():
    plt.plot(threads, times, marker='o', label=f'Kroki: {steps}')

## Konfiguracja wygladu wykresu
plt.title('Czas obliczen PI w zaleznosci od liczby watkow')
plt.xlabel('Liczba watkow')
plt.ylabel('Czas (sekundy)')
plt.grid(True)
plt.legend()

## Zapisanie wykresu do pliku
output_filename = "wykres_pi.png"
plt.savefig(output_filename)
print(f"Gotowe! Wykres zapisano jako: {output_filename}")
plt.show()