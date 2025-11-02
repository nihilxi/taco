# TACO Language Specification

## Paradygmaty Języka

### 1. Imperatywny
TACO jest językiem imperatywnym, w którym programy składają się z sekwencji instrukcji modyfikujących stan programu.

### 2. Statyczne Typowanie
- Każda zmienna musi być zadeklarowana z typem przed użyciem
- Typy są sprawdzane w czasie kompilacji
- Dostępne typy podstawowe: `int`, `float`, `double`, `bool`, `void`

### 3. Zarządzanie Pamięcią
- Alokacja automatyczna na stosie dla zmiennych lokalnych
- Brak garbage collection - deterministyczne zarządzanie pamięcią
- Przyszłe rozszerzenie: wskaźniki i ręczna alokacja pamięci

### 4. Strukturalny
- Bloki kodu `{}`
- Funkcje jako podstawowe jednostki modularyzacji
- Wsparcie dla kontroli przepływu (if, while, for)

## Cechy Języka

### Silnie Typowany
```taco
int x = 5;
float y = 3.14;
// x = y;  // Błąd kompilacji - niezgodność typów
```

### Funkcje Pierwszej Klasy
```taco
int add(int a, int b) {
    return a + b;
}

int compute(int x) {
    return add(x, 10) * 2;
}
```

### Kontrola Przepływu
```taco
// If-else
if (x > 0) {
    y = x * 2;
} else {
    y = 0;
}

// While
while (i < 10) {
    sum = sum + i;
    i = i + 1;
}

// For
for (int i = 0; i < n; i = i + 1) {
    total = total + arr[i];
}
```

### Operatory

**Arytmetyczne:**
- `+`, `-`, `*`, `/`, `%`

**Porównania:**
- `==`, `!=`, `<`, `<=`, `>`, `>=`

**Logiczne:**
- `&&` (AND), `||` (OR), `!` (NOT)

**Przypisanie:**
- `=`

## Przykładowy Program

```taco
// Funkcja obliczająca silnię
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

// Funkcja główna
int main() {
    int n = 5;
    int result = factorial(n);
    return result;
}
```

## Priorytety Operatorów (od najwyższego)

1. `()` - wywołanie funkcji, `[]` - indeksowanie
2. `!`, `-`, `+` - operatory unarne
3. `*`, `/`, `%` - mnożenie, dzielenie, modulo
4. `+`, `-` - dodawanie, odejmowanie
5. `<`, `<=`, `>`, `>=` - porównania
6. `==`, `!=` - równość
7. `&&` - logiczne AND
8. `||` - logiczne OR
9. `=` - przypisanie

## Ograniczenia Obecnej Wersji

- Brak tablic dynamicznych
- Brak struktur/klas
- Brak wskaźników
- Brak obsługi wyjątków
- Podstawowe typy danych

## Planowane Rozszerzenia

- Tablice wielowymiarowe
- Struktury danych
- Moduły/import
- Obsługa plików
- Standardowa biblioteka matematyczna
