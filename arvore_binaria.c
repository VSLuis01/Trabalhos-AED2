#include "gfx.h"
#include <stdio.h>  /* printf */
#include <stdlib.h>
#include <unistd.h> /* sleep  */
#include <stdint.h>
#include <string.h>
/*
 * Structs utilizadas
 */
typedef struct s_no {
    int32_t chave:30;
    int32_t reservado:2; /* sem uso, mas deve ter sempre o key 0 (zero) */
    struct s_no* esq;
    struct s_no* dir;
}BiTree;
struct s_arq_no {
    int32_t chave:30;
    uint32_t esq:1;
    uint32_t dir:1;
};

//Procedimento para desenhar graficamente a arvore
void paintTree(BiTree* pt, unsigned short xi, unsigned short xf, unsigned short y);
//Menu, onde contém todas as opcoes
void menu();

//Procedimento para liberar a arvore binaria de forma recursiva em pré-ordem
void liberateBiTree(BiTree **root){
    if (*root == NULL) return;
    liberateBiTree(&(*root) -> esq);
    liberateBiTree(&(*root) -> dir);
    printf("\nNO LIBERADO %d[Adress -> %p]", (*root) -> chave, (void*)(*root));
    sleep(1);
    free(*root);
    *root = NULL;
}
/*
 * Funcao que trata os 3 tipos de remocao
 * Retorna o novo nó depois de ter removido
 */
BiTree* removeCurrent(BiTree* current) {
    BiTree *node1, *node2;
    //Tratamento se tem só um filho a direita ou se é uma folha
    //Também resolve se for uma folha
    if(current -> esq == NULL) {
        node2 = current -> dir;
        free(current);
        return node2;
    }
    //Resolve caso tenha apenas um filho a esquerda ou ambos filhos
    node1 = current;
    node2 = current -> esq;
    //Procura o filho mais a direita do filho a esquerda do nó a ser removido
    while (node2 -> dir != NULL) {
        node1 = node2;
        node2 = node2 -> dir;
    }

    if(node1 != current) {
        node1 -> dir = node2 -> esq;
        node2 -> esq = current -> esq;
    }
    node2 -> dir = current -> dir;
    free(current);
    return node2;
}
//Funcao de remocao, anda pela arvore até achar o nó a ser removido
int biRemove(BiTree** root, int key) {
    BiTree *prev = NULL;
    BiTree *current = *root;
    //Faz a busca pela chave a ser removida de forma iterativa
    //Guardando sempre o nó anterior, ou seja, o pai do nó a ser removido
    while(current != NULL) {
        if(key == current -> chave) {
            if(current == *root)
                *root = removeCurrent(current);
            else {
                if(prev -> dir == current)
                    prev -> dir = removeCurrent(current);
                else
                    prev -> esq = removeCurrent(current);
            }
            return 1;
        }
        prev = current;
        if(key > current -> chave) {
            current = current -> dir;
        } else {
            current = current -> esq;
        }
    }
    return 0;
}
/*
 * Funcao para buscar se a chave existe ou nao
 * Retornos da flag(f): 0 se a arvore for vazia, 1 se o pt aponta pro nó a ser buscado
 * 2 nao foi encontrado a esquerda e 3 nao foi encontrado a direita
 */
BiTree* searchTree(int key, BiTree* pt, int *f) {
    if(pt == NULL) {
        *f = 0;
    } else if(key == pt -> chave) {
        *f = 1;
        return pt;
    } else if(key < pt -> chave) {
        if(pt -> esq == NULL) {
            *f = 2;
            return pt;
        } else {
            pt = pt -> esq;
            return searchTree(key, pt, f);
        }
    } else if(key > pt -> chave){
        if(pt -> dir == NULL) {
            *f = 3;
            return pt;
        } else {
            pt = pt -> dir;
            return searchTree(key, pt, f);
        }
    }
    return pt;
}
/*
 * Funcao de insercao, usando como auxilio a funcao de busca
 */
