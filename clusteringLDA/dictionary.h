#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <map>
#include <vector>
#include <string>
#include <ostream>

#include "news.h"

/* classe che rappresenta il dizionario di un dataset
   NOTA: eredita da map */

class Dictionary: public std::map<std::string, double>
{
public:
    // costruttore che crea il dizionario del dataset
    Dictionary(const std::vector<News> &dataset);

    // restituisce la stringa ottenuta da print_dictionary(std::ostream &stream)
    std::string print_dictionary() const;

    /* scrive sullo stream passato come parametro la dimensione del dizionario seguito da
       la lista di tutte le parole del dizionario ed il rispettivo idf:
       ogni riga contiene la parola e l'idf separati da uno spazio */
    void print_dictionary(std::ostream &stream) const;

    // overloading dell'operatore <<: esegue print_dictionary(std::ostream &stream) sullo ostream
    friend std::ostream& operator<<(std::ostream& os, const Dictionary& dict);
};

#endif // DICTIONARY_H
