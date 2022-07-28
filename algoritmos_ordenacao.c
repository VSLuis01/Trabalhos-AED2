#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>


typedef int32_t List;
//Variavel global
long int nElements;

//Leitura do arquivo
void readFile(FILE *arq, List *L);
//Troca de variaveis
void swap(List *l1, List *l2);

/**
 *BubbleSort Original
 * @param n numero de elementos
 * @param L lista com os elementos a serem ordenados
 */
void bubbleSort(long int n, List *L) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n - 1; ++j) {
            if (L[j] > L[j + 1]) {
                swap(&L[j], &L[j + 1]);
            }
        }
    }
}
/**
 * BubbleSort Melhorado
 * @param n numero de elementos
 * @param L lista com os elementos a serem ordenados
 */
void improvedBubbleSort(long int n, List *L) {
    bool change = true; // varivel para indicar se houve a troca de elementos desornedados
    long int j, guarda = n, n1 = n;

    while (change) {
        j = 0;
        change = false;
        while (j < n1) {
            if (j + 1 < n) {
                if (L[j] > L[j + 1]) {
                    swap(&L[j], &L[j + 1]);
                    change = true;
                    guarda = j;
                }
            }
            ++j;
        }
        n1 = guarda;
    }
}
/**
 * InsertiorSort
 * @param n NUmero de elementos
 * @param L List com os elementos a serem ordenados
 */
void insertionSort(long int n, List *L) {
    List prov; //variavel provisoria
    List val;
    int j;
    for (int i = 1; i < n; ++i) {
        prov = L[i];
        val = L[i];
        j = i - 1;
        while (j >= 0 && val < L[j]) {
            L[j + 1] = L[j];
            --j;
        }
        L[j + 1] = prov;
    }
}
/**
 * Intercalar as sub-listas do MergeSort
 * @param L Lista com todos elementos
 * @param ini1
 * @param ini2
 * @param fim2
 */
void interlayer(List *L, long int ini1, long int ini2, long int fim2) {
    List *tmp = (int32_t *) malloc(nElements * sizeof(int32_t *));
    long int fim1 = ini2 - 1;
    long int nro = 0;
    long int ind = ini1;
    long int orig_ini1 = ini1;

    while (ini1 <= fim1 && ini2 <= fim2) {
        if (L[ini1] < L[ini2]) {
            tmp[ind] = L[ini1];
            ++ini1;
        } else {
            tmp[ind] = L[ini2];
            ++ini2;
        }
        ++ind, ++nro;
    }
    while (ini1 <= fim1) {
        tmp[ind] = L[ini1];
        ++ini1, ++ind, ++nro;
    }
    while (ini2 <= fim2) {
        tmp[ind] = L[ini2];
        ++ini2, ++ind, ++nro;
    }
    for (int i = 0; i < nro; ++i) {
        L[i + orig_ini1] = tmp[i + orig_ini1];
    }
    free(tmp);
}
/**
 *
 * @param L Lista com os elementos a seres ordenados
 * @param left indice do comeco da lista
 * @param right indice do fim da lista
 */
void mergeSort(List *L, long int left, long int right) {
    if (left < right) {
        long int center = left + (right - left) / 2;
        mergeSort(L, left, center);
        mergeSort(L, center + 1, right);
        interlayer(L, left, center + 1, right);
    }
}
/**
 * Procedimento pivo tem o objetivo de calcular a mediana de 3
 * @param L Lista com os elementos a serem ordenados
 * @param l indice do inicio da lista
 * @param r indice do final da lista
 * @param med indice da mediana dos 3 (que sera calculado)
 */
void pivo(const int32_t *L, List l, List r, long int *med) {
    long int mid = l + (r - l) / 2; //equivalente a (l + r) / 2
    List i = L[l];//Valor da chave no inicio da lista
    List f = L[r];//Valor da chave no final da lista
    List m = L[mid];//Valor da chave no meio da lista
    if (i < m) {
        if (m < f) {
            *med = mid;
        } else {
            if (i < f) {
                *med = r;
            } else {
                *med = l;
            }
        }
    } else {
        if (f < m) {
            *med = mid;
        } else {
            if (f < i) {
                *med = r;
            } else {
                *med = l;
            }
        }
    }
}
/**
 * Ordenacao rapido usando a mediana de 3 como pivo
 * @param L Lista com os elementos a serem ordenados
 * @param left inicio da lista
 * @param right final da lista
 */
void quickSortMediana(List *L, long int left, long int right) {
    long int mediana, i, j;
    List key;
    //Caso tenha apenas 2 elementos
    if (right - left < 2) {
        if (right - left == 1) {
            if (L[left] > L[right]) {
                swap(&L[left], &L[right]);
            }
        }
        //Caso tenha 3 ou mais elementos
    } else {
        pivo(L, left, right, &mediana);
        swap(&L[mediana], &L[right]);
        i = left, j = right - 1;
        key = L[right];
        while (j >= i) {
            while (L[i] < key) {
                ++i;
            }
            while (j > 0 && L[j] > key) {
                --j;
            }
            if (j >= i) {
                swap(&L[i], &L[j]);
                ++i, --j;
            }
        }
        swap(&L[i], &L[right]);
        quickSortMediana(L, left, i - 1);
        quickSortMediana(L, i + 1, right);
    }
}
/**
 * Ordenacao rapido usando o ultimo elemento da lista como pivo
 * @param L Lista com os elementos a serem ordenados
 * @param left indice do comeco da lista
 * @param right indice do final da lista
 */
