#pragma warning(disable : 4996)
#include <iostream>
#include <vector>
#include <cstring>

#define DEBUG false

class TTrieNode {
private:
    std::vector<TTrieNode*> children;
    unsigned long litera = 0;
    TTrieNode* suffix;
    unsigned long term = 0;
public:
    TTrieNode(unsigned long inp) {
        litera = inp;
        term = 0;
        suffix = nullptr;
    }

    ~TTrieNode() {
        for (int i = 0; i < children.size(); i++) {
            delete(children[i]);
        }
    }

    void setSuff(TTrieNode* suff) {
        suffix = suff;
    }

    unsigned long liter() {
        return litera;
    }

    std::vector<TTrieNode*> child() {
        return children;
    }

    bool hasChild(unsigned long a) {
        for (int i = 0; i < children.size(); ++i) {
            if (children[i]->litera == a) {
                return true;
            }
        }
        return false;
    }

    unsigned long isTerm() {
        return term;
    }

    void addChild(TTrieNode* adb) {
        children.push_back(adb);
    }

    void setterm(unsigned long count) {
        term = count;
    }

    TTrieNode* child(unsigned long a) {
        for (int i = 0; i < children.size(); ++i) {
            if (children[i]->litera == a) {
                return children[i];
            }
        }
        return nullptr;
    }

    TTrieNode* back() {
        return suffix;
    }
};

void printtrie(TTrieNode* root,int depth) {
    for (int i = 0; i < depth; ++i) {
        printf("\t");
    }
    if (root->isTerm()) {
        printf("!");
    }
    printf("|%lu|", root->liter());
    std::vector<TTrieNode*> next = root->child();
    printf("%d>\n", next.size());
    for (int i = 0; i < next.size(); ++i) {
        printtrie(next[i],depth+1);
    }
}

TTrieNode* correlate(TTrieNode* root, std::vector<unsigned long> word) {
    for (int i = 1; i < word.size(); ++i) {
        TTrieNode* cur = root;
        for (int j = i; j < word.size();++j) {
            if (cur->child(word[j]) == nullptr) {
                break;
            }
            else {
                cur = cur->child(word[j]);

                if (j + 1 == word.size()) {
                    return cur;
                }
            }
        }
    }
    return root;
}

void select(TTrieNode* root, TTrieNode* cur, std::vector<unsigned long> word) {
    if (cur != root) {
        word.push_back(cur->liter());
        
        if(DEBUG)printf("word found:\n");
        for (int i = 0; i < word.size(); ++i) {
            if(DEBUG)printf("  |%lu\n", word[i]);
        }
        TTrieNode* suff = correlate(root, word);
        if (DEBUG) {
            if (suff != root) {
                printf("suffix leads to: %lu\n", suff->liter());
            }
            else {
                printf("suffix leads to root\n");
            }
        }
        cur->setSuff(suff);
    }

    std::vector<TTrieNode*> next = cur->child();
    for (int i = 0; i < next.size(); ++i) {
        select(root, next[i], word);
    }
}

int reconst(TTrieNode* begin, TTrieNode* end, int depth) {
    
    if (begin == end) {
        return depth;
    }

    std::vector<TTrieNode*> next = begin->child();
    for (int i = 0; i < next.size(); ++i) {
        int tmp = reconst(next[i], end, depth + 1);
        if (tmp != -1) {
            return tmp;
        }
    }

    return -1;
}

class TTrie {
private:
    TTrieNode* root;
    int maxSize;
    unsigned long count = 0;
public:
    TTrie() {
        root = new TTrieNode(0);
        maxSize = 0;
        root->setSuff(root);
    }

    ~TTrie() {
        delete root;
    }

    unsigned long Count() {
        return count;
    }

    unsigned long add(std::vector<unsigned long> word) {
        if (word.size() > maxSize) {
            maxSize = word.size();
        }

        TTrieNode* cur = root;

        for (int i = 0; i < word.size(); ++i) {
            if (cur->hasChild(word[i])) {
                cur = cur->child(word[i]);
                if (DEBUG)printf("passed (%lu)\n", cur->liter());
            }
            else {
                TTrieNode* adb = new TTrieNode(word[i]);
                cur->addChild(adb);
                cur = cur->child(word[i]);
                if (DEBUG)printf("aded (%lu)\n", cur->liter());
            }
        }

        if (cur->isTerm()) {
            return cur->isTerm();
        }

        count++;
        cur->setterm(count);
        return count;
    }

