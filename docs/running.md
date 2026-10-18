# Pokretanje projekta

## Potrebni alati

Postavka proverena na Windows-u:

- Git Bash
- CMake 3.20 ili noviji i Microsoft C++ Build Tools
- Arm GNU Toolchain 15.3.Rel2, cilj arm-none-eabi
- Renode 1.17.0
- Python 3.11.9

Komande za Git Bash pokreću se iz glavnog foldera projekta.

## Simulacija i C++ testovi na računaru

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build -C Debug --output-on-failure
```

Očekivano: 7 uspešnih testova.

Pokretanje simulacije:

```bash
./build/Debug/vehicle_can_monitoring.exe
```

## ARM kompajler

Dodati njegov bin folder u PATH. Na razvojnom računaru:

```bash
export PATH="/c/Users/Milos/sanja/tools/bin:$PATH"
```

Na drugom računaru prilagoditi putanju.

## Kompajliranje firmware-a

```bash
bash firmware/can_demo/build.sh
```

Nastaju:

- build/firmware/can_demo/sender.elf
- build/firmware/can_demo/receiver.elf

## Ručna demonstracija u Renode-u

U Renode Monitoru:

```text
include @C:/Users/Milos/sanja/vehicle-can-monitoring-system/firmware/can_demo/recovery_demo.resc
```

Prilagoditi apsolutnu putanju lokaciji projekta.

Demonstracija prikazuje:

1. Periodičan prijem RPM-a, temperature i brzine.
2. Zastarevanje podataka nakon zaustavljanja procesora pošiljaoca.
3. Ponovni prijem nakon nastavka rada pošiljaoca.

Na kraju je emulacija pauzirana.

## Priprema automatskih Renode testova

Jednom nakon preuzimanja projekta:

```bash
python -m venv build/renode-venv
build/renode-venv/Scripts/python.exe -m pip install -r "C:/Program Files/Renode/tests/requirements.txt"
```

Prilagoditi putanju ako je Renode instaliran u drugom folderu.

## Pokretanje automatskih Renode testova

```bash
bash tests/renode/run_tests.sh
```

Skripta kompajlira firmware i pokreće pet automatskih testova:

1. Prijem RPM-a, temperature i brzine.
2. Detekcija zastarelih podataka nakon prekida slanja i prijem nakon oporavka.
3. Provera da se upozorenje na pregrevanje pojavljuje pri 110 °C, a izostaje pri 109 °C i nakon povratka na 90 °C.
4. Nezavisno praćenje svežine RPM-a dok temperatura i brzina nastavljaju da stižu.
5. Odbacivanje RPM poruke pogrešne dužine i nastavak prijema nakon vraćanja ispravne dužine.

Python okruženje nije potrebno prethodno aktivirati.

Pre pokretanja testova dodati ARM kompajler u PATH, prema odeljku „ARM kompajler“.
Putanja Renode instalacije može se zadati ovako:

```bash
RENODE_DIR="D:/Tools/Renode" bash tests/renode/run_tests.sh
```

## Rezultati

Svako pokretanje testova pravi zaseban folder:

build/renode-results/GGGGMMDD-HHMMSS/

Naziv foldera sadrži datum i vreme pokretanja. U njemu se nalaze:

- report.html — pregled rezultata testova;
- log.html — detalji izvršavanja;
- robot_output.xml — rezultati u XML formatu.

Tačne putanje ispisuju se u terminalu nakon završetka testiranja.

Build izlazi i Python okruženje ne čuvaju se u Gitu.

## Ograničenja

Demonstracija koristi dva virtuelna STM32 čvora.
Pošiljalac šalje sva tri podatka svakih 500 ms.
Prijemnik prijavljuje zastarevanje nakon 2000 ms bez odgovarajuće poruke.

Provera prekida zaustavlja procesor pošiljaoca;
ne simulira fizički prekid žice.

Uspešna emulacija ne potvrđuje električne osobine CAN magistrale
niti sve greške i vremenske karakteristike fizičkog hardvera.
