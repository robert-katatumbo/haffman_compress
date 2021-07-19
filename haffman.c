#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <limits.h>

#define TIME double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; printf("%f\n",cpu_time_used);

const char *filename = "result.compress";

struct tree{
    int symb;
    unsigned int count;
    struct tree *left;
    struct tree *right; 
    char *code;
};

struct list{
    char nav;    
    struct list *next;
};

struct push{
    unsigned char x1;
    unsigned char x2;
    unsigned char x3;
    unsigned char x4;
};

struct list* push_last(struct list **first, char data){
    struct list *res = *first;
    while(res->next)
        res = res->next;

    struct list *tmp = malloc(sizeof(struct list));
    tmp->nav = data;
    res->next = tmp;
    return *first;
}

void free_last(struct list *first){
    while(first->next->next)
        first = first->next;
    
    free(first->next);
    first->next = NULL;
}

void free_code(struct tree *root, int size){
    for(int i = 0; i < size; ++i){
        free(root[i].code);
    }
}

void free_list(struct list *first){
    if(!first)
        return;  
    free_list(first->next);
    free(first);
}

int list_len(const struct list* first){
    int len = 0;
    while(first->next){
        first = first->next;
        ++len;
    }
    return len;
}

void list_to_path(struct tree *root,struct list *path, int index){

    int i = 0;
    int len = list_len(path);   
    root[index].code = malloc(len);
    path = path->next;
    
    for(i = 0; i<len; ++i){
        root[index].code[i] = path->nav;
        path = path->next;
    }
    root[index].code[i] = '\0';
    return;
    
}

int index_of(const struct tree *root, int find){
    int i = 0;
    while(root[i].symb != find)
        ++i;
    return i;
}

void init_path(struct tree *root, struct list *path, struct tree *begin){   
//  Проход по дереву и запоминание пути в конях

	if((!root->left) && (!root->right)){
        list_to_path(begin,path,index_of(begin,root->symb));
        free_last(path);
      	return;
    }    
    
    init_path(root->right,push_last(&path,'1'),begin);
    if(root->right->symb == -1)
        free_last(path);
    
    init_path(root->left,push_last(&path,'0'),begin);
    if(root->left->symb == -1)
        free_last(path);
}


int checker(const struct tree *r, const int size){
	for(int i =0; i < size-1; ++i){
		if(r[i].count > r[i+1].count){
            return 1;   //bad sort
        }
	}
    return 0;   //good sort
}

void quick_sort_grok(struct tree *r, const int size){
    unsigned int opora;
    int l = 0;
    struct tree tmp;
    if(size < 2)
        return;
    
    if(size == 2){
        if(r[0].count > r[1].count){
            tmp = r[0];
            r[0] = r[1];
            r[1] = tmp;   
            return;     
    }}
    opora = rand()%size;
    opora = r[opora].count;
    
    for(int i=0; i<size; ++i){
        if(r[i].count < opora){
            tmp  = r[l];
            r[l] = r[i];
            r[i] = tmp;
            ++l;
        }
    }
    if(!l && !checker(r,size)) 
            return;
        //  if infinity cycle and 
        //  sorted this asea -> return
    quick_sort_grok(r,l);
    quick_sort_grok(r+l,size-l);
}

void writing_alphabet(struct tree *root, FILE *fres, int ch){
// Запись алфавита в файл

    struct push *writer;
    fputc(ch,fres);
    for(int i = 0; i < ch; ++i){
        // Запись символа
        fputc(root[i].symb,fres);
        // Запись 4х байт uint
        writer = (struct push*)&root[i].count;
        fputc(writer->x1,fres);  
        fputc(writer->x2,fres);        
        fputc(writer->x3,fres);        
        fputc(writer->x4,fres);
    }    
}

void build_haffman(struct tree *root, const int ch){
// Создание дерева двумя массивами

    int m; //first min
    int n; //second min
    int RT = ch; // Чтение с дерева
    int RA = 0;  // Чтение с массива
    int WT = ch; // Запись в дерево
    

while(RA < ch-1){

        m = RA; // вервый минимальный элемент
        if(root[m].count > root[RA+1].count){
            ++m;
        }
        if(root[m].count > root[RT].count){
            m = RT;
        }
        if(root[m].count > root[RT+1].count){
            m = RT+1;
        }
    
            if(m == RT)
                  ++RT;
            else  ++RA;

        n = RA; // второй минимальный элемент
        if((RA+1<ch) && (root[n].count > root[RA+1].count)){
            ++n;
        }
        if(root[n].count > root[RT].count){
            n = RT;
        }
        if(root[n].count > root[RT+1].count){
            n = RT+1;
        }

            if(n == RT)
                  ++RT;
            else  ++RA;
            
        root[WT].count = root[m].count + root[n].count;
        root[WT].left = &root[m];
        root[WT].right = &root[n];    
        ++WT; // Запись в дерево
    }
    //RA > c - чтение с дерева и последнего элемента массива
        while(WT+1 < 2*ch){

        if(RA < ch){
            m = RA; // вервый минимальный элемент
            if(root[m].count > root[RT].count)
                m = RT;
        }else{
            m = RT;
            if(root[m].count > root[RT+1].count)
                m = RT+1;
        }

        if(m == RT)
              ++RT;
        else  ++RA;
        
        if(RA < ch){
            n = RA; // второй минимальный элемент
            if(root[n].count > root[RT].count)
                n = RT;
        }
        else{
            n = RT;
        }
        if(root[n].count > root[RT+1].count)
            n = RT+1;

        if(n == RT)
              ++RT;
        else  ++RA;

        root[WT].count = root[m].count + root[n].count;
        root[WT].left = &root[m];
        root[WT].right = &root[n];    
         ++WT; // Запись в дерево
        }
}