    bool contains(char* word) const {
        TTrieNode* cur = root;

        for (int i = 0; i < strlen(word); ++i) {
            if (cur->hasChild(word[i])) {
                cur = cur->child(word[i]);
            }
            else {
                return false;
            }
        }

        return true;
    }

    void print() {
        printtrie(root, 0);
    }

    void suffixate() {
        std::vector<unsigned long> word;
        select(root, root, word);
    }

    TTrieNode* Root() {
        return root;
    }
};

class result {
public:
    unsigned long numberOfPattern; 
    unsigned long startPos;
    unsigned long endPos;

    result(unsigned long num, unsigned long beg, unsigned long end) {
        numberOfPattern = num;
        startPos = beg;
        endPos = end;
    }
};

std::vector<result> search(TTrie* trie,std::vector<unsigned long> text) {
    std::vector<result> ret;
    TTrieNode* cur = trie->Root();

    for (unsigned long i = 0; i < text.size(); ++i) {
        if (DEBUG)printf("<%lu>\n", i);
        if (cur->isTerm()) {
            if (DEBUG)printf("pattern found (%lu) from %lu to %lu\n", cur->isTerm(), i - reconst(trie->Root(), cur, 0) + 1, i);
            ret.push_back(result(cur->isTerm(), i - reconst(trie->Root(),cur,0) + 1, i));
        }

        TTrieNode* next = cur->child(text[i]);
        if (next == nullptr) {
            next = cur->back();
            
            if (cur != trie->Root()) {
                --i;
            }
        }
        cur = next;
        if (DEBUG)printf("%d | passing to %lu\n", i, cur->liter());
    }

    while(cur != trie->Root()) {
        if (cur->isTerm()) {
            if (DEBUG)printf("pattern found (%lu) from %lu  to %lu\n", cur->isTerm(), text.size() - reconst(trie->Root(), cur, 0) + 1, text.size());
            ret.push_back(result(cur->isTerm(), text.size() - reconst(trie->Root(), cur, 0) + 1, text.size()));
        }
        
        cur = cur->back();
        if (DEBUG)printf("passing to %lu\n", cur->liter());
    }

    return ret;
}

/*#define WAIT 0
#define PROCEDE 1


std::vector<result> collect(std::vector<result> preRes, unsigned long maxSize, std::vector<unsigned long> templ, unsigned long max) {
    unsigned long beg = 0;
    unsigned long end = 0;
    int mode = WAIT;
    unsigned long templPos = 0;
    unsigned long space = 0;
    unsigned long incCount = 0;
    unsigned long pastNum = 0;
    std::vector<result> finalRes;

    space = 0;
    if (templ[templPos] == -1) {
        while (templ[templPos] == -1 && templPos < templ.size()) {
            ++space;
            ++templPos;
        }
        if (DEBUG)printf("jumped for %lu\n", space);
    }

    for (unsigned long i = 0; i < preRes.size(); ++i) {

        if (DEBUG)printf("mode: %d|processing %d: %lu-%lu (max size is %lu)\n",mode, preRes[i].numberOfPattern, preRes[i].startPos, preRes[i].endPos,maxSize);
            
        if (mode == WAIT) {
            if ((preRes[i].numberOfPattern == templ[templPos]) && (preRes[i].startPos - 1 >= templPos)) {
                if (DEBUG)printf("start found\n");
                incCount++;

                mode = PROCEDE;
                beg = preRes[i].startPos;
                pastNum = 1;

                if (incCount < maxSize) {
                    templPos++;
                }
            }
        }
        else 
        if (mode == PROCEDE) {
            if (DEBUG)printf("%lu <=space|templPos=> %lu (%lu|%lu)\n", space,templPos, templ[templPos], preRes[i].numberOfPattern);
            if (DEBUG)printf("%lu|%lu\n", incCount, maxSize);
            if (preRes[i].numberOfPattern == templ[templPos] && preRes[i].startPos == preRes[i-1].endPos + space + 1) {
                incCount++;

                if (incCount < maxSize) {
                    templPos++;
                }
            }
            else {
                mode = WAIT;
                incCount = 0;
                templPos = 0;
                space = 0;

                continue;
            }
        }

        space = 0;
        if (templ[templPos] == -1) {
            while (templ[templPos] == -1 && templPos < templ.size()) {
                ++space;
                ++templPos;
            }
            if (DEBUG)printf("jumped for %lu\n", space);
        }

        if (incCount == maxSize) {
            if (DEBUG)printf("inclusion possible\n");
            if (DEBUG)printf("%lu | %lu\n", templPos + 1,templ.size());
            if (templPos + 1 != templ.size()) {
                if (DEBUG)printf("final J chek\n");
                if (DEBUG)printf("%lu | %lu\n", preRes[i].endPos, max);
                if (preRes[i].endPos + (templ.size() - (templPos + 1)) > max) {
                    mode = WAIT;
                    incCount = 0;
                    templPos = 0;
                    space = 0;

                    if (DEBUG)printf("chek failed\n");
                    continue;
                }

                if (DEBUG)printf("chek passed\n");
            }

            finalRes.push_back(result(0, beg, preRes[i].endPos));
            if (DEBUG)printf("inclusion confirmed\n");
            mode = WAIT;
            incCount = 0;
            templPos = 0;
            space = 0;

        }
    }
    return finalRes;
}
*/
#define WAIT 0
#define PROCEDE 1


