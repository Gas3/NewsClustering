#ifndef NEWS_H
#define NEWS_H

#include <string>

// classe che rappresenta una news

class News
{
public:
    News();
    News(int epoch, std::string &id_source, std::string &newspaper, std::string &title,
         std::string &body, int id_category, std::string &link);

    // metodi che restituiscono il valore dei campi della news
    int epoch() const;
    std::string id_source() const;
    std::string newspaper() const;
    std::string title() const;
    std::string body() const;
    int id_category() const;
    std::string link() const;
    std::string old_body() const;

    // restituisce true se il cui corpo della news è vuoto
    bool is_empty() const;

    // modifica del corpo
    void set_body(std::string  newbody);

    // redefinizione dell'operatore per accedere ai campi della news
    std::string operator[] (int i) ;

private:
    // campi di una news
    int _epoch;
    std::string _id_source;
    std::string _newspaper;
    std::string _title;
    std::string _body;
    int _id_category;
    std::string _link;

    // corpo iniziale della news (prima delle modifiche)
    std::string _old_body;

    // indica se il corpo è vuoto
    bool _is_empty;
};

#endif // NEWS_H
