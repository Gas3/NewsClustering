#include <math.h>
#include <sstream>
#include <set>

#include "dictionary.h"

using std::string;
using std::vector;
using std::pair;
using std::map;
using std::set;

// note: in fase di costruzione il secondo campo del dizionario viene usato per contare i documenti in cui
// ogni parola occorre e poi, al termine della costruzione, viene calcolato l'idf finale.
Dictionary::Dictionary(const std::vector<News> &dataset)
{
    /* result è una variabile d'appoggio, serve per testare l'esito dell'inserimento nel dizionario,
       result.second == false se si cerca di inserire un elemento già presente */
    pair<iterator, bool> result;

    /* utilizziamo un insieme (set) per collezionare i termini presenti in un documento,
       a partire dall'insieme aggiorno il dizionario*/
    set<string> document_terms;
    // variabile d'appoggio per il risultato dell'inserimento di un termine in document_terms
    pair<set<string>::iterator, bool> document_result;

    // scansione delle news per trovare tutte le parole
    for (vector<News>::const_iterator news = dataset.begin(); news != dataset.end(); news++)
    {
        if ( !(news->is_empty()) )
        {
            // NOTA: body contiene già in coda il titolo della news
            string body (news->body());
            size_t start_pos = 0;                  // indice del primo carattere del termine successiva
            size_t end_pos;                        // indice del primo spazio dopo il termine che inizia in start_pos
            size_t length;                         // lunghezza del termine

             // inizializzo end_pos alla fine del primo termine
            end_pos = body.find(' ', start_pos);

            while(end_pos != std::string::npos)
            {
                length = end_pos - start_pos;
                /* se la lunghezza del termine è maggiore di uno oppure il carattere (perché la stringa è lunga uno) non è lo spazio
                   inserisco il termine nel dizionario del documento, se il termine è già presente non viene inserito e
                   document_result contiene l'iteratore posizionato sull'elemento */
                if (length > 1 || body[start_pos] != ' ')
                    document_result = document_terms.insert(body.substr(start_pos, length));

                // NOTA: questo incremento impedisce che start_pos ed end_pos siano uguali (skippa lo spazio in body[end_pos])
                start_pos = end_pos + 1;
                // assegno ad end_pos la fine del prossimo termine
                end_pos = body.find(' ', start_pos);
            }
            /* se la parola non termina con uno spazio, l'ultima parola presente nel corpo viene gestita
               come caso particolare ma sicuramente non ci sono più spazi;
               se end_pos indicava l'ultimo carattere del corpo, start_pos è pari alla lunghezza perché
               start_pos = end_pos + 1 */
            if(start_pos != body.length()) {
                    document_result = document_terms.insert(body.substr(start_pos, std::string::npos));
            }

            // itero sui termini del documento e aggiorno il dizionario
            for(set<string>::iterator term = document_terms.begin(); term != document_terms.end(); term++)
            {
                // se è la prima volta che la parola occorre, viene inserita nel dizionario e il secondo campo prende valore 1
                result = insert(value_type(*term,1.0));
                // altrimenti viene incrementato il contatore
                if (!result.second)
                    result.first->second += 1.0;
            }

            // svuoto l'insieme dei termini di un documento, per la prossima iterazione
            document_terms.clear();
        }
    }

    // calcolo idf = log(N/N_term) = logN - logN_term
    mapped_type logN = log(dataset.size());
    for (iterator vi = begin(); vi != end(); vi++)
        (vi->second) = logN - log(vi->second);
}

// scrittura del dizionario su stringa
string Dictionary::print_dictionary() const
{
    std::stringstream ss;
    print_dictionary(ss);
    return ss.str();
}

// scrittura del dizionazio su stream
void Dictionary::print_dictionary(std::ostream &stream) const
{
    stream << size();

    for (const_iterator vi = begin(); vi != end(); vi++)
        stream << std::endl << vi->first << " " << vi->second;
}

// overload dell'operatore <<
std::ostream& operator<<(std::ostream& os, const Dictionary& dict)
{
  dict.print_dictionary(os);
  return os;
}
