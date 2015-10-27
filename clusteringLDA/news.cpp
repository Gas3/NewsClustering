#include "conversion.h"
#include "news.h"

using std::string;

// costruttore vuoto
News::News() {}

News::News(int epoch, std::string &id_source, std::string &newspaper, std::string &title,
           std::string &body, int id_category, std::string &link)
    :_epoch(epoch)
    , _id_source(id_source)
    , _newspaper(newspaper)
    , _title(title)
    , _body(body)
    , _id_category(id_category)
    , _link(link)
    , _old_body(body)
{
    if (_body == "")
        _is_empty = true;
    else
        _is_empty = false;
}

// getter campi news
int News::epoch() const { return _epoch; }

std::string News::id_source() const { return _id_source; }

std::string News::newspaper() const { return _newspaper; }

std::string News::title() const { return _title; }

std::string News::body() const { return _body; }

int News::id_category() const { return _id_category; }

std::string News::link() const { return _link; }

std::string News::old_body() const { return _old_body; }

// stato del corpo della news
bool News::is_empty() const { return _is_empty; }

// setter per corpo
void News::set_body(std::string newbody) {
    _body.swap(newbody);
    if (_body == "")
        _is_empty = true;
    else
        _is_empty = false;
}

std::string News::operator[] (int i) {
    switch(i)
    {
        case 0:
            return NumberToString<int>(_epoch);
        case 1:
            return id_source();
        case 2:
            return newspaper();
        case 3:
            return title();
        case 4:
            return body();
        case 5:
            return NumberToString<int>(_id_category);
        case 6:
            return link();
        default:
            return "";
    }
}
