#include "gfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
/**
 * Estruturas utilizadas
 */
typedef struct s_no {
    int32_t key:28;
    int32_t bal:2;
    int32_t reserved:2; /* sem uso */
    struct s_no* left;
    struct s_no* right;
} AVLTree;
typedef struct s_arq_no {
    int32_t key:28;
    int32_t bal:2;
    uint32_t left:1;
    uint32_t right:1;
} ARQTree;

/**
 *
 * @param pt Arvore AVL
 * @param xi X inicial da tela
 * @param xf X final da tela
 * @param y altura
 */
void paintTree(AVLTree* pt, unsigned int xi, unsigned int xf, unsigned int y);
void menu();

/**
 *
 * @param root Arvore AVL
 * Deleta recursivamente a arvore AVL
 */
void delete(AVLTree** root) {
    if (*root == NULL) return;
    delete(&(*root) -> left);
    delete(&(*root) -> right);
    printf("\nNO LIBERADO %d[Adress -> %p]", (*root) -> key, (void*)(*root));
    free(*root);
    *root = NULL;
}
AVLTree* rightRotation(AVLTree** pt) {
    AVLTree *u, *v;
    u = (*pt) -> left;
    if(u -> bal > 0) {
        v = u -> right;
        u -> right = v -> left;
        (*pt) -> left = v -> right;
        v -> left = u;
        v -> right = (*pt);
        switch (v -> bal) {
            case -1:
                u -> bal = 0; (*pt) -> bal = 1;
                break;
            case 0:
                u -> bal = 0; (*pt) -> bal = 0;
                break;
            case 1:
                u -> bal = -1; (*pt) -> bal = 0;
                break;
            default:
                break;
        }
        v -> bal = 0;
        return v;
    }
    (*pt) -> left = u -> right;
    u -> right = (*pt);
    if(u -> bal < 0) {
        u -> bal = 0;
        (*pt) -> bal = 0;
    } else {
        u -> bal = 1;
        (*pt) -> bal = -1;
    }
    return u;
}
AVLTree* leftRotation(AVLTree** pt) {
    AVLTree *u, *v;
    u = (*pt) -> right;
    if(u -> bal > 0) {
        v = u -> left;
        u -> left = v -> right;
        (*pt) -> right = v -> left;
        v -> right = u;
        v -> left = (*pt);
        switch (v -> bal) {
            case -1:
                u -> bal = -1; (*pt) -> bal = 0;
                break;
            case 0:
                u -> bal = 0; (*pt) -> bal = 0;
                break;
            case 1:
                u -> bal = 0; (*pt) -> bal = 1;
                break;
            default:
                break;
        }
        v -> bal = 0;
        return v;
    }
    (*pt) -> right = u -> left;
    u -> left = (*pt);
    if(u -> bal < 0) {
        u -> bal = 0;
        (*pt) -> bal = 0;
    } else {
        u -> bal = -1;
        (*pt) -> bal = 1;
    }
    return u;
}
AVLTree* predecessor(AVLTree* pt) {
    pt = pt -> left;

    while(pt -> right != NULL)
        pt = pt -> right;

    return pt;
}

bool removeAVL(int32_t key, AVLTree** pAvlTree, bool *h) {
    AVLTree *temp;

    if((*pAvlTree) == NULL) {
        printf("CHAVE NAO ENCONTRADA!\n");
        return false;
    }
    if(key < (*pAvlTree) -> key) {
        if(removeAVL(key, &(*pAvlTree) -> left, h)) {
            if(*h) {
                ((*pAvlTree) -> bal)++;
                switch ((*pAvlTree) -> bal) {
                    case -2:
                        puts("Rotacao Esquerda");
                        (*pAvlTree) = leftRotation(&(*pAvlTree));
                        if((*pAvlTree) -> bal == 1) *h = false;
                        break;
                    case 1:
                        *h = false;
                        break;
                    default:
                        break;
                }
                return true;
            }
        }
    } else {
        if(key > (*pAvlTree) -> key) {
            if(removeAVL(key, &(*pAvlTree) -> right, h)) {
                if(*h) {
                    ((*pAvlTree) -> bal)--;
                    switch ((*pAvlTree) -> bal) {
                        case -2:
                            puts("Rotacao Direita");
                            (*pAvlTree) = rightRotation(&(*pAvlTree));
                            if((*pAvlTree) -> bal == -1) *h = false;
                            break;
                            case -1:
                                *h = false;
                                break;
                            default:
                                break;
                    }
                    return true;
                }
            }
        } else {
            if(key == (*pAvlTree) -> key) {
                if(((*pAvlTree) -> left == NULL) && ((*pAvlTree) -> right == NULL)) {
                    free(*pAvlTree);
                    (*pAvlTree) = NULL;
                    *h = true;
                }else if(((*pAvlTree) -> left != NULL) && ((*pAvlTree) -> right != NULL)) {
                    temp = predecessor(*pAvlTree);
                    (*pAvlTree) -> key = temp -> key;
                    removeAVL((*pAvlTree) -> key, &(*pAvlTree) -> left, h);
                } else {
                    if((*pAvlTree) -> left != NULL) {
                        (*pAvlTree) -> key = (*pAvlTree) -> left -> key;
                        free((*pAvlTree) -> left);
                        (*pAvlTree) -> left = NULL;
                    } else {
                        (*pAvlTree) -> key = (*pAvlTree) -> right -> key;
                        free((*pAvlTree) -> right);
                        (*pAvlTree) -> right = NULL;
                    }
                    *h = true;
                }
                return true;
            }
        }
    }
    return false;
}

