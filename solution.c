#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

const int DATA_STR_LEN = 257;
const int RED = 1;
const int VAL_LEN = 21;
typedef unsigned long long int TVal;

struct TNode {
    TVal val;
    int red;
    char* data;
    struct TNode *link [2];
};
struct TTree {
    struct TNode *root;
};
struct TNode *RRemove (struct TNode *root, char * data, int *done);
short int TRemove (struct TTree *tree, char * data);
struct TNode *RemoveBalance (struct TNode *root, int dir, int *done);
short int IsRed (struct TNode *root);
struct TNode *RotDouble (struct TNode *root, int dir);
struct TNode *RotSingle (struct TNode *root, int dir);
short int TInsert (struct TTree *tree, const char * data, TVal val);
struct TNode *RInsert (struct TNode *root, const char * data, TVal val);
void TSearch (struct TNode *root, const char *data);
short int MyIsAlpha(const char *str);
void DeleteTree (struct TNode *root);
short int DeSerialize(struct TNode *root, FILE *f);
void Serialize(struct TNode *root, FILE *f);
struct TNode *MakeTNode (const char * data, TVal val);
struct TNode *MakeTNodeDes (const char * data, TVal val, int red);
unsigned short int SaveTree(struct TTree* tree, const char* path);
short int LoadTree(struct TTree* tree, const char* path);

void DeleteTree (struct TNode *root){
    if (root){
        DeleteTree (root->link[0]);
        DeleteTree (root->link[1]);
        free (root->data);
        free (root);
        root = NULL;
    }
}
short int IsRed (struct TNode *root){
    return root != NULL && root->red == RED;
}
struct TNode *RotSingle (struct TNode *root, int dir){
    struct TNode *save = root->link [!dir];
    root->link [!dir] = save->link [dir];
    save->link [dir] = root;
    root->red = 1;
    save->red = 0;
    return save;
}
struct TNode *RotDouble (struct TNode *root, int dir){
    root->link [!dir] = RotSingle (root->link [!dir], !dir);
    return RotSingle (root, dir);
}
struct TNode *MakeTNode (const char * data, TVal val){
    struct TNode *tmp = malloc (sizeof *tmp);
    tmp->data = malloc (DATA_STR_LEN);
    if (tmp != NULL) {
        strcpy (tmp->data, data);
        tmp->val = val;
        tmp->red = 1; 
        tmp->link [0] = NULL;
        tmp->link [1] = NULL;
    }
    return tmp;
}

struct TNode *MakeTNodeDes (const char * data, TVal val, int red){
    struct TNode *tmp = malloc (sizeof *tmp);
    tmp->data = malloc (DATA_STR_LEN);
    if (tmp != NULL) {
        strcpy (tmp->data, data);
        tmp->val = val;
        tmp->red = red; /* 1 is red, 0 is black */
        tmp->link[0] = NULL;
        tmp->link[1] = NULL;
    }
    return tmp;
}

void TSearch(struct TNode *root, const char *data){
    if (root == NULL){
        printf("NoSuchWord\n");
        return;
    }
    short int tmp = strcmp (data, root->data);
    int dir = 0;
    if (tmp == 0){
        printf("OK: %llu\n", root->val);
    }else{ 
        dir = tmp > 0;
        TSearch(root->link[dir], data);
    }
    return;
}

struct TNode *RInsert (struct TNode *root, const char * data, TVal val){
    if (root == NULL){
        root = MakeTNode (data, val);
        if (root != NULL){
            printf ("OK\n");
        }else{
            printf("ERROR: Allocate error\n");
        }
    }else{ 
        int dir;
        short int tmp = strcmp(data, root->data);
        if (tmp == 0){
            printf("Exist \n");
            return root;
        }else{
            dir = tmp > 0;
        }
        root->link [dir] = RInsert (root->link [dir], data, val);
        if (IsRed (root->link [dir])) {
            if (IsRed (root->link [!dir])) {
                /* Случай  1 */
                root->red = 1;
                root->link [0]->red = 0;
                root->link [1]->red = 0;
            }else{
                /* Случай 2 & 3 */
                if (IsRed (root->link [dir]->link [dir]))
                    root = RotSingle (root, !dir );
                else if (IsRed (root->link [dir]->link [!dir]))
                    root = RotDouble (root, !dir);
            }
        }
    }
    return root;
}
short int TInsert (struct TTree *tree, const char * data, TVal val){
    tree->root = RInsert (tree->root, data, val);
    tree->root->red = 0;
    return 0;
}