std::vector<result> collect(std::vector<result> preRes, unsigned long maxSize, std::vector<unsigned long> templ, unsigned long max) {
    unsigned long beg = 0;
    unsigned long end = 0;
    int mode = WAIT;
    std::vector<result> finalRes;
    unsigned long templPos = 0;

    unsigned long start = 0;
    if (templ[templPos] == -1) {
        while (templ[templPos] == -1 && templPos < templ.size()) {
            ++start;
            ++templPos;
        }
        if (DEBUG)printf("starting in %lu\n", start);
    }

    for (unsigned long i = 0; i < preRes.size(); ++i) {
        templPos = 0;
        if (DEBUG)printf("mode: %d|processing %d: %lu-%lu (max size is %lu)\n", mode, preRes[i].numberOfPattern, preRes[i].startPos, preRes[i].endPos, maxSize);


        if ((preRes[i].numberOfPattern == templ[start]) && (preRes[i].startPos - 1 >= start)) {
            if (DEBUG)printf("start found. Proceding\n");
            unsigned long incCount = 1;
            unsigned long space = 0;
            unsigned long pastNum = 0;
            templPos = start + 1;
            beg = i - start;

            if (incCount == maxSize) {//проверяем вхождение, если удалось достроить
                if (DEBUG)printf("inclusion possible\n");
                if (DEBUG)printf("%lu | %lu\n", templPos, templ.size());
                if (templPos < templ.size()) {
                    if (DEBUG)printf("final J chek\n");
                    if (DEBUG)printf("%lu | %lu\n", preRes[i].endPos, max);
                    if (preRes[i].endPos + (templ.size() - (templPos)) > max) {

                        if (DEBUG)printf("chek failed\n");
                        break;
                    }

                    if (DEBUG)printf("chek passed\n");
                }

                finalRes.push_back(result(0, beg + 1, preRes[i].endPos + (templ.size() - (templPos))));
                if (DEBUG)printf("inclusion confirmed\n");
                continue;
            }

            for (unsigned long j = i + 1; j < preRes.size(); ++j) {//в этом цикле производим продление найденного старта
                if (DEBUG)printf("cheking: processing %d: %lu-%lu (max size is %lu)\n", preRes[j].numberOfPattern, preRes[j].startPos, preRes[j].endPos, maxSize);

                
                if (incCount < maxSize) {

                    space = 0;
                    if (templ[templPos] == -1) { //находим отступ джокеров
                        while (templ[templPos] == -1 && templPos < templ.size()) {
                            ++space;
                            ++templPos;
                        }
                        if (DEBUG)printf("jumped for %lu\n", space);
                    }

                    if (DEBUG)printf("%lu <=space|templPos=> %lu (%lu|%lu)\n", space, templPos, templ[templPos], preRes[j].numberOfPattern);
                    if (DEBUG)printf("%lu|%lu\n", incCount, maxSize);
                    if (preRes[j].numberOfPattern == templ[templPos] && preRes[j].startPos == preRes[j - 1].endPos + space + 1) { //проверяем, подходит ли результат под продолжение
                        incCount++;

                        if (incCount < maxSize) {
                            templPos++;
                        }
                    }
                    else { //пока не наткнемся на подходящее вхождение
                        continue;
                    }
                }

                if (incCount == maxSize) {//проверяем вхождение, если удалось достроить
                    if (DEBUG)printf("inclusion possible\n");
                    if (DEBUG)printf("%lu | %lu\n", templPos + 1, templ.size());
                    if (templPos + 1 != templ.size()) {
                        if (DEBUG)printf("final J chek\n");
                        if (DEBUG)printf("%lu | %lu\n", preRes[j].endPos, max);
                        if (preRes[j].endPos + (templ.size() - (templPos + 1)) > max) {

                            if (DEBUG)printf("chek failed\n");
                            break;
                        }

                        if (DEBUG)printf("chek passed\n");
                    }

                    finalRes.push_back(result(0, beg+1, preRes[j].endPos + (templ.size() - (templPos + 1))));
                    if (DEBUG)printf("inclusion confirmed\n");
                    break;
                }
            }
        }
    }

        
        
    return finalRes;
}

