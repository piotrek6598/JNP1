#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <sstream>
#include <tuple>
#include <vector>

using namespace std;

using tram_stop_name = string;
using hour = string; // w formacie gg:mm
using tram_line_number = string;
using tram_stops = map<tram_stop_name, hour>;
using ticket_price = int64_t;
using ticket_name = string;
using valid_ticket_time = int32_t;
using ticket = tuple<ticket_name, valid_ticket_time, ticket_price>;

/// Maksymalny czas waĹźnoĹci biletu w minutach (dĹugoĹÄ dnia + 1).
#define MAX_VALIDITY_TIME 927
/// Maksymalna dĹugoĹÄ (iloĹÄ cyfr) czasu waĹźnoĹÄi biletu.
/// Bilety waĹźne dĹuĹźej sÄ traktowane jak waĹźne MAX_VALIDITY_TIME.
#define MAX_VALIDITY_TIME_NUMBER_LENGTH 3
/// Maksymalna liczba biletĂłw do zaoferowanie dla pojedynczego zapytania.
#define MAX_USING_TICKETS 3
/// Kod ASCII dla liczby 0.
#define ASCII_CODE_0 48
/// Maksymalna obsĹugiwana dĹugoĹÄ (iloĹÄ cyfr) ceny biletu.
#define MAX_TICKET_TIME_PRICE_LENGTH 18

/// Przechowuje nazwy dodanych biletĂłw.
set<ticket_name> ticket_names;
/// Mapa dla danego numeru, mapa nazwa przystanku/czas.
map<tram_line_number, tram_stops> tram_lines;
/// Mapa dla danego czasu waĹźnoĹci, najtaĹszy bilet.
map<valid_ticket_time, ticket> ticket_validity_times;
/// Licznik wszytkich zaoferowanych biletĂłw.
int32_t sold_ticket_count = 0;

/** @brief Oblicza rĂłĹźnicÄ czasu miÄdzy godzinami.
 * Funkcja zwraca wynik w minutach. WartoĹÄ bezwzglÄdna wyniku jest
 * rĂłĹźnicÄ czasu miÄdzy danymi godzinami. WartoĹc ujemna oznacza Ĺźe pierwsza
 * godzina jest pĂłĹşniejsza.
 * @param[in] hour1   - pierwsza godzina
 * @param[in] hour2   - druga godzina
 * @return RĂłĹźnicÄ minut miÄdzy danymi godzinami. Ujemna liczba oznacza, Ĺźe
 * pierwsza godzina jest pĂłĹşniejsza.
 */
int32_t timeDiff(hour hour1, hour hour2) {
    int32_t diff = 0;
    diff += (hour2[0] - ASCII_CODE_0) * 10 + hour2[1] - ASCII_CODE_0;
    diff *= 60;
    diff += (hour2[3] - ASCII_CODE_0) * 10 + hour2[4] - ASCII_CODE_0;

    diff -= (hour1[0] - ASCII_CODE_0) * 600 + (hour1[1] - ASCII_CODE_0) * 60;
    diff -= (hour1[3] - ASCII_CODE_0) * 10 + hour1[4] - ASCII_CODE_0;

    return diff;
}

/** @brief Dodaje nowy bilet.
 * Funkcja dodaje nowy bilet do rozkĹadu. Funkcja zakĹada Ĺźe czas waĹźnoĹci
 * biletu oraz cena sÄ liczbÄ caĹkowitÄ.
 * Bilet waĹźny dĹuĹźej niĹź MAX_VALIDITY_TIME minut, jest traktowany
 * jak waĹźny MAX_VALIDITY_TIME minut.
 * @param[in] name         - nazwa nowego biletu
 * @param[in] valid_time   - czas waĹźnoĹci nowego biletu
 * @param[in] price        - cena nowego biletu wyraĹźowa w groszach
 * @return WartoĹÄ @p true, jeĹli bilet zostaĹ dodany. WartoĹÄ @p false, jeĹli
 * bilet o danej nazwie jest juĹź dodany lub bilet ma czas waĹźnoĹci 0.
 */
