import subprocess
import matplotlib.pyplot as plt
import re
import sys
import os

# === KONFIGURACJA ===
# Ścieżka pobrana z Twojego zdjęcia:
exe_path = r"C:\Users\123pi\Desktop\zadanie5-program\zad5\x64\Debug\zad5.exe"

# Sprawdzenie czy plik istnieje
if not os.path.exists(exe_path):
    print(f"BŁĄD: Nie znaleziono pliku: {exe_path}")
    print("Upewnij się, że skompilowałeś projekt w Visual Studio!")
    sys.exit(1)

# Parametry testowe zgodne z zadaniem [cite: 6]
# Zmniejszyłem nieco największą liczbę, żeby test nie trwał wieki, 
# ale do ostatecznej dokumentacji użyj 3000000000
steps_list = [100000000, 1000000000, 3000000000] 
max_threads = 50 

results = {} 

print(f"Rozpoczynam testy programu: {exe_path}")
print("To może potrwać kilka minut. Nie zamykaj okna...")

for steps in steps_list:
    times = []
    threads_x = []
    print(f"\n--- Rozpoczynam serię dla {steps} kroków ---")
    
    for thread_count in range(1, max_threads + 1):
        # Dane wejściowe dla Twojego programu C++
        input_str = f"{steps}\n{thread_count}\n"
        
        try:
            process = subprocess.Popen(
                [exe_path],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )
            
            stdout, stderr = process.communicate(input=input_str)
            
            # Szukanie czasu w tym co wypisał program
            match = re.search(r"Czas obliczen:\s*([0-9.]+)", stdout)
            
            if match:
                elapsed_time = float(match.group(1))
                times.append(elapsed_time)
                threads_x.append(thread_count)
                # Wypisujemy postęp co 5 wątków, żeby nie zaśmiecać ekranu
                if thread_count % 5 == 0 or thread_count == 1:
                    print(f"  Wątki: {thread_count} -> Czas: {elapsed_time:.4f} s")
            else:
                print(f"  Błąd odczytu dla {thread_count} wątków. Program wypisał: {stdout}")
                
        except Exception as e:
            print(f"  Krytyczny błąd uruchomienia: {e}")
            break
    
    results[steps] = (threads_x, times)

# === RYSOWANIE WYKRESU [cite: 9] ===
print("\nRysuję wykres...")
plt.figure(figsize=(10, 6))

for steps, (threads, times) in results.items():
    plt.plot(threads, times, marker='o', label=f'Kroki: {steps}')

plt.title('Czas obliczeń PI w zależności od liczby wątków')
plt.xlabel('Liczba wątków')
plt.ylabel('Czas (sekundy)')
plt.grid(True)
plt.legend()

output_filename = "wykres_pi.png"
plt.savefig(output_filename)
print(f"Gotowe! Wykres zapisano jako: {output_filename}")
plt.show()