// soswin
//
#include <cstddef>
#include <stdint.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <bitset>
#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <ctime>
#include <set>
#include <memory>

size_t uiLevenshteinDistance(const std::string &s1, const std::string &s2)
{
    const size_t m(s1.size());
    const size_t n(s2.size());

    if( m==0 ) return n;
    if( n==0 ) return m;

    size_t *costs = new size_t[n + 1];

    for( size_t k=0; k<=n; k++ ) costs[k] = k;

    size_t i = 0;
    for ( std::string::const_iterator it1 = s1.begin(); it1 != s1.end(); ++it1, ++i )
    {
        costs[0] = i+1;
        size_t corner = i;

        size_t j = 0;
        for ( std::string::const_iterator it2 = s2.begin(); it2 != s2.end(); ++it2, ++j )
        {
            size_t upper = costs[j+1];
            if( *it1 == *it2 )
            {
                costs[j+1] = corner;
            }
            else
            {
                size_t t(upper<corner?upper:corner);
                costs[j+1] = (costs[j]<t?costs[j]:t)+1;
            }

            corner = upper;
        }
    }

    size_t result = costs[n];
    delete [] costs;

    return result;
}


const size_t D_A = 0;
const size_t D_SIZE = 26;

class WordPoint
{
    public:

        WordPoint(const std::string& s)
            : m_Word(s)
        {
            m_Dimensions.fill(0);
            std::transform(m_Word.begin(), m_Word.end(), m_Word.begin(), ::tolower);

            for(size_t i = 0; i < m_Word.size(); ++i)
            {
                m_Dimensions[ CalculateOffset(m_Word[i]) ] += 1;
            }
        }

        size_t CalculateOffset(char l) const
        {
            if (l >= 'a' && l <='z')
            {
                size_t offset = D_A + (l-'a');
                return offset;
            }

            return 0;
        }

        size_t CalculateDistance(const WordPoint& that) const
        {
            int d = 0;
            int r;

            const char * pThis = &(this->m_Dimensions[0]);
            const char * pThat = &(that.m_Dimensions[0]);
            for( size_t i = 0; i < D_SIZE; ++i )
            {
                r = (*pThis) - (*pThat);
                d+= (r*r);
                ++pThis;
                ++pThat;
            }

            return d;
        }

        bool IsCloseEnough(const WordPoint& that, size_t distance) const
        {
            int d = 0;
            int r;

            const char * pThis = &(this->m_Dimensions[0]);
            const char * pThat = &(that.m_Dimensions[0]);
            for( size_t i = 0; i < D_SIZE; ++i )
            {
                r = (*pThis) - (*pThat);
                d+= (r*r);
                if (d>=distance)
                {
                    return false;
                }
                ++pThis;
                ++pThat;
            }
            return true;
        }

        const std::string& GetWord() const
        {
            return m_Word;
        }

    protected:
        std::array<char,D_SIZE> m_Dimensions;
        std::string m_Word;
};

class WordList
{
    public:
        WordList(size_t len)
            : m_WordLen(len) {}

        void AddWord(const std::string& word)
        {
            m_WordList.push_back(WordPoint(word));
        }

        std::vector<WordPoint>::iterator begin()
        {
            return m_WordList.begin();
        }

        std::vector<WordPoint>::iterator end()
        {
            return m_WordList.end();
        }

        size_t size() const
        {
            return m_WordList.size();
        }

    protected:
        std::vector<WordPoint> m_WordList;
        size_t m_WordLen;
};

class Dictionary
{
    public:
        Dictionary()
        {
            m_WordLists.resize(21);
        }

        ~Dictionary()
        {
        }

        void AddWord(const std::string& word)
        {
            m_WordList.push_back(std::shared_ptr<WordPoint>(new WordPoint(word)));

            GetWordList(word.size())->AddWord(word);
        }

        std::shared_ptr<WordList> GetWordList(size_t len)
        {
            len = std::min(len, m_WordLists.size()-1);
            if (nullptr == m_WordLists[len])
            {
                m_WordLists[len].reset(new WordList(len));
            }

            return m_WordLists[len];
        }

        void FindClosestMatchePreFilter(const std::string& word, std::set<std::string>& results)
        {
            WordPoint a(word);

            size_t start = std::max(1,int(word.size())-2);
            size_t end = word.size()+2;
            std::cout << "Len is " << word.size() << " start " << start << " end " << end << std::endl;
            for ( ; start <= end ; ++start )
            {
                std::shared_ptr<WordList> wl =  GetWordList(start);
                std::vector<WordPoint>::iterator wpIt = wl->begin();
                for( ; wpIt != wl->end(); ++wpIt)
                {
                    // calculate the distance between the 2 words
                    if((*wpIt).IsCloseEnough(a, 8))
                    {
                        size_t ld = uiLevenshteinDistance(word,(*wpIt).GetWord());
                        if (ld <= 2)
                        {
                            results.insert((*wpIt).GetWord());
                        }
                    }
                }
            }
        }

        void FindClosestMatches(const std::string& word, std::set<std::string>& results)
        {
            for(size_t i = 0; i < m_WordList.size(); ++i)
            {
                if (abs((int)word.size() - (int)m_WordList[i]->GetWord().size()) <= 2)
                {
                    size_t dist = uiLevenshteinDistance(word,m_WordList[i]->GetWord());
                    if(dist<=2)
                    {
                        results.insert(m_WordList[i]->GetWord());
                    }
                }
            }
        }

    protected:
        std::vector<std::shared_ptr<WordPoint> > m_WordList;
        std::vector<std::shared_ptr<WordList> > m_WordLists;
};

void dumpResults(std::set<std::string>& wdWords, std::set<std::string>& levWords)
{
    std::set<std::string>::iterator it,it2;
    bool cont = true;
    it = wdWords.begin();
    it2 = levWords.begin();

    std::cout << "WD words\tLev words" << std::endl;

    while(cont)
    {
        if (it != wdWords.end())
        {
            std::cout << *it;
            ++it;
        }
        std::cout << "\t\t";
        if (it2 != levWords.end())
        {
            std::cout << *it2;
            ++it2;
        }
        std::cout << std::endl;
        if (it == wdWords.end() && it2 == levWords.end())
        {
            cont = false;
        }
    }
}

int main(int argc, char* argv[])
{
    Dictionary d;

    std::ifstream ifs;

    ifs.open("./words.txt", std::ifstream::in);

    std::string line;
    while(std::getline(ifs,line))
    {
        std::transform(line.begin(), line.end(), line.begin(), ::tolower);
        d.AddWord(line);
    }

    std::string test;
    std::cout << "Enter word:" << std::endl;
    std::cin >> test;

    std::set<std::string> preFilterResults;
    std::set<std::string> levResults;

    double preFilterDuration;
    double levDuration;

    {            
        std::clock_t start = std::clock();
        d.FindClosestMatchePreFilter(test, preFilterResults);
        preFilterDuration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    }

    {    
        std::clock_t start = std::clock();
        d.FindClosestMatches(test, levResults);
        levDuration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    }
    dumpResults(preFilterResults, levResults);
    std::cout << "Times: Pre-filter " << preFilterDuration << " Lev " << levDuration << std::endl;

    return 0;
}

