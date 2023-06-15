# Min Lisptolk

## Syfte
Jag skriver ett IoT-projekt i C, och behöver ett bekvämt sätt att skicka instruktioner till enhet i fält. 
Att skicka en sträng lispkod som utvärderas på enhet gör processen trivial.
Därmed skriver jag en förenklad lisptolk i C.

## Specifikation

### Operationer som stöds
vanliga operationer så som '+' och '-' stödjs, men än så länge endast för positiva heltal.

### Satser som stöds
Än så länge endast if-satser

### Praktisk användning

### Rekursivt Nedstigande Träd
Varje lisputtryck kan skriva ut med samtliga steg i tolkningen.
Exempelvis ger uttrycket "(= (* 3 3) (+ 4 (- 19 14)))" följande utskrift:
```
(
   EQ
    (
       MUL
         3
         3
    ) -> 9
    (
       ADD
         4
         (
            SUB
             19
             14
         ) -> 5
    ) -> 9
) -> 1
```