bool addTicket(ticket_name name, valid_ticket_time valid_time,
               ticket_price price) {

    ticket new_ticket;

    // JeĹli bilet waĹźny wiÄcej niĹź MAX_VALIDITY_TIME minut, to przyjmujemy,
    // Ĺźe waĹźny MAX_VALIDITY_TIME minut.
    if (valid_time > MAX_VALIDITY_TIME)
	valid_time = MAX_VALIDITY_TIME;

    // Sprawdzenie czy bilet o danej nazwie juĹź istnieje.
    if (ticket_names.count(name) > 0)
	return false;

    // JeĹli istnieje bilet niedroĹźszy waĹźny tyle samo,
    // trzbea tylko zapamiÄtaÄ jego nazwÄ.
    if (ticket_validity_times.count(valid_time) > 0 &&
        get<2>(ticket_validity_times[valid_time]) <= price) {
	ticket_names.insert(name);
	return true;
    }

    new_ticket = make_tuple(name, valid_time, price);

    // Zapisywanie nazwy oraz czasu waĹźnoĹci biletu.
    ticket_names.insert(name);
    ticket_validity_times[valid_time] = new_ticket;
    return true;
}

/** @brief Dokonuje wyboru biletĂłw.
 * Funkcja sprawdza czy podana trasa jest poprawna. Trasa jest poprawna, gdy:
 * wszystkie kursy zostaĹy dodane wczeĹniej do rozkĹadu, wszystkie przystanki
 * naleĹźÄ do odpowiednich kursĂłw, czas przyjazdu na dany przystanek jest
 * niepĂłĹşniejszy niĹź czas odjazdu z niego, godziny przyjazdu i odjazdu
 * na kolejne przystanki tworzÄ ciÄg ĹciĹle rosnÄcy.
 * JeĹli podana trasa jest niepoprawna to funkcja zwraca false, jeĹli
 * trasa jest poprawna to funkcja zwraca true oraz wypisuje na standardowe
 * wyjĹcie wybrane bilety albo informacje o koniecznoĹci czekania lub braku
 * moĹźliwoĹci wyboru biletĂłw.
 * @param[in] tram_line_numbers   - vector numerĂłw kursĂłw tworzÄcych trasÄ
 * @param[in] tram_stop_names     - vector nazw przystankĂłw tworzÄcych trasÄ
 * @return WartoĹÄ @p true, jeĹli trasa jest poprawna, wartoĹÄ @p false,
 * jeĹli trasa jest nie poprawna.
 */
bool chooseTickets(vector<tram_line_number> &tram_line_numbers,
                   vector<tram_stop_name> &tram_stop_names) {
    hour arrival_time;
    hour departure_time;
    tram_stop_name first_waiting_tram_stop = "";
    int32_t waiting_time;
    int32_t need_time;
    ticket_price min_ticket_price;
    vector<ticket_name> choosen_tickets;

    // Sprawdzenie czy wszystkie kursy istnieje.
    for (unsigned i = 0; i < tram_line_numbers.size(); i++) {
	if (tram_lines.count(tram_line_numbers[i]) == 0)
	    return false;
    }

    // Sprawdzenie czy linie przejeĹźdzajÄ przez dane przystanki w dobrej
    // kolejnosci oraz czy godziny przyjazdu/odjazdu tworzÄ ciÄg ĹciĹle rosnÄcy.
    for (unsigned i = 0; i < tram_line_numbers.size(); i++) {
	if (tram_lines[tram_line_numbers[i]].count(tram_stop_names[i]) == 0)
	    return false;
	if (tram_lines[tram_line_numbers[i]].count(tram_stop_names[i + 1]) == 0)
	    return false;
	if (timeDiff(tram_lines[tram_line_numbers[i]][tram_stop_names[i]],
	             tram_lines[tram_line_numbers[i]][tram_stop_names[i + 1]]) <= 0)
	    return false;
    }

    departure_time = tram_lines[tram_line_numbers[0]][tram_stop_names[0]];
    arrival_time = tram_lines[tram_line_numbers.back()][tram_stop_names.back()];

    // Sprawdzenie czy trzeba czekac i czy przyjazd wczesniej niz odjazd.
    for (unsigned i = 1; i + 1 < tram_stop_names.size(); i++) {
	waiting_time =
	    timeDiff(tram_lines[tram_line_numbers[i - 1]][tram_stop_names[i]],
	             tram_lines[tram_line_numbers[i]][tram_stop_names[i]]);
	if (waiting_time > 0 && first_waiting_tram_stop.empty())
	    first_waiting_tram_stop = tram_stop_names[i];
	if (waiting_time < 0)
	    return false;
    }

    // JeĹli trzeba czekac.
    if (!first_waiting_tram_stop.empty()) {
	cout << ":-( " << first_waiting_tram_stop << endl;
	return true;
    }

    need_time = timeDiff(departure_time, arrival_time) + 1;

    min_ticket_price = -1;

    // Wybieranie najtaĹszej opcji.
    for (const pair<valid_ticket_time, ticket> &t1 : ticket_validity_times) {
	for (const pair<valid_ticket_time, ticket> &t2 :
	     ticket_validity_times) {
		for (const pair<valid_ticket_time, ticket> &t3 :
	         ticket_validity_times) {
		if (t1.first + t2.first + t3.first >= need_time) {
			if (get<2>(t1.second) + get<2>(t2.second) +
		            get<2>(t3.second) < min_ticket_price ||
		            min_ticket_price == -1) {
			min_ticket_price = get<2>(t1.second) +
			                   get<2>(t2.second) +
			                   get<2>(t3.second);
			choosen_tickets.clear();

			// Wybranie mniej niĹź 3 biletĂłw, symulujemy poprzez
			// wybĂłr biletĂłw waĹźnych 0 minut.
			if (get<1>(t1.second) != 0)
			    choosen_tickets.push_back(get<0>(t1.second));
			if (get<1>(t2.second) != 0)
			    choosen_tickets.push_back(get<0>(t2.second));
			if (get<1>(t3.second) != 0)
			    choosen_tickets.push_back(get<0>(t3.second));
			}
		}
		}
	}
    }

    // Nie da siÄ kupiÄ biletĂłw.
    if (min_ticket_price == -1) {
	cout << ":-|" << endl;
	return true;
    }

    // Wypisywanie biletĂłw.
    cout << "! ";
    for (unsigned i = 0; i + 1 < choosen_tickets.size(); i++)
	cout << choosen_tickets[i] << "; ";
    cout << choosen_tickets.back() << endl;
    sold_ticket_count += choosen_tickets.size();
    return true;
}