int insert(int key, int* f, BiTree** ptRoot) {
    BiTree* pt = *ptRoot;
    pt = searchTree(key, pt, f);

    if(*f == 1) {
        return 0;
    } else {
        BiTree* pt1 = (BiTree*) malloc(sizeof(BiTree));
        pt1 -> chave = key;
        pt1 -> reservado = 0;
        pt1 -> dir = NULL;
        pt1 -> esq = NULL;

        if(*f == 0) {
            *ptRoot = pt1;
        } else if (*f == 2){
            pt -> esq = pt1;
        } else {
            pt -> dir = pt1;
        }
    }
    return 1;
}
/*
 * Funcao rescursiva para salvar a arvore no arquivo binario em pre-ordem
 */
void savePreOrder(BiTree* pt, FILE* arq) {
    if(pt == NULL) return;
    struct s_arq_no structsArq;

    structsArq.chave = pt -> chave;
    structsArq.esq = (pt -> esq != NULL) ? 1 : 0;
    structsArq.dir = (pt -> dir != NULL) ? 1 : 0;

    fwrite(&structsArq, sizeof(structsArq), 1, arq);

    savePreOrder(pt -> esq, arq);
    savePreOrder(pt -> dir, arq);

}
/*
 * Funcao para escrever a arvore em um arquivo binario
 * Retorna 0 caso ocorra falha ao escrever ou 1 caso a arvore for escrita com sucesso
 */
int writeBinaryFile(BiTree* biTree) {
    setbuf(stdin, NULL); //Limpa o buffer do teclado
    FILE* binArq = NULL;
    char str[25];
    //NAO ACEITA STRING VAZIA PARA SALVAR O ARQUIVO
    do {
        printf("\nENTRE COM O NOME DO ARQUIVO PARA SER GRAVADO, SEM ESPACOS(max 25 chars): ");
        fgets(str, sizeof(str), stdin);
        str[strlen(str) - 1] = '\0';
        if(strlen(str) == 0)
            printf("\nNOME VAZIO!\n");
    } while(strlen(str) == 0);

    binArq = fopen(str, "w");
    if(binArq == NULL) return 0;

    savePreOrder(biTree, binArq);
    fclose(binArq);

    return 1;
}

/*
 * Funcao responsavel por ler recursivamente e em pre-ordem o arquivo binario
 */
BiTree* readingFile(FILE* arq, BiTree* pt) {
    struct s_arq_no sArqNo;

    pt = (BiTree*) malloc(sizeof(BiTree));

    if(pt == NULL) {
        printf("\nFALHA AO ALOCAR MEMORIA!\n");
        sleep(2);
        return NULL;
    }
    if(!feof(arq)) {
        fread(&sArqNo, sizeof(sArqNo), 1, arq);
        //chamadas recursivas em pre-ordem, utilizando operador ternario
        pt -> chave = sArqNo.chave;
        pt -> esq = sArqNo.esq == 0 ? NULL : readingFile(arq, pt -> esq);
        pt -> dir = sArqNo.dir == 0 ? NULL : readingFile(arq, pt -> dir);

    }
    return pt;
}
/*
 * Funcao para ler a arvore em um arquivo
 * Retorna ponteiro para a raiz da arvore
 */
BiTree* readBinaryFile(BiTree* biTree) {
    setbuf(stdin, NULL);
    FILE* binArq;
    char str[25];

    printf("\nENTRE COM O NOME DO ARQUIVO PARA SER LIDO(max 25 chars): ");
    fgets(str, sizeof(str), stdin);
    str[strlen(str) - 1] = '\0';

    binArq = fopen(str, "r");
    if(binArq == NULL) {
        printf("\nNAO FOI POSSIVEL ENCONTRAR O ARQUIVO\n");
        sleep(2);
        return NULL;
    }
    return readingFile(binArq, biTree);
}

int main() {
    gfx_init(1280, 720, "Arvore binária de busca");
    gfx_set_font_size(20);
    menu();
    gfx_quit();
    return 0;
}

