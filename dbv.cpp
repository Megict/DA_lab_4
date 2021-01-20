#include <iostream>
#include <vector>
#include <cstring>

#define DEBUG false

class TTrieNode {
private:
    std::vector<TTrieNode*> children;
    std::vector<unsigned long> term;
    unsigned long litera = 0;
    TTrieNode* suffix;
    TTrieNode* parent;
    TTrieNode* Uplink;
public:
    TTrieNode(unsigned long inp) {
        litera = inp;
        suffix = nullptr;
        Uplink = nullptr;
        parent = nullptr;
    }

    ~TTrieNode() {
        for (unsigned long i = 0; i < children.size(); i++) {
            delete(children[i]);
        }
    }

    void setUp(TTrieNode* suff) {
        Uplink = suff;
    }

    void setSuff(TTrieNode* suff) {
        suffix = suff;
    }

    void setPar(TTrieNode* par) {
        parent = par;
    }

    unsigned long liter() {
        return litera;
    }

    std::vector<TTrieNode*> child() {
        return children;
    }

    bool hasChild(unsigned long a) {
        for (unsigned long i = 0; i < children.size(); ++i) {
            if (children[i]->litera == a) {
                return true;
            }
        }
        return false;
    }

    unsigned long isTerm() {
        return term.size();
    }

    std::vector<unsigned long> Term() {
        return term;
    }

    void addChild(TTrieNode* adb) {
        adb->setPar(this);
        children.push_back(adb);
    }

    void setterm(unsigned long count) {
        term.push_back(count);
    }

    TTrieNode* child(unsigned long a) {
        for (unsigned long i = 0; i < children.size(); ++i) {
            if (children[i]->litera == a) {
                return children[i];
            }
        }
        return nullptr;
    }

    TTrieNode* back() {
        return suffix;
    }

    TTrieNode* Up() {
        return Uplink;
    }

    TTrieNode* Parent() {
        return parent;
    }
};

void printtrie(TTrieNode* root,int depth) {
    for (int i = 0; i < depth; ++i) {
        printf("\t");
    }
    if (root->isTerm()) {
        printf("<!");
        std::vector<unsigned long> curT = root->Term();
        for(unsigned long k =0;k < curT.size();++k) { 
            printf("%lu,",curT[k]);
        }
        printf("!>");
    }
    printf("|%lu|[]", root->liter());
    std::vector<TTrieNode*> next = root->child();
    printf("%lu>", next.size());
    if (root->back()!=nullptr) {
        printf("(%lu)",root->back()->liter());
    }
    printf("\n");
    for (unsigned long i = 0; i < next.size(); ++i) {
        printtrie(next[i],depth+1);
    }
}

std::vector<unsigned long> reconstruct(TTrieNode* root, TTrieNode* end) {
    std::vector<unsigned long> word;

    while(end != root) {
        word.push_back(end->liter());
        end = end->Parent();
    }

    std::vector<unsigned long> res(word.size());
    for(unsigned long i=0;i<res.size();++i) {
        res[i] = word[word.size()-i-1];
    }
    
    return res;
}