/** @brief Zmienia format godziny na "gg:mm".
 * Zmienia format godziny ze "zwykĹego" formaty (np. 7:15, 19:01) na "gg:mm"
 * (np. 07:15, 19:01).
 * @param[in,out] h         - godzina do zmiany w "zwykĹym" formacie.
 */
void changeHourFormat(hour &h) {
    if (h.size() < 5) {
	h.insert(0, "0");
    }
}

/** @brief Usuwa wiodÄce zera z liczby nieujemnej.
 * @param[in,out] s         - poprawna liczba nieujemna.
 */
void eraseLeadingZeros(string &s) {
    if (s.size() > 1 && s[0] == '0') {
	reverse(s.begin(), s.end());
	while (s.size() > 1 && s.back() == '0') {
		s.pop_back();
	}
	reverse(s.begin(), s.end());
    }
}

/** @brief Dodaje nowy kurs tramwajowy do rozkĹadu.
 * Sprawdza czy kurs jest poprawny i jeĹli tak, to dodajÄ go do mapy kursĂłw
 * (@ref tram_lines). Kurs jest poprawny jeĹli Ĺźaden z przystankĂłw siÄ nie
 * powtarza, linia o danym numerze nie zostaĹa wczeĹniej dodana, a godziny
 * przyjazdĂłw na przystanki sÄ w kolejnoĹci ĹciĹle rosnÄcej oraz zawierajÄ
 * siÄ w przedziale pracy tramwajĂłw.
 * @param tram_line[in]          - poprawne skĹadniowo polecenie dodania
 * kursu.
 * @return WartoĹÄ @p true wtedy i tylko wtedy, gdy kurs jest poprawny.
 */
bool addTramLine(const string &tram_line) {
    istringstream iss(tram_line);

    tram_line_number tram_line_num;
    iss >> tram_line_num;
    eraseLeadingZeros(tram_line_num);

    // Linia o tym numerze juĹź zostaĹa wczeĹniej dodana.
    if (tram_lines.find(tram_line_num) != tram_lines.end()) {
	return false;
    }

    // Dodajemy do struktury przechowujÄcej linie tramwajowe liniÄ o numerze
    // tram_lin (na raziÄ bez przystankĂłw). Przekazujemy przez referencjÄ
    // zbiĂłr przystankĂłw tej linii by mĂłc poniĹźej dodaÄ odpowiednie
    // przystanki.
    tram_stops &tram_line_stops = tram_lines[tram_line_num];

    // Godzina rozpoczÄcia i zakoĹczenia pracy tramwajĂłw.
    const static hour day_begin = "05:55";
    const static hour day_end = "21:21";

    // Godzina przybycia tramwaju na poprzedni przystanek. Na poczÄtku
    // powinno byÄ "null", ale symulujemy to godzinÄ "00:00".
    hour previous_hour = "00:00";

    tram_stop_name stop_name;
    hour stop_hour;

    // Dodajemy przystanki tramwajowe do danej linii, sprawdzajÄc czy
    // przystanki i godziny sÄ poprawne.
    while (iss >> stop_hour) {
	iss >> stop_name;

	// Zmieniamy format godziny na taki, w ktĂłrym chcemy go
	// przechowywaÄ.
	changeHourFormat(stop_hour);

	// JeĹli przystanek siÄ powtĂłrzyĹ, godziny nie sÄ rosnÄco lub dana
	// godzina jest poza godzinami pracy tramwajĂłw, to linia jest
	// niepoprawna.
	if (tram_line_stops.find(stop_name) != tram_line_stops.end() ||
	    previous_hour >= stop_hour || stop_hour < day_begin ||
	    stop_hour > day_end) {
		tram_lines.erase(tram_line_num);
	    return false;
	}

	tram_line_stops[stop_name] = stop_hour;
	previous_hour = stop_hour;
    }

    return true;
}