void quickSortLast(List *L, long int left, long int right) {
    //Caso tenha apenas 2 elementos na lista
    if (right - left < 2) {
        if (right - left == 1) {
            if (L[left] > L[right]) {
                swap(&L[left], &L[right]);
            }
        }
        //Caso tenha 3 ou mais elementos na lista
    } else {
        List key = L[right];
        long int i = left;
        long int j = right;

        while (i < j) {
            while (i < j && L[i] < key) {
                ++i;
            }
            while (i < j && L[j] >= key) {
                --j;
            }
            swap(&L[i], &L[j]);
        }
        swap(&L[i], &L[right]);
        quickSortLast(L, left, i - 1);
        quickSortLast(L, i, right);
    }
}
/**
 * Ordenacao rapido utilizando um elemento aleatorio na lista como pivo
 * @param L Lista com os elementos a serem ordenados
 * @param left indice do comeco da lista
 * @param right indice do final da lista
 */
void quickSortRandom(List *L, long int left, long int right) {
    //Caso tenha apenas 2 elementos na lista
    if (right - left < 2) {
        if (right - left == 1) {
            if (L[left] > L[right]) {
                swap(&L[left], &L[right]);
            }
        }
        //Caso tenha 3 ou mais elementos na lista
    } else {
        long int r = left + rand() % (right - left);
        swap(&L[r], &L[right]);
        long int i = left - 1;
        List key = L[right];
        for (long int j = left; j <= right - 1; j++) {
            if (L[j] <= key) {
                i++;
                swap(&L[i], &L[j]);
            }
        }
        swap(&L[i + 1], &L[right]);
        quickSortRandom(L, left, i);
        quickSortRandom(L, i + 2, right);
    }
}

void descer(List *L, long int i, long int n) {
    long int j = 2 * i;
    if (j <= n) {
        if (j < n) {
            if (L[j + 1] > L[j]) {
                ++j;
            }
        }
        if (L[i] < L[j]) {
            swap(&L[i], &L[j]);
            descer(L, j, n);
        }
    }
}
/**
 * Procedimento que tem o objetivo de transforma a lista em uma heap
 * @param L Lista a ser transformada em heap
 */
void makeHeap(List *L) {
    long int n = nElements - 1;
    for (int i = 0; i < n; ++i) {
        int f = i + 1;
        while (f > 0 && L[(long int) floor(f / 2)] < L[f]) {
            swap(&L[(long int) floor(f / 2)], &L[f]);
            f /= 2;
        }
    }
}
/**
 * HeapSort
 * @param L Lista a ser ordenada
 */
void heapSort(List *L) {
    long int m = nElements - 1;
    while (m > 0) {
        swap(&L[0], &L[m]);
        --m;
        descer(L, 0, m);
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    List *L = NULL;
    FILE *in, *out;

    //Caso nao for passado a quantidade correta de argumentos, o programa sera parado
    if (argc != 4) {
        printf("How to use: %s (1..8) arq_entrada arq_saida", argv[0]);
        return EXIT_FAILURE;
    }
    // arquivo de entrada, com os elementos a serem ordenados
    in = fopen(argv[2], "rb");
    // arquivo de saida, com os elementos ja ordenados
    out = fopen(argv[3], "wb");

    if (in == NULL) {
        printf("\nErro ao abrir arquivo de entrada");
        return EXIT_FAILURE;
    }
    if(out == NULL) {
        printf("\nErro ao abrir arquibo de saida");
    }

    //Pegando a quantidade de elementos dentro do arquivo
    fseek(in, 0, SEEK_END);
    nElements = ftell(in) / 4;
    fseek(in, 0, SEEK_SET);
    //..

    L = (int32_t *) malloc(nElements * sizeof(int32_t *));//Alocando lista
    readFile(in, L);
    //Inicio do temporizador do programa. Antes do readFile, para desconsiderar o tempo da entrada
    clock_t begin = clock();

    switch (argv[1][0]) {
        case '1':
            bubbleSort(nElements, L);
            break;
        case '2':
            improvedBubbleSort(nElements, L);
            break;
        case '3':
            insertionSort(nElements, L);
            break;
        case '4':
            mergeSort(L, 0, nElements - 1);
            break;
        case '5':
            quickSortLast(L, 0, nElements - 1);
            break;
        case '6':
            quickSortRandom(L, 0, nElements - 1);
            break;
        case '7':
            quickSortMediana(L, 0, nElements - 1);
            break;
        case '8':
            makeHeap(L);
            heapSort(L);
            break;
        default:
            puts("Escolha entre 1 e 8!");
            break;
    }
    //Fim do temporizador do programa, antes do fwrite, para desconsiderar o tempo de saida.
    clock_t end = clock();
    double te = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("\nTempo de execucao: %.3fs\n", te);

    //Arquivo de saida.
    fwrite(L, sizeof(List), nElements, out);

    fclose(in);
    fclose(out);
    free(L);
    return EXIT_SUCCESS;
}

/**
 * Procedimento que tem como objetivo ler um arquivo
 * @param arq arquivo a ler lido
 * @param L Lista que ira guardar os elementos lidos
 */
void readFile(FILE *arq, List *L) {
    int i = 0;
    while (fread(&L[i], sizeof(L[i]), 1, arq)) {
        ++i;
    }
}
/**
 * Procedimento que tem como objetivo fazer a troca de valores
 * @param l1 elemento da lista a ser trocado
 * @param l2 outro elemento da lista a ser trocado
 */
void swap(List *l1, List *l2) {
    List aux;
    aux = *l1;
    *l1 = *l2;
    *l2 = aux;
}