void menu() {
    BiTree* ptRoot = NULL;
    int flag, menu, opcao5;
    int32_t chave;

    do {
        printf("Operacoes disponiveis:\n");
        printf("\n"
               "1- INSERCAO\t4- GRAVAR ARVORE EM ARQ_BINARIO\n"
               "2- BUSCA\t5- LER ARVORE EM ARQ_BINARIO\n"
               "3- REMOCAO\t0- Sair\n");

        printf("\n");
        printf("Opcao: ");
        scanf("%d", &menu);
        switch (menu) {
            case 1:
                printf("\n");
                printf("\nEntre com a chave: ");
                scanf("%i", &chave);

                if(insert(chave, &flag, &ptRoot))
                    printf("\nCHAVE INSERIDA COM SUCESSO!\n");
                else
                    printf("\nCHAVE JA EXISTENTE!\n");

                paintTree(ptRoot, 0, gfx_get_width(), 40);
                gfx_paint();
                break;
            case 2:
                printf("\n");
                printf("\nEntre com a chave: ");
                scanf("%d", &chave);
                searchTree(chave, ptRoot, &flag);
                if(flag == 0) {
                    printf("\nARVORE VAZIA!\n");
                } else if(flag == 1) {
                    printf("\nA CHAVE (%d) FOI ENCONTRADA!\n", chave);
                } else {
                    printf("\nA CHAVE NAO ENCONTRADA!!\n");
                }
                sleep(1);
                break;
            case 3:
                if(ptRoot == NULL) {
                    printf("\nARVORE VAZIA!\n");
                    break;
                }
                printf("\n");
                printf("\nEntre com a chave: ");
                scanf("%d", &chave);
                if(biRemove(&ptRoot, chave)) {
                    printf("\nCHAVE REMOVIDA COM SUCESSO!\n");
                } else {
                    printf("\nCHAVE NAO ENCONTRADA!\n");
                }
                gfx_clear();
                paintTree(ptRoot, 0, gfx_get_width(), 40);
                gfx_paint();
                sleep(1);
                break;
            case 4:
                if (ptRoot == NULL) {
                    printf("\nARVORE VAZIA!\n");
                    break;
                }
                if(writeBinaryFile(ptRoot)) {
                    printf("\nARVORE SALVA COM SUCESSO NO PATH:\n");
                    system("pwd");//Mostrar o diretorio atual
                } else {
                    printf("\nFALHA AO ABRIR O ARQUIVO\n");
                }
                sleep(1);
                break;
            case 5:
                if(ptRoot != NULL) {
                    printf("\nDeseja salvar a arvore atual, antes de ler outra?\n");
                    do {
                        printf("\n1- SIM\t2- NAO\n");
                        printf("Opcao: ");
                        scanf("%d", &opcao5);
                        if(opcao5 != 1 && opcao5 != 2)
                            printf("\nOPCAO INVALIDA!");
                    } while (opcao5 != 1 && opcao5 != 2);
                    if(opcao5 == 1) {
                        if(writeBinaryFile(ptRoot)) {
                            printf("\nARVORE SALVA COM SUCESSO NO PATH:\n");
                            system("pwd");
                        } else {
                            printf("\nFALHA AO ABRIR O ARQUIVO\n");
                        }
                    }
                    liberateBiTree(&ptRoot);
                }
                ptRoot = readBinaryFile(ptRoot);
                if(ptRoot) printf("\nARVORE LIDA COM SUCESSO!\n");
                gfx_clear();
                paintTree(ptRoot, 0, gfx_get_width(), 40);
                gfx_paint();
                sleep(1);
                break;
            case 0:
                system("clear");
                if(ptRoot != NULL) printf("APAGANDO ARVORE...\n");
                liberateBiTree(&ptRoot);
                break;
            default:
                puts("OPERACAO INVALIDA!");
                sleep(1);
                break;
        }
        printf("\n");
    } while(menu != 0);

}

void paintTree(BiTree* pt, unsigned short xi, unsigned short xf, unsigned short y) {
    char str[64];
    unsigned short xm;

    if( pt == NULL )
        return;

    xm = (xi+xf)/2;
    snprintf(str, 64, "%i", pt -> chave);

    gfx_set_color(255, 255, 255);
    gfx_filled_ellipse(xm, y, 40, 40);
    gfx_set_color(0, 0, 0);

    gfx_text(xm - (unsigned short)(strlen(str) * 5), y - 10, str);

    gfx_set_color(255, 255, 255);

    if(pt -> esq != NULL) {
        gfx_line(xm, y + 10, (xi + xm)/ 2, y + 90);
    }
    paintTree(pt -> esq, xi, xm, y+90);

    if(pt -> dir != NULL) {
        gfx_line(xm, y + 10, (xm + xf)/ 2, y + 90);
    }
    paintTree(pt -> dir, xm, xf, y+90);
}