/** @brief Dokonuje wyboru biletĂłw.
 * Funkcja wywoĹuje @ref chooseTickets interpretujÄc poprawne skĹadniowo
 * polecenie @p tram_line = "? przystanek_1 numer_kursu_1 przystanek_2
 * numer_kursu_2 ... przystanek_n numer_kursu_n przystanek_n+1".
 * @param tram_line[in]          - poprawne skĹadniowo polecenie wybrania
 * biletĂłw.
 */
bool chooseTicketsHelper(const string &tram_line) {
    istringstream iss(tram_line);

    // Pozbywamy siÄ znaku '?' z poczÄtku.
    iss.ignore(1, ' ');

    tram_stop_name name;
    tram_line_number number;

    vector<tram_stop_name> tram_stop_names;
    vector<tram_line_number> tram_line_numbers;
    iss >> name;
    tram_stop_names.push_back(name);

    while (iss >> number) {
	iss >> name;

	eraseLeadingZeros(number);

	tram_line_numbers.push_back(number);
	tram_stop_names.push_back(name);
    }

    return chooseTickets(tram_line_numbers, tram_stop_names);
}

/** @brief Przelicza cenÄ zapisanÄ w sĹowie w zĹotĂłwka na cenÄ w groszach.
 * @param s[in]             - cena w groszach (np. "123.45").
 * @return Cena biletu w groszach lub @p -1 jeĹli cena jest napisem dĹuĹźszym
 * niĹź obsĹugiwany (@ref MAX_TICKET_TIME_PRICE_LENGTH).
 */
ticket_price convertSringToTicketPrice(string s) {
    s.erase(s.end() - 3);
    eraseLeadingZeros(s);

    if (s.size() > MAX_TICKET_TIME_PRICE_LENGTH) {
	return -1;
    }

    return stoll(s);
}

/** @brief Przelicza czas zapisany w sĹowie na czas typu @ref valid_ticket_time.
 * @param s                 - czas waĹźnoĹci biletu.
 * @return JeĹli dany czas jest wiÄkszy niĹź @ref MAX_VALIDITY_TIME, to zwraca
 * MAX_VALIDITY_TIME. W przeciwnym wypadku czas zmieniony na typ
 * @ref valid_ticket_time.
 */
valid_ticket_time convertSringToValidTicketTime(string s) {
    eraseLeadingZeros(s);

    // JeĹli dana liczba zawiera ostro wiÄcej cyfr niĹź MAX_VALIDITY_TIME, to
    // ta liczba wiÄksza niĹź MAX_VALIDITY_TIME.
    if (s.size() > MAX_VALIDITY_TIME_NUMBER_LENGTH) {
	return MAX_VALIDITY_TIME;
    }

    return stoi(s);
}

/** @brief Dodaje nowy bilet.
 * Funkcja wywoĹuje @ref addTicket interpretujÄc poprawne skĹadniowo
 * polecenie
 * @p tram_line = "nazwa_biletu cena czas_waĹźnoĹci".
 * @param tram_line[in]          - poprawne skĹadniowo polecenie dodania
 * biletu.
 */