/**
 *
 * @param pt Nó a ser iniciado
 * @param key chave para iniciar o nó
 * Aloca o nó e inicia seus atributos
 */
void startNode(AVLTree **pt, int32_t key) {
    (*pt) = (AVLTree*) malloc(sizeof(AVLTree));
    (*pt) -> left = NULL, (*pt) -> right = NULL;
    (*pt) -> key = key, (*pt) -> bal = 0, (*pt) -> reserved = 0;
}
/**
 *
 * @param pt Sub-Arvore que precisa se rebalanceada
 * @param h flag para identificar se a altura foi alterada ou não
 * Faz o balanceamento na sub-arvore à esquerda
 */
void rebalancingCase1(AVLTree** pt, bool* h) {
    AVLTree* ptu = (*pt) -> left;
    if(ptu -> bal == -1) {
        (*pt) -> left = ptu -> right;
        ptu -> right = (*pt);
        (*pt) -> bal = 0;
        (*pt) = ptu;
    } else {
        AVLTree* ptv = ptu -> right;
        ptu -> right = ptv -> left;
        ptv -> left = ptu;
        (*pt) -> left = ptv -> right;
        ptv -> right = (*pt);

        if(ptv -> bal == -1) (*pt) -> bal = 1;
        else (*pt) -> bal = 0;

        if(ptv -> bal == 1) ptu -> bal = -1;
        else ptu -> bal = 0;
        (*pt) = ptv;
    }
    (*pt) -> bal = 0;
    *h = false;
}
/**
 *
 * @param pt Sub-Arvore que precisa se rebalanceada
 * @param h flag para identificar se a altura foi alterada ou não
 * Faz o balanceamento na sub-arvore à direita
 */
void rebalancingCase2(AVLTree** pt, bool *h) {
    AVLTree* ptu = (*pt) -> right;
    if(ptu -> bal == 1) {
        (*pt) -> right = ptu -> left;
        ptu -> left = (*pt);
        (*pt) -> bal = 0;
        (*pt) = ptu;
    } else {
        AVLTree* ptv = ptu -> left;
        ptu -> left = ptv -> right;
        ptv -> right = ptu;
        (*pt) -> right = ptv -> left;
        ptv -> left = (*pt);

        if(ptv -> bal == 1) (*pt) -> bal = -1;
        else (*pt) -> bal = 0;

        if(ptv -> bal == -1) ptu -> bal = 1;
        else ptu -> bal = 0;
        (*pt) = ptv;
    }
    (*pt) -> bal = 0;
    *h = false;
}
/**
 *
 * @param key Chave do nó
 * @param pAvlTree nó a ser inserido
 * @param h flag para identificar se foi alterado a altura
 */