int main() {
    TTrie mainTrie = TTrie();

    unsigned long inp = 0;

    char sq;
    int lastq = -1;
    std::vector<unsigned long> inpword;
    std::vector<unsigned long> textmodula;
    std::vector<unsigned long> jPattern;
    while (scanf("%c", &sq)) {
        if ((sq >= '0') && (sq <= '9')) {
            inp *= 10;
            inp += sq - '0';
            lastq = 0;
        }
        else
            if (sq == ' ' && lastq == 0) {
                inpword.push_back(inp);
                inp = 0;
                if (DEBUG)printf("pushed\n");
            }
            else
                if (sq == '?') {
                    if (lastq == 0) {

                        unsigned long ii = mainTrie.add(inpword);
                        inpword.clear();
                        if (DEBUG)printf("added\n");

                        jPattern.push_back(ii);
                        jPattern.push_back(-1);
                    }
                    else {
                        jPattern.push_back(-1);
                    }
                    lastq = 1;
                }
        if (sq == '\n') {
            if (lastq == 0) {
                inpword.push_back(inp);
                unsigned long ii = mainTrie.add(inpword);
                inpword.clear();
                if (DEBUG)printf("added\n");
                jPattern.push_back(ii);
            }
            break;
        }
    }

    unsigned long ms = 0;
    for (int i = 0; i < jPattern.size(); ++i) {
        if (jPattern[i] == -1) {
            if (DEBUG)printf("j");
        }
        else {
            if (DEBUG)printf("%lu", jPattern[i]);
            ++ms;
        }
    }
    if (DEBUG)printf("\n");

    inpword.clear();
    unsigned long textSize = 1;
    /*while (scanf("%lu", &inp)!=EOF) {
        if (inp == 0) {
            break;
        }
        inpword.push_back(inp);
        ++textSize;
    }*/
    inp = 0;
    int spx = 0;
    textmodula.push_back(1);
    while (scanf("%c", &sq) != EOF) {
        if ((sq >= '0') && (sq <= '9')) {
            inp *= 10;
            inp += sq - '0';
            lastq = 0;
            spx = 0;
        }
        else
        if (sq == ' ') {
            inpword.push_back(inp);
            inp = 0;
            spx = 1;
            textSize++;
        }
        if (sq == '\n') {
            if (spx == 0) {
                inpword.push_back(inp);
                textSize++;
            }
            textmodula.push_back(textSize);
        }
    }


    //mainTrie.print();
    mainTrie.suffixate();

    std::vector<result> preRes = search(&mainTrie, inpword);
    std::vector<result> finalRes = collect(preRes, ms, jPattern, textSize);

    if(DEBUG == true) {
        for (int i = 0; i < finalRes.size(); ++i) {
            printf("inclusion %d: %lu-%lu\n", i, finalRes[i].startPos, finalRes[i].endPos);
        }
    }
    for (int i = 0; i < finalRes.size(); ++i) {
        for (int j = 0; j < textmodula.size(); ++j) {
            if (DEBUG == true) printf("newstr: %lu\n", textmodula[j]);
            if (finalRes[i].startPos >= textmodula[j]) {
                printf("%lu, %lu\n", textmodula[j], finalRes[i].startPos - textmodula[j] + 1);
            }
        }
    }
    return 0;
}