int main(const int argc, const char **argv){
    if(argc < 2){
        printf("Not enough argv\n");
        printf("$ ./haffman option filename\n");
        printf("option{-c to compress, -d to uncompress}\n");
        return 0;    
    }
 
    if(!(argv[1][0] == '-' && (argv[1][1]=='c'||argv[1][1]=='d')))
        return 0;
    
    clock_t start, end;
    start = clock();
    const int byte = 256;
    int ch = 0;
    int i = 0;
    int tmp = 0;
    FILE *f;

    if(argv[1][1] == 'c'){
    
    //                            //
    //                            //
    //       COMPRESS ZONE        //
    //                            //
    //                            //

    f = fopen(argv[2],"r");
    if(!f){
        perror("");
        return 1;        
    }
    int *helper = calloc(256,sizeof(int));
    
    while((ch = fgetc(f))!=EOF){
        ++helper[ch]; 
    }
    
    ch = 0;
    
    for(i = 0; i < byte; ++i){
        if(helper[i])
        ++ch;
    }   //ch - количество ненулевых символов

    // root - будущее дерево
    struct tree *root = calloc(2*ch,sizeof(struct tree));
    struct list *path = malloc(sizeof(struct list));

    for(i = 0; i < byte; ++i){    
        if(!helper[i])
            continue;
        root[tmp].symb = i;
        root[tmp].count = helper[i];
        root[ch+tmp].count = INT_MAX;
        //root[ch+tmp].count = 999;
        root[ch+tmp].symb = -1;
        ++tmp;
    }
    // Сортировка массива символ - частота    
    quick_sort_grok(root,ch);

    // Генерация дерева    
    build_haffman(root,ch);

    // Обход по дереву и запомининание пути    
    init_path(&root[ch*2-2],path,root);


    
    // Вывод путей    
//    printf("\n");
//    for(i = 0; i < ch; ++i){
//        printf("%p\t%d\t%s",root[i].symb,root[i].count,root[i].code);    
//        printf("\n");
//    }
        
    FILE *fres = fopen(filename,"w");
    if(!fres){
        perror("");
        return 1;    
    }
    
    //  Запись алфавита в результирующий текст
    writing_alphabet(root,fres,ch);

    unsigned char wr = 0;
    unsigned char mask = 128;   //10000000
    int str = 0;
    //  Повторное чтение исходого текста
    rewind (f);  
    
    
    //  Побитовая запись
    while((tmp = fgetc(f))!=EOF){
        for (i = 0; i<ch; ++i){
            if(root[i].symb == tmp)
                break;
        }
        for(str = 0;root[i].code[str]; ++str){
            if(root[i].code[str] & 1){  // 49 = 1
                wr |= mask;
                mask >>= 1;
            } else {                   // 48 = 0
                mask >>= 1;
            }   
            if(!mask){  // mask = 0
                fputc(wr,fres);
                wr = 0;
                mask = 128; 
            }
        
        }
    }fputc(wr,fres); // назополненный байт, может быть мусор

    printf("created result.compress\n");

    // Освобождение памяти
    free_code(root,ch);
    free_list(path);
    free(helper);
    free(root);
    fclose(f);
    
    }  else if(argv[1][1] == 'd') {                      
    //                            //
    //                            //
    //      DECOMPRESS ZONE       //
    //                            //
    //                            //
    
    ch = 0;
    f = fopen(argv[2],"r");
    if(!f){
        perror("");
        return 1;        
    }
    // Считывание алфавита
    // Первый байт алфавита - количество символов (ненулевых)
    ch = fgetc(f);
    if(ch == EOF){
        perror("");
        exit(1);        
    }
    int size = ch;
    struct tree *root = calloc(2*ch,sizeof(struct tree));
    struct push *reader = malloc(sizeof(struct push));

    // Чтение алфавита
    for(i = 0; i < size; ++i){
    // Считывание символа
        root[i].symb = fgetc(f);
    
    // Считывание uint
        reader->x1 = fgetc(f);
        reader->x2 = fgetc(f);
        reader->x3 = fgetc(f);
        reader->x4 = fgetc(f);
        root[i].count = *((unsigned int*)&reader->x1); // ебанутый..
            
        root[ch+i].count = INT_MAX;
        root[ch+i].symb = -1;
    }
    
    // Генерация дерева
    build_haffman(root,ch);
    
    // head указывает на корень дерева
    // reader будет бегать по дереву, пока не встретит символ
    struct tree *head = &root[ch*2-2];
    struct tree *runner = head; 
    unsigned char mask = 128;   //10000000
      
    while((ch = fgetc(f))!=EOF){
        while(mask>0){
            if(mask & ch)       //  1
                runner = runner -> right;
            else                //  0
                runner = runner -> left;
        
            if(runner->symb != -1){
                printf("%c",runner->symb); 
                runner = head;
            } 
            mask>>=1;
        }
        mask = 128;
    }
    
    printf("\n");
    free(reader);
    free(root);
    fclose(f); 
}
    // Подсчет времени
    end = clock();
    TIME
    return 0;
}