bool addTicketHelper(const string &tram_line) {
    const static regex ticket_name_regex("(([a-zA-Z]| )*)");

    // Odzyskujemy nazwÄ biletu. UĹźywamy do tego regexu, bo nazwa ta moĹźe
    // zawieraÄ spacje.
    ticket_name name;
    smatch m;
    regex_search(tram_line, m, ticket_name_regex);
    name = m[0];

    // Usuwamy nadwymiarowÄ spacjÄ (korzystajÄc z wyĹźej zdefiniowanego
    // regexu do zmiennej name wkrada siÄ spacjÄ spomiÄdzy "nazwa_biletu",
    // "cena").
    name.pop_back();

    // Odzyskujemy cenÄ i czas waĹźnoĹci.
    istringstream iss(m.suffix().str());
    string valid_time_in_string;
    string ticket_price_string;
    iss >> ticket_price_string >> valid_time_in_string;

    ticket_price t_price = convertSringToTicketPrice(ticket_price_string);

    // JeĹli cena biletu jest wiÄksza niĹź obsĹugiwana.
    if (t_price == -1) {
	return false;
    }

    return addTicket(name, convertSringToValidTicketTime(valid_time_in_string),
                     t_price);
}

/** @brief Czyta pojedyncze polecenie (wiersz) z wejĹcia.
 * Interpretuje polecenia z wejĹcia sprawdzajÄc ich poprawnoĹÄ. JeĹli
 * polecenia sÄ poprawne to je wykonuje. ObsĹugiwane polecenia:
 *  - Dodanie nowego kursu do rozkĹadu: "numer_kursu czas_1 przystanek_1
 *    czas_2 przystanek_2 ... czas_n przystanek_n";
 *  - Dodanie biletu do rozkĹadu: "nazwa_biletu cena czas_waĹźnoĹci"
 *  - Pytanie o bilety: "? przystanek_1 numer_kursu_1 przystanek_2
 * numer_kursu_2
 *    ... przystanek_n numer_kursu_n przystanek_n+1"
 * JeĹli polecenie jest niepoprawna wypisuje odpowiedni komunikat na
 * standardowe wyjĹcie diagnostyczne. Puste wiersze sÄ ignorowane.
 * @return WartoĹÄ @p false wtedy i tylko wtedy, gdy nie ma juĹź wiÄcej
 * wierszy na wejĹciu.
 */
bool readLine() {
    // Licznik oznaczajÄcy numer wiersze z wejĹcia.
    static int input_line_count = 0;

    // Tworzymy odpowiednie regexy, dla obsĹugiwanych poleceĹ.
    const static string tram_line_number_pattern = "([0-9]+)";
    const static string hour_pattern = "(([0-9]|(1[0-9])|(2[0-3])):[0-5][0-9])";
    const static string tram_stop_name_pattern = "(([a-zA-Z]|_|\\^)+)";
    const static string ticket_name_pattern = "(([a-zA-Z]| )*)";
    const static string ticket_price_pattern = "([0-9]+\\.[0-9]{2})";
    const static string valid_ticket_time_pattern = "([1-9][0-9]*)";

    const static regex add_tram_line_regex(tram_line_number_pattern + "( " +
        hour_pattern + " " + tram_stop_name_pattern + ")+");
    const static regex add_ticket_regex(ticket_name_pattern + " " +
        ticket_price_pattern + " " + valid_ticket_time_pattern);
    const static regex choose_tickets_regex(
        "\\? " + tram_stop_name_pattern + "( " + tram_line_number_pattern +
        " " + tram_stop_name_pattern + ")+");

    string input_line;
    if (!getline(cin, input_line)) {
	return false;
    }
    input_line_count++;

    // Ignorujemy puste wiersze.
    if (input_line.empty()) {
	return true;
    }

    // Rozpoznajemy polecenie i prĂłbujemy je wykonaÄ.
    bool success = false;
    if (regex_match(input_line, add_tram_line_regex)) {
	if (addTramLine(input_line)) {
		success = true;
	}
    } else if (regex_match(input_line, add_ticket_regex)) {
	if (addTicketHelper(input_line)) {
		success = true;
	}
    } else if (regex_match(input_line, choose_tickets_regex)) {
	if (chooseTicketsHelper(input_line)) {
		success = true;
	}
    }

    if (!success) {
	cerr << "Error in line " << input_line_count << ": " << input_line
	     << "\n";
    }

    return true;
}

/** @brief initTicket_validity_times Ustawia poczÄtkowy stan mapy
 * ticket_validity_times.
 * PoczÄtkowo mapa zawiera "bilet" kosztujÄcy 0.00, waĹźny 0 minut.
 * "Bilet" ten jest uĹźywany do symulowania wyboru mniej niĹź 3 biletĂłw.
 */
void initTicket_validity_times() {
    ticket_validity_times[0] = make_tuple("", 0, 0);
}

int main() {
    initTicket_validity_times();
    while (readLine()) {
    }
    cout << sold_ticket_count << endl;

    return 0;
}
