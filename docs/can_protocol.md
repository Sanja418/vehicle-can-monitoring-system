# Format CAN poruka

Ovaj dokument definiše format podataka za komunikaciju između
motora (EngineNode) i instrument table (DashboardNode).

Trenutna implementacija koristi red poruka u memoriji.
Fizički CAN prenos i podešavanje magistrale još nisu implementirani.

## Pregled poruka

| Poruka             | ID    | Dužina  | Tip podatka | Jedinica          |
| ------------------ | ----- | ------- | ----------- | ----------------- |
| Obrtaji motora     | 0x100 | 2 bajta | uint16_t    | obrtaji/min (RPM) |
| Temperatura motora | 0x101 | 2 bajta | int16_t     | °C                |
| Brzina vozila      | 0x102 | 2 bajta | uint16_t    | km/h              |

ID-evi su namenjeni standardnim CAN porukama sa 11-bitnim identifikatorom.
Dužina označava broj bajtova podataka, a ne veličinu celog CAN okvira.

## Redosled bajtova

Obe poruke koriste little-endian raspored:

- data[0]: donjih 8 bitova vrednosti.
- data[1]: gornjih 8 bitova vrednosti.

Preostali bajtovi u CanFrame nisu deo ovih poruka.

## Obrtaji motora

ID: 0x100
Dužina: tačno 2 bajta.

Vrednost je ceo broj bez predznaka, bez dodatnog skaliranja.

Primer: 1500 RPM = 0x05DC.

- data[0] = 0xDC
- data[1] = 0x05

Mogući opseg zapisa je 0–65535 RPM.
Trenutno ne proveravamo fizički realan opseg obrtaja.

## Temperatura motora

ID: 0x101
Dužina: tačno 2 bajta.

Vrednost je ceo broj sa predznakom u celim stepenima Celzijusa.
Negativne vrednosti koriste 16-bitni zapis u drugom komplementu.

Primer: 90 °C = 0x005A.

- data[0] = 0x5A
- data[1] = 0x00

Primer: -10 °C ima zapis 0xFFF6.

- data[0] = 0xF6
- data[1] = 0xFF

Mogući opseg zapisa je -32768–32767 °C.
Trenutno ne proveravamo fizički realan opseg temperature.

## Pravila prijema

Instrument tabla prihvata poruku kada ID i dužina
odgovaraju jednom od definisanih formata.

Ispravna poruka ažurira samo odgovarajući podatak
i njegovo vreme prijema.

Neispravna ili nepodržana poruka ne menja sačuvane podatke
niti vreme njihovog poslednjeg ispravnog prijema.

## Dostupnost i svežina podataka

RPM, temperatura i brzina imaju nezavisno stanje:

- Nema podatka: ispravna poruka još nije primljena.
- Svež podatak: starost je manja od zadatog roka.
- Zastareo podatak: starost je jednaka roku ili veća.

U demonstraciji koristimo rok od 2 sekunde.
Poslednja vrednost ostaje sačuvana i nakon zastarevanja.

Nula je validna vrednost i ne označava nedostatak podataka.

## Podešavanja koja tek treba definisati

- Model mikrokontrolera i CAN periferije.
- Brzina CAN magistrale.
- Period slanja RPM i temperaturnih poruka.
- Prijemni filteri i obrada grešaka CAN kontrolera.

## Brzina vozila

Pošiljalac: VehicleNode.
ID: 0x102.
Dužina: tačno 2 bajta.

Vrednost predstavlja cele kilometre na sat, bez predznaka.
Koristi little-endian raspored.

Primer: 60 km/h = 0x003C.

- data[0] = 0x3C
- data[1] = 0x00

Mogući opseg zapisa je 0–65535 km/h.
Trenutno ne proveravamo fizički realan opseg brzine.

Instrument tabla zasebno pamti brzinu i vreme njenog prijema.
RPM i temperaturne poruke ne osvežavaju brzinu.
Nula je validan podatak i označava zaustavljeno vozilo.

## Upozorenje na pregrevanje

Demonstracioni prag je 110 °C.

Upozorenje se prikazuje kada je temperatura sveža
i njena vrednost je veća ili jednaka pragu.

Ako temperatura nedostaje ili je zastarela,
tabla prikazuje taj status umesto zaključka o pregrevanju.
Odsustvo upozorenja tada ne znači da je motor bezbedan.

Upozorenje se računa na instrument tabli;
nije zasebna CAN poruka.