TTrieNode* correlate(TTrieNode* root, std::vector<unsigned long> word) {
    for (unsigned long i = 1; i < word.size(); ++i) {
        TTrieNode* cur = root;
        for (unsigned long j = i; j < word.size();++j) {
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
        for (unsigned long i = 0; i < word.size(); ++i) {
            if(DEBUG)printf("  |%lu\n", word[i]);
        }
        word = reconstruct(root,cur);
        TTrieNode* suff = correlate(root, word);
        if (DEBUG) {
            if (suff != root) {
                printf("suffix leads to: %lu, []\n", suff->liter());
            }
            else {
                printf("suffix leads to root\n");
            }
        }
        cur->setSuff(suff);
    }

    std::vector<TTrieNode*> next = cur->child();
    for (unsigned long i = 0; i < next.size(); ++i) {
        select(root, next[i], word);
    }
}

int reconst(TTrieNode* begin, TTrieNode* end, int depth) {
    
    if (begin == end) {
        return depth;
    }

    std::vector<TTrieNode*> next = begin->child();
    for (unsigned long i = 0; i < next.size(); ++i) {
        int tmp = reconst(next[i], end, depth + 1);
        if (tmp != -1) {
            return tmp;
        }
    }

    return -1;
}

TTrieNode* sufGet(TTrieNode* root, TTrieNode* forWhom) {
    if(forWhom->back() != nullptr) {
        return forWhom->back();
    }
    else {
        std::vector<unsigned long> word = reconstruct(root,forWhom);
        TTrieNode* res = correlate(root, word);
        forWhom->setSuff(res);
        return res;
    }
}

TTrieNode* nextGet(TTrieNode* root, TTrieNode* forWhom, unsigned long to);

TTrieNode* sufGetByG(TTrieNode* root, TTrieNode* forWhom) {
    if(forWhom->back() != nullptr) {
        return forWhom->back();
    }
    else {
        if(forWhom == root || forWhom->Parent() == root) {
            forWhom->setSuff(root);
            return root;
        }
        else {
            forWhom->setSuff(nextGet(root,sufGetByG(root,forWhom->Parent()),forWhom->liter()));
            return forWhom->back();
        }
    }
}

TTrieNode* upGet(TTrieNode* root, TTrieNode* forWhom) {
    if(forWhom->Up() != nullptr) {
        return forWhom->Up();
    }
    else {
        TTrieNode* cur = sufGetByG(root, forWhom);
        while(cur != root) {
            if(cur->isTerm()) {
                break;
            }

            cur = sufGetByG(root, cur);
        }
        
        forWhom->setUp(cur);
        return cur;
    }
}

TTrieNode* nextGet(TTrieNode* root, TTrieNode* forWhom, unsigned long to) {
    if(forWhom->child(to) == nullptr) {
        if(forWhom == root) {
            return root;
        }
        else {
            return nextGet(root, sufGetByG(root,forWhom),to);
        }
    }
    else {
        return forWhom->child(to);
    }
}


class TTrie {
private:
    TTrieNode* root;
    unsigned long maxSize;
    long long count = 0;
public:
    TTrie() {
        root = new TTrieNode(0);
        maxSize = 0;
        count = 0;
        root->setSuff(root);
    }

    ~TTrie() {
        delete root;
    }

    unsigned long Count() {
        return count;
    }

    long add(std::vector<unsigned long> word, unsigned long id) {
        if (word.size() > maxSize) {
            maxSize = word.size();
        }

        TTrieNode* cur = root;

        for (unsigned long i = 0; i < word.size(); ++i) {
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

        count++;
        cur->setterm(id);
        return count;
    }

    bool contains(char* word) const {
        TTrieNode* cur = root;

        for (unsigned long i = 0; i < strlen(word); ++i) {
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

std::vector<int> search(TTrie* trie,std::vector<unsigned long> text) {
    std::vector<int> ret(text.size(),0);
    TTrieNode* cur = trie->Root();


    for (unsigned long i = 0; i < text.size(); ++i) {
        
        cur = nextGet(trie->Root(),cur,text[i]);

        if (DEBUG){
            if(cur == trie->Root())
                printf("step %lu. node in trie [ROOT], item: ROOT | item in text: %lu\n", i, text[i]);
            else
                printf("step %lu. node in trie [], item: %lu | item in text: %lu\n", i, cur->liter(),text[i]);
        }
        
        if (cur->isTerm()) {
            if (DEBUG)printf("\tpattern found (%lu) from %lu to %lu\n", cur->Term()[0], i - reconst(trie->Root(), cur, 0) + 2, i + 1);
            
            std::vector<unsigned long> curT = cur->Term();
            for(unsigned long k =0;k < curT.size();++k) {
                if(i + 1 >= curT[k]) {
                    ret[i-curT[k]+1]++;
                    if (DEBUG)printf("uppos %lu\n",i + 1 - curT[k]);
                }
            }
        }
        
        //if ( i + 1 == text.size() || nextGet(trie->Root(),cur,text[i + 1]) != trie->Root()) {
            TTrieNode* cheker = upGet(trie->Root(),cur);
            if (cheker == trie->Root()) {
                if (DEBUG) printf("\tupling goes to ROOT\n");
            }
            while(cheker != trie->Root()) {
                if (DEBUG) {
                    if(cheker != trie->Root())
                        printf("\tcheking by suffix links []\n");
                    else
                        printf("\tcheking by suffix links IN ROOT\n");
                }

                if (cheker->isTerm()) {
                    if (DEBUG)printf("\t\tpattern found (%lu) from %lu  to %lu\n", cheker->Term()[0], i - reconst(trie->Root(), cheker, 0) + 2, i + 1);
                    
                    std::vector<unsigned long> curT = cheker->Term();
                    for(unsigned long k = 0;k < curT.size();++k) { 
                        if(i + 1 >= curT[k]) {
                            ret[i + 1 - curT[k]]++;
                            if (DEBUG)printf("uppos %lu\n",i + 1 - curT[k]);
                        }
                    }
                }
                
                cheker = upGet(trie->Root(),cheker);
            }
        //}
    }


    return ret;
}

int main() {
    TTrie mainTrie = TTrie();

    unsigned long inp = 0;

    char sq;
    int lastq = -1;
    std::vector<unsigned long> inpword;
    std::vector<unsigned long> textmodula;
    std::vector<long long> jPattern;
    bool writeReady = false;
    unsigned long count = 0; 
    unsigned long len = 0; 
    int patCOUNT = 0; 
    while (scanf("%c", &sq) != EOF) {
        if ((sq >= '0') && (sq <= '9')) {
            if(writeReady) {
                inpword.push_back(inp);
                ++len;
                inp = 0;
                if (DEBUG)printf("pushed\n");
                writeReady = false;
            }
            inp *= 10;
            inp += sq - '0';
            lastq = 0;
        }
        else
        if (sq == ' ' && lastq == 0) {
            writeReady = true;
        }
        else
        if (sq == '?') {
            if (lastq == 0) {
                if(writeReady) {
                    inpword.push_back(inp);
                    ++len;
                    inp = 0;
                    if (DEBUG)printf("pushed\n");
                    writeReady = false;
                }
                count += len;
                patCOUNT++;
                mainTrie.add(inpword,count);
                jPattern.push_back(count);
                len = 0;
                inpword.clear();
                if (DEBUG)printf("added\n");

                jPattern.push_back(-1);
            }
            else {
                jPattern.push_back(-1);
            }
            lastq = 1;
            ++count;
        }
        else
        if (sq == '\n') {
            if (lastq == 0) {
                inpword.push_back(inp);
                ++len;
                count += len;
                patCOUNT++;
                mainTrie.add(inpword,count);
                jPattern.push_back(count);
                len = 0;
                inpword.clear();
                if (DEBUG)printf("added\n");
            }
            break;
        }
    }

    unsigned long ms = 0;
    for (unsigned long i = 0; i < jPattern.size(); ++i) {
        if (jPattern[i] == -1) {
            if (DEBUG)printf("j");
        }
        else {
            if (DEBUG)printf("%lld", jPattern[i]);
            ++ms;
        }
    }
    if (DEBUG)printf("\ncount is: %lu\n",count);

    inpword.clear();
    unsigned long textSize = 0;
    
    inp = 0;
    int spx = 0;
    writeReady = false;
    textmodula.push_back(1);
    while (scanf("%c", &sq) != EOF) {
        if ((sq >= '0') && (sq <= '9')) {
            if(writeReady) {
                inpword.push_back(inp);
                inp = 0;
                textSize++;
                //writeReady = false;
            }

            inp *= 10;
            inp += sq - '0';
            lastq = 0;
            
            spx = 1;
        }
        else
        if (sq == ' ' && spx == 1) {
            inpword.push_back(inp);
                inp = 0;
                spx = 0;
                textSize++;
                //writeReady = false;
            spx = 0;
        }
        if (sq == '\n') {
            if(spx == 1) {
                inpword.push_back(inp);
                inp = 0;
                spx = 0;
                textSize++;
                //writeReady = false;
            }
            
            textmodula.push_back(textSize + 1);
        }
    }

    if(DEBUG == true) {
        for (unsigned long i = 0; i < inpword.size(); ++i) {
            printf("%lu ", inpword[i]);
        }
        printf(" <-inpw\n");
    }

    //mainTrie.suffixate();

    if(DEBUG) mainTrie.print();

    std::vector<int> preRes = search(&mainTrie, inpword);
    //std::vector<Tresult> finalRes = collect(preRes, ms, jPattern, textSize, count);

    if(DEBUG == true) {
        for (unsigned long i = 0; i < textmodula.size(); ++i) {
            printf("%lu ", textmodula[i]);
        }
        printf(" <-mod\n");
    }

    unsigned long modulapos = 1;
    if(preRes.size() >= count && count != 0)
    for(unsigned long i=0;i<=preRes.size()-count;++i) {
        if(DEBUG) printf("%d-%lu ",preRes[i],i);
        while(i+1 >= textmodula[modulapos]) {
            modulapos++;
        }
        if(preRes[i] == patCOUNT) {
            if(DEBUG)printf("\n");
            //printf("pattern at %lu\n", i+1);
            printf("%lu, %lu\n", modulapos, i+2 - textmodula[modulapos-1]);
            
        }
    }
    if(DEBUG) printf("\n");

    if (DEBUG) printf("end\n");
    return 0;
}