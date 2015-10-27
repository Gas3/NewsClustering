#include <iostream>
#include <fstream>

#include "newscluster.h"
#include "conversion.h"

using namespace std;

NewsCluster::NewsCluster(words_topic_t terms, documents_topic_t idnews, int validity)
    : first_field_(0)
    , terms_(terms)
    , idnews_(idnews)
    , validity_(validity)
    , use_shingles_(false)
{}

NewsCluster::NewsCluster(words_topic_t terms, documents_topic_t idnews, int validity, bool use_shingles)
    : first_field_(0)
    , terms_(terms)
    , idnews_(idnews)
    , validity_(validity)
    , use_shingles_(use_shingles)
{}

// ritorna una stringa formattata nel seguente modo:
// [intero non significativo]
// [\t]
// [sequenza termini importanti del cluster, divisi da spazi]
// [\t]
// [sequenza ID news ordinate per importanza nel cluster]
// [\t]
// [flag binario: 1 cluster valido, 0 cluster non valido]
string NewsCluster::str()
{
    return ( NumberToString<int>(first_field_) +'\t' + print_terms() + '\t' +
             print_ids() + '\t' + NumberToString<int>(validity_));
}

std::ostream& operator<<(std::ostream& os, const NewsCluster& nc)
{
  os << nc.first_field_ << "\t";
  nc.print_terms(os);
  os << "\t";
  nc.print_ids(os);
  os << "\t" << nc.validity_;
  return os;
}

int NewsCluster::size()
{
    return static_cast<int>(idnews_.size());
}

int NewsCluster::get_validity()
{
   return validity_;
}

void NewsCluster::split_shingles(std::ostream &stream, std::string word, char sep) const
{
    size_t pos, old_pos = 0;

    pos = word.find(sep, old_pos);
    while(pos != std::string::npos)
    {
        stream << word.substr(old_pos, pos - old_pos) << " ";
        old_pos = pos + 1;
        pos = word.find(sep, old_pos);
    }
    stream << word.substr(old_pos, std::string::npos);
}

// scrittura dei termini su stream
void NewsCluster::print_terms(std::ostream &stream) const
{
    int size = static_cast<int>(terms_.size());
    // itero sui topic
    for (words_topic_t::const_iterator topic_it = terms_.begin(); topic_it != terms_.end(); topic_it++)
    {
        if (use_shingles_)
        {
            stream << topic_it->second << " ";
            split_shingles(stream, topic_it->first, '_');
        }
        else
            stream << topic_it->second << " " << topic_it->first;
        // controllo per aggiungere la virgola a tutti, meno che l'ultimo
        if ( (topic_it - terms_.begin()) < (size - 1))
            stream << ",";
    }
}

// nota: ogni termine presente nella stringa ritornata è separato da un altro da uno spazio.
string NewsCluster::print_terms() const
{
    std::stringstream ss;
    print_terms(ss);
    return ss.str();
}

// scrittura degli id su stream
void NewsCluster::print_ids(std::ostream &stream) const
{
    int size = static_cast<int>(idnews_.size());
    // itero sui topic
    for (documents_topic_t::const_iterator topic_it = idnews_.begin(); topic_it != idnews_.end(); topic_it++)
    {
        stream << (topic_it->first + 1);
        // controllo per aggiungere lo spazio a tutti, meno che l'ultimo
        if ( (topic_it - idnews_.begin()) < (size - 1))
            stream << " ";
    }
}

// nota: ogni id presente nella stringa ritornata è separato da un altro da uno spazio.
string NewsCluster::print_ids() const
{
    std::stringstream ss;
    print_ids(ss);
    return ss.str();
}