void insertAVL(int32_t key, AVLTree** pAvlTree, bool* h) {
    //Chave não encontrada, então pode inserir
    if(*pAvlTree == NULL) {
        startNode(&(*pAvlTree), key);
        *h = true;
    } else if(key == (*pAvlTree) -> key) {
        printf("CHAVE JA EXISTENTE!\n");
        return ;
    } else {
        //Procura se chave existe pela esquerda
        if(key < (*pAvlTree) -> key) {
            insertAVL(key, &(*pAvlTree) -> left, h);
            if(*h) {
                switch ((*pAvlTree) -> bal) {
                    case 1:
                        (*pAvlTree) -> bal = 0;
                        *h = false;
                        break;
                    case 0:
                        (*pAvlTree) -> bal = -1;
                        break;
                    case -1:
                        rebalancingCase1(&(*pAvlTree), h);
                        break;
                    default:
                        break;
                }
            }
        } else {
            //Procura se a chave existe pela direita
            insertAVL(key, &(*pAvlTree) -> right, h);
            if(*h) {
                switch ((*pAvlTree) -> bal) {
                    case -1:
                        (*pAvlTree) -> bal = 0;
                        *h = false;
                        break;
                    case 0:
                        (*pAvlTree) -> bal = 1;
                        break;
                    case 1:
                        rebalancingCase2(&(*pAvlTree), h);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
/**
 *
 * @param key Chave a ser buscada
 * @param pt Raiz da árvore AVL
 * @return retorna true caso a chave foi encontrada, caso contrário, retorna false
 */
bool AVLFind(int32_t key, AVLTree* pt) {
    if(pt == NULL) {
        return false;
    } else if(key == pt -> key) {
        return true;
    } else if(key < pt -> key) {
        return AVLFind(key, pt -> left);
    }
    return AVLFind(key, pt -> right);
}
/**
 *
 * @param pt nó da árvore
 * @param arq arquivo que vai guardar a árvore
 * Salva a árvore recursivamente em pré-ordem no arquivo binário
 */
void savePreOrder(AVLTree* pt, FILE* arq) {
    if(pt == NULL) return;
    ARQTree structsArq;

    //INiciado os valores na struct do arquivo
    structsArq.key = pt -> key;
    structsArq.bal = pt -> bal;
    structsArq.left = (pt -> left != NULL) ? 1 : 0;
    structsArq.right = (pt -> right != NULL) ? 1 : 0;

    fwrite(&structsArq, sizeof(structsArq), 1, arq); //Escrevendo no arquivo

    savePreOrder(pt -> left, arq);
    savePreOrder(pt -> right, arq);

}
/**
 *
 * @param avlTree Raiz da arvore AVL
 * @return retorna 1, caso a arvore for escrita com sucesso, caso contrario retorna 0
 */
int writeBinaryFile(AVLTree* avlTree) {
    setbuf(stdin, NULL); //Limpa o buffer do teclado
    FILE* binArq = NULL;
    char str[30];
    //NAO ACEITA STRING VAZIA PARA SALVAR O ARQUIVO
    do {
        printf("\nENTRE COM O NOME DO ARQUIVO PARA SER GRAVADO, SEM ESPACOS(max 25 chars): ");
        fgets(str, sizeof(str), stdin);
        str[strlen(str) - 1] = '\0';
        if(strlen(str) == 0)
            printf("\nNOME VAZIO!\n");
    } while(strlen(str) == 0);
    strcat(str, ".bin");//Adiciona a extensão .bin no arquivo

    binArq = fopen(str, "wb");
    if(binArq == NULL) return 0;

    savePreOrder(avlTree, binArq);
    fclose(binArq);
    return 1;
}
/**
 *
 * @param arq Arquivo contendo a arvore AVL
 * @param pt Ponteiro que vai receber a raiz da arvore AVL
 * @return Retorna a raiz da arvore AVL
 * Le o arquivo recursivamente em pre-ordem
 */
AVLTree* readingFile(FILE* arq, AVLTree* pt) {
    ARQTree sArqNo;

    pt = (AVLTree*) malloc(sizeof(AVLTree));

    if(pt == NULL) {
        printf("\nFALHA AO ALOCAR MEMORIA!\n");
        sleep(1);
        return NULL;
    }
    if(feof(arq)) return NULL;
    if(fread(&sArqNo, sizeof(sArqNo), 1, arq)) {
        //chamadas recursivas em pre-ordem, utilizando operador ternario
        pt -> key = sArqNo.key;
        pt -> bal = sArqNo.bal;
        pt -> left = sArqNo.left == 0 ? NULL : readingFile(arq, pt -> left);
        pt -> right = sArqNo.right == 0 ? NULL : readingFile(arq, pt -> right);

    } else return NULL;
    return pt;
}
/**
 *
 * @return Retorna a raiz da arvore AVL
 */
AVLTree* readBinaryFile() {
    setbuf(stdin, NULL);
    FILE *binArq;
    AVLTree *avlTree = NULL;
    char str[25];

    printf("\nENTRE COM O NOME DO ARQUIVO PARA SER LIDO, SEM ESPACOS(max 25 chars): ");
    fgets(str, sizeof(str), stdin);
    str[strlen(str) - 1] = '\0';

    binArq = fopen(str, "rb");
    if (binArq == NULL) {
        printf("\nNAO FOI POSSIVEL ENCONTRAR O ARQUIVO\n");
        return NULL;
    }
    return readingFile(binArq, avlTree);
}
//FUNÇÃO MAIN...........
int main() {
    gfx_init(1920, 1080, "Arvore binária de busca");
    gfx_set_font_size(12);
    menu();
    gfx_quit();
    return 0;
}
void menu() {
    AVLTree *ptRoot = NULL;
    short menu, opt;
    int32_t key;

    do {
        bool h = false;
        printf("Operacoes disponiveis:\n");
        printf("\n"
               "1- INSERCAO\t4- GRAVAR ARVORE EM ARQ_BINARIO\n"
               "2- BUSCA\t5- LER ARVORE EM ARQ_BINARIO\n"
               "3- REMOCAO\t0- Sair\n\n");

        printf("Opcao: ");
        scanf("%hd", &menu);
        switch (menu) {
            case 1:
                printf("\nEntre com a chave: ");
                scanf("%d", &key);
                insertAVL(key, &ptRoot, &h);

                gfx_clear();
                paintTree(ptRoot, 0, gfx_get_width(), 40);
                gfx_paint();
                break;
            case 2:
                if(ptRoot != NULL) {
                    printf("\nEntre com a chave a ser buscada: ");
                    scanf("%d", &key);
                    if(AVLFind(key, ptRoot)) {
                        printf("\nCHAVE ENCONTRADA!");
                    } else {
                        printf("\nCHAVE NAO FOI ENCONTRADA!");
                    }
                } else {
                    printf("\nARVORE VAZIA!");
                }
                break;
            case 3:
                if(ptRoot) {
                    printf("\nEntre com a chave a ser removida: ");
                    scanf("%d", &key);

                    removeAVL(key, &ptRoot, &h);

                    gfx_clear();
                    paintTree(ptRoot, 0, gfx_get_width(), 40);
                    gfx_paint();
                } else {
                    printf("\nARVORE VAZIA!");
                }
                break;
            case 4:
                if(writeBinaryFile(ptRoot)) {
                    printf("\nARVORE SALVA COM SUCESSO NO PATH:\n");
                    system("pwd");//Mostrar o diretorio atual
                } else {
                    printf("\nFALHA AO ABRIR O ARQUIVO\n");
                }
                break;
            case 5:
                if(ptRoot != NULL) {
                    printf("\nDeseja salvar a arvore atual, antes de ler outra?\n");
                    do {
                        printf("\n1- SIM\t2- NAO\n");
                        printf("Opcao: ");
                        scanf("%hd", &opt);
                        if(opt != 1 && opt != 2)
                            printf("\nOPCAO INVALIDA!");
                    } while (opt != 1 && opt != 2);
                    if(opt == 1) {
                        if(writeBinaryFile(ptRoot)) {
                            printf("\nARVORE SALVA COM SUCESSO NO PATH:\n");
                            system("pwd");
                        } else {
                            printf("\nFALHA AO ABRIR O ARQUIVO\n");
                        }
                    }
                    delete(&ptRoot);
                    printf("\n");
                    gfx_clear();
                }
                ptRoot = readBinaryFile();
                if(ptRoot) printf("\nARVORE LIDA COM SUCESSO!\n");

                gfx_clear();
                paintTree(ptRoot, 0, gfx_get_width(), 40);
                gfx_paint();
                break;
            case 0:
                delete(&ptRoot);
                break;
            default:
                puts("OPERACAO INVALIDA!");
                break;
        }
        printf("\n");
    } while(menu != 0);

}
void paintTree(AVLTree* pt, unsigned int xi, unsigned int xf, unsigned int y) {
    char key[32], bal[32];
    unsigned int xm;

    if(pt == NULL)
        return;

    xm = (xi + xf)/2;
    snprintf(key, 32, "%d", pt -> key);
    snprintf(bal, 32, "%d", pt -> bal);

    gfx_set_color(18, 255, 0);
    gfx_filled_ellipse(xm, y, 23, 23);
    gfx_set_color(255, 255, 255);
    gfx_filled_ellipse(xm, y, 20, 20);

    gfx_set_color(0, 0, 0);
    gfx_text(xm - (unsigned int)(strlen(key) * 4), y - 15, key);
    gfx_set_color(255, 0, 0);
    gfx_text(xm - (unsigned int)(strlen(key) * 4), y - 2, bal);

    if(pt -> left != NULL) {
        gfx_set_color(255, 255, 255);
        gfx_line(xm, y + 10, (xi + xm)/ 2, y + 90);
    }
    if(pt -> right != NULL) {
        gfx_set_color(255, 255, 255);
        gfx_line(xm, y + 10, (xm + xf)/ 2, y + 90);
    }

    paintTree(pt -> left, xi, xm, y + 90);
    paintTree(pt -> right, xm, xf, y + 90);
}



