#include <iostream>
#include <utility>
#include <vector>
#include <list>
#include <sstream>
#include <cassert>

class Page {

private:
    //attributes:
    const std::string content;
    const size_t key;
public:
    int counter = 0;

    Page(size_t k, std::string c)
            : content(std::move(c)), key(k) {}

    //methods
    void ShowPage() const { std::cout << content << std::endl; };

    void UpdateCounter() { counter++; };

    [[nodiscard]] size_t GetKey() const { return key; };

    void statistics() const { std::cout << "Page " << key << " was loaded " << counter << " times." << std::endl; };
};


bool cmp(const std::pair<size_t, Page> &a, const std::pair<size_t, Page> &b) {
    if (a.second.counter == b.second.counter) {
        return (a.first > b.first);
    } else
        return (a.second.counter >
                b.second.counter); //the pages visited the least times are now displayed at the bottom
}

struct Cache {
    size_t sz;
    std::list<std::pair<size_t, Page>> cache;
    size_t next_id = 1;

    explicit Cache(size_t sz) : sz(sz) {}

    [[nodiscard]] bool full() const { return (cache.size() == sz); }

    void AddPage(Page &P) {
        P.counter++;
        cache.emplace(cache.begin(), next_id++, P);
    };

    void SortLeastFrequent() {
        cache.sort(cmp);
    };

    //lru algorithm

    void update_cache(Page &p) {
        size_t current_key = p.GetKey();
        auto pos = std::find_if(cache.begin(), cache.end(),
                                [current_key](std::pair<size_t, Page> const &b) {
                                    return b.second.GetKey() == current_key;
                                });

        if (pos == cache.end()) {
            if (full()) {
                SortLeastFrequent(); //n log n sort
                cache.pop_back();
            }
            AddPage(p);
        } else {
            pos->second.counter++;
            pos->first = next_id++;
        }

    };
};

using Internet = std::vector<Page>;

Internet create_internet(std::size_t size) {
    Internet internet;
    for (std::size_t i = 0; i < size; ++i) {
        size_t address = i;
        std::ostringstream oss;
        oss << "I am the Page number " << address;
        std::string text = oss.str();

        internet.emplace_back(address, text);
    }

    return internet;
}

int main() {
    size_t n = 20;

    Internet mini_internet = create_internet(n);

    size_t num = 5;
    assert(num < n);
    Cache mycache(num);
    std::vector<size_t> address = {1, 1, 2, 1, 3, 4, 5, 6, 7, 8, 3};


    for (std::size_t i: address) {
        auto pos = std::find_if(mini_internet.begin(), mini_internet.end(),
                                [i](Page const &b) {
                                    return b.GetKey() == i;
                                });

        if (pos != mini_internet.end()) { mycache.update_cache(*pos); }
    }

    for (const auto &i: mycache.cache) {
        i.second.ShowPage();
        std::cout << "Page id in cache equals " << i.first << std::endl;
        i.second.statistics();
    }

    return 1;
}