struct TNode *RRemove (struct TNode *root, char * data, int *done){
    if (root == NULL){
        printf("NoSuchWord\n");
        *done = 1;
    }else{
        int dir; 
        short int tmp = strcmp(data, root->data);
        if (tmp == 0) {
            if (root->link [0] == NULL || root->link [1] == NULL) {
                struct TNode *save = root->link [root->link [0] == NULL];
                if (IsRed (root)){
                    *done = 1;
                }else if (IsRed (save)) {
                    save->red = 0;
                    *done = 1;
                }
                printf ("OK\n");
                free(root->data);
                free (root);
                root = NULL;
                return save;
            }else{
                struct TNode *heir = root->link [0]; 
                while (heir->link [1] != NULL){
                    heir = heir->link [1];
                }
                
                strcpy(root->data, heir->data);
                strcpy(data, heir->data);
                root->val = heir->val;
            }
        }
        tmp = strcmp(data, root->data);
        dir = tmp > 0;
        root->link [dir] = RRemove (root->link [dir], data, done);
        if (!*done){
            root = RemoveBalance (root, dir, done);
        }
    }
    return root;
}
short int TRemove (struct TTree *tree1, char * data){
    int done = 0;
    tree1->root = RRemove (tree1->root, data, &done);
    if (tree1->root != NULL){
        tree1->root->red = 0;
    }
    return 0; 
}

 struct TNode *RemoveBalance (struct TNode *root, int dir, int *done){
    struct TNode *p = root;
    struct TNode *s = root->link [!dir];
    if (IsRed (s)) {
        root = RotSingle (root, dir);
        s = p->link [!dir];
    }
    if (s != NULL) {
        if (!IsRed (s->link [0]) && !IsRed (s->link [1])) {
            if (IsRed (p)){
                *done = 1;
            }
            p->red = 0;
            s->red = 1;
        }else{
            int save = p->red;
            int new_root = (root == p);
            if (IsRed (s->link [!dir]))
                p = RotSingle (p, dir);
            else
                p = RotDouble (p, dir);
            p->red = save;
            p->link [0]->red = 0;
            p->link [1]->red = 0;
            if (new_root){
                root = p;
            }else{
                root->link [dir] = p;
            }

            *done = 1;
        }
    }
   return root;
}

short int MyIsAlpha(const char *str){
    for(int i = 0; i < strlen(str); i++){
        if(!isalpha(str[i])){
            return 0;
        }
    }
    return 1;
}

unsigned long long Digit(const char *val1){
    unsigned long long val = 0;
    for (int i = 0; i < strlen(val1); i++){
        val = val * 10 + (val1[i] - '0');
    }
    return val;
}

void Serialize(struct TNode *root, FILE *f){
    if (root->link[0]){
        fprintf(f,"L\n");
        Serialize(root->link[0], f);
    }
    if (root->link[1]){
        fprintf(f,"R\n");
        Serialize(root->link[1], f);
    }
    fprintf(f,"%s\n%llu\n%d\n", root->data,root->val,root->red);
    return;
}

unsigned short int SaveTree(struct TTree* tree, const char* path){
    FILE* f = fopen(path, "wb");
    if(!f){
        return 1;
    }
    if(tree->root){
        fputs("[\n",f);
        Serialize(tree->root, f);
    }else{
        fputs("[",f);
        fputs("\n",f);
        fputs("]",f);
        fclose(f);
        printf ("OK\n");
        return 0;
    }
    fputs("]",f);
    fclose(f);
    printf ("OK\n");
    return 0;
}

short int DeSerialize(struct TNode *root, FILE *f){
    char str[DATA_STR_LEN];
    TVal val = 0;
    int red = 0, error = 0;
    if(fscanf(f,"%s",str) != EOF){
        if (str[0] == 'L'){
            root->link[0] = MakeTNodeDes("\0", 0, 0);
            if (root->link[0] == NULL){
                printf("ERROR: Allocate error\n");
                return 2;
            }
            error = DeSerialize(root->link[0], f);
            if (error == 2){
                return 2;
            }
            if(fscanf(f,"%s",str) == EOF){
                printf("ERROR: Invalid data\n");
                return 2;
            }
        }
        if (str[0] == 'R'){
            root->link[1] = MakeTNodeDes("\0", 0, 0);
            if (root->link[1] == NULL){
                printf("ERROR: Allocate error\n");
                return 2;
            }
            error = DeSerialize(root->link[1], f);
            if (error == 2){
                return 2;
            }
            if(fscanf(f,"%s",str) == EOF){
                printf("ERROR: Invalid data\n");
                return 2;
            }
        }
        if (str[0] == ']'){
            return 1;
        }
        
        if (MyIsAlpha(str)){
            if(fscanf(f,"%llu %d", &val, &red) != EOF){
                root->val = val;
                root->red = red; 
                strcpy (root->data, str);
            }else{
                printf("ERROR: Invalid data\n");
                return 2;
            }
        }
    }else{
        printf("ERROR: Invalid data\n");
        return 2;
    }
    return 0;
}
short int LoadTree(struct TTree* tree, const char* path){
    FILE* f = fopen(path, "rb");
    if(!f){
        return 1;
    }
    const short int TWO_SYMBOL = 2;
    char str[TWO_SYMBOL];
    short error = 0;
   if(fscanf(f,"%s",str) != EOF){
        if (str[0] == '['){
            tree->root = MakeTNodeDes("\0", 0, 0);
            if (tree->root == NULL){
                printf("ERROR: Allocate error\n");
                return 2;
            }
            error = DeSerialize(tree->root, f);
            if (error == 2){
                return 2;
            }
            if(fscanf(f,"%s",str) != EOF || error == 1){ // ] считывается до этого момента ОШИБКА
                if (str[0] == ']' || error == 1){
                    printf("OK\n");
                }else{
                    printf("ERROR: Wrong file 1\n");
                }
            }else{
                printf("ERROR: Wrong file 2\n");
            }
        }else{
            printf("ERROR: Wrong file 3\n");
        }
    }else{
        fclose(f);
        return 1;
    }
    fclose(f);
    return 0;
}

char Skip(char c){
    while (c != '\n'){
        c = tolower(getchar());
    }
    c = tolower(getchar());// считываем команду
    printf("ERROR: Invalid data\n");
    return c;
}

int main(){
    TVal val = 0;
    char str[DATA_STR_LEN];
    char val1[VAL_LEN];
    struct TTree tree;
    tree.root = NULL;
    char c;
    int i = 0;
    short error = 0;
    c = tolower(getchar());

    while (c > 0) { 
        if (c == '\n'){
            c = tolower(getchar());
            continue;
        }
        memset(val1, 0, VAL_LEN);
        memset(str, 0, DATA_STR_LEN); //обнуляем str
        i = 0;
        if (c == '+'){ // добавление в дерево 
            if(tolower(getchar()) != ' '){ // если после командного символа не идет пробел то строка неправильная 
                c = Skip(c);// считываем команду
                continue;
            }
            c = tolower(getchar());
            while (c >= 'a' && c <='z'){ // пока считывается слово заносим его в строку
                str[i] = c;
                ++i;
                c = tolower(getchar());
            }
            i = 0;
            if(c != ' '){ // если после слова не идет пробел то строка неправильная 
                c = Skip(c);// считываем команду
                continue;
            }
            c = tolower(getchar());
            while (c >= '0' && c <='9'){ // пока считывается число заносим его в строку
                val1[i] = c;
                ++i;
                c = tolower(getchar());
            }
            if(c != '\n'){ // если считываемая строка содержит символы кроме c >= 'a' && c <='z' и c >= '0' && c <='9'
                c = Skip(c);// считываем команду
                continue;
            }
            val = Digit(val1);
            TInsert(&tree, str, val);
        }else if (c == '-'){ // удаление 
            if(tolower(getchar()) != ' '){ // если после командного символа не идет пробел то строка не правильная 
                c = Skip(c);// считываем команду
                continue;
            }
            c = tolower(getchar());
            while (c >= 'a' && c <='z'){ // пока считывается слово заносим его в строку
                str[i] = c;
                ++i;
                c = tolower(getchar());
            }
            if(c != '\n'){ // если считываемая строка содержит символы кроме c >= 'a' && c <='z'
                c = Skip(c);// считываем команду
                continue;
            }
            TRemove(&tree, str);
        }else if (c >= 'a' && c <='z'){ // поиск 
            while (c >= 'a' && c <='z'){ // пока считывается слово заносим его в строку
                str[i] = c;
                ++i;
                c = tolower(getchar());
            }
            if(c != '\n'){ // если считываемая строка содержит символы кроме c >= 'a' && c <='z'
                c = Skip(c);// считываем команду
                continue;
            }
            TSearch(tree.root, str);
        }else if (c == '!'){ // Save/Load
            if(tolower(getchar()) != ' '){ // если после командного символа не идет пробел то строка не правильная 
                c = Skip(c);// считываем команду
                continue;
            }
            if(scanf("%s", str)!= EOF){
                if(strcmp(str, "Save") == 0){ // ! Save /path/to/file 
                    if(scanf("%s", str)!= EOF){
                        error = SaveTree(&tree, str);
                        if(error > 0){
                            printf("ERROR: Wrong command or format\n");
                        }
                    }else{
                        printf("ERROR: Invalid file name\n");
                    }
                }else if(strcmp(str, "Load") == 0){//! Load /path/to/file 
                    struct TTree tmproot;
                    tmproot.root = NULL;
                    if(scanf("%s", str)!= EOF){
                        error = LoadTree(&tmproot, str);
                        if(error > 0){
                            printf("ERROR: Wrong command or format\n");
                        }
                        if ((tmproot.root) && (error == 0)){
                            DeleteTree(tree.root);
                            tree.root = tmproot.root;
                            tmproot.root = NULL;
                        }else{
                            DeleteTree(tmproot.root);
                            tmproot.root = NULL;
                        }
                        if (tmproot.root){
                            DeleteTree(tmproot.root);
                        }
                    }else{
                        printf("ERROR: Invalid file name\n");
                    }
                }
                c = tolower(getchar());
                if(c != '\n'){ // если считываемая строка содержит символы кроме c >= 'a' && c <='z'
                    while (c != '\n'){
                        c = tolower(getchar());
                    }
                    c = tolower(getchar());// считываем команду
                    continue;
                }
            }
        }else{
            c = Skip(c);
            continue;
        }
        c = tolower(getchar());
    }
    if (tree.root){
        DeleteTree(tree.root); 
    }
    return 0;
}