# Rozproszone systemy pomiarowe

Repozytorium startowe do projektu z systemów rozproszonych.  
Projekt dotyczy budowy rozproszonego systemu pomiarowego, w którym urządzenia ESP32 zbierają dane z czujników, publikują je do brokera MQTT, a następnie dane są odbierane przez serwisy backendowe, zapisywane do bazy danych i udostępniane przez REST API.

Aktualnie projekt zawiera przygotowane serwisy backendowe uruchamiane przez Docker Compose oraz katalogi na kolejne elementy systemu, takie jak:
- `esp32`
- `ingestor`
- `ui`
- `docs`

---

## Quick Start

Poniższa instrukcja pozwala uruchomić podstawową wersję środowiska projektowego dla systemu rozproszonego.

### Wymagania

Przed uruchomieniem upewnij się, że masz zainstalowane:
- Docker
- Docker Compose

### Klonowanie repozytorium

```bash
git clone https://github.com/mateuszbartczak-pwr/Rozproszone-systemy-pomiarowe.git
cd Rozproszone-systemy-pomiarowe
```

### Uruchomienie środowiska
Aby zbudować i uruchomić wszystkie dostępne serwisy:
```bash
docker compose up --build
```
lub aby uruchomić środowisko w tle:
```bash
docker compose up -d --build
```
### Zatrzymanie środowiska
```bash
docker compose down
```

### Aktualnie dostępne serwisy

Po uruchomieniu Docker Compose powinny być dostępne następujące usługi:

- REST API (Flask) — http://localhost:5001

- Broker MQTT — localhost:1883

- PostgreSQL — localhost:5432

### Podgląd logów

Aby sprawdzić logi wszystkich serwisów:
```bash
docker compose logs
```

Aby śledzić logi na żywo:
```bash
docker compose logs -f
```

Aby wyświetlić logi tylko jednego serwisu:
```bash
docker compose logs -f flask
docker compose logs -f broker
docker compose logs -f database
```
Sprawdzenie statusu kontenerów
```bash
docker compose ps
```

### Struktura projektu

Repozytorium zawiera między innymi następujące katalogi:

- `api/` — backend REST API

- `broker/` — broker MQTT

- `database/` — baza danych PostgreSQL

- `esp32/` — kod dla urządzeń ESP32

- `ingestor/` — serwis odbierający dane z MQTT i zapisujący je do bazy

- `ui/` — warstwa interfejsu użytkownika

- `docs/` — dokumentacja projektu

- `utils/` — narzędzia pomocnicze

### Uwagi

Projekt będzie rozwijany etapami w trakcie semestru.
W kolejnych zajęciach repozytorium będzie rozszerzane o dodatkowe serwisy, integracje i mechanizmy bezpieczeństwa.

## Lab03
Utworzenie kodu esp32 publikującego dane json'owe do brokera

### Wykonane zadania

### 1. Konfiguracja środowiska (PlatformIO)
W pliku `platformio.ini` zdefiniowano środowisko dla płytki `esp32dev` oraz dodano niezbędne biblioteki do obsługi MQTT i formatu JSON:
* `knolleary/PubSubClient` (do komunikacji MQTT)
* `bblanchon/ArduinoJson` (do budowania paczek z danymi)

### 2. Konfiguracja poświadczeń (`secrets.h`)
Utworzono plik `include/secrets.h`, który zawiera:
* SSID i hasło do sieci Wi-Fi.
* Adres brokera MQTT 
* Port (1883) oraz identyfikator grupy roboczej.

### 3. Główny kod programu (`main.cpp`)
Napisano program realizujący następujące funkcje:
* **Generowanie unikalnego ID:** Wykorzystano funkcję `ESP.getEfuseMac()`, aby wygenerować unikalny `device_id` na podstawie sprzętowego adresu MAC płytki.
* **Połączenie z Wi-Fi:** Funkcja `connectWiFi()` łącząca ESP32 z lokalną siecią.
* **Połączenie z MQTT:** Funkcja `connectMQTT()` nawiązująca sesję z brokerem.
* **Publikowanie danych (JSON):** Użyto biblioteki `ArduinoJson` (klasa `JsonDocument`) do zbudowania ramki danych zawierającej m.in. ID urządzenia, typ czujnika ("temperature"), wartość (24.5 °C) i znacznik czasu (`millis()`). Następnie zmodyfikowane kod, aby zczytywał realną wartość temperatury rdzenia mikrokontrolera.
* Dane są wysyłane co 5 sekund na dedykowany topic: `lab/<grupa>/<device_id>/temperature`.


### 4. Weryfikacja działania (MQTT Explorer)
Działanie kodu zweryfikowano za pomocą programu **MQTT Explorer**. Po podłączeniu się do odpowiedniego brokera i zasubskrybowaniu tematu grupy, potwierdzono poprawne, cykliczne odbieranie wiadomości JSON ze zdefiniowaną temperaturą.

### 5. Kontrola wersji (Git)
Kod został zsynchronizowany ze zdalnym repozytorium na GitHubie. 