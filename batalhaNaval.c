#include <stdio.h>
#include <string.h>

#define BOARD_SIZE 10

/* Valores no tabuleiro:
   0 = água
   3 = navio
   5 = área afetada pela habilidade
*/

/* Protótipos */
void inicializar_tabuleiro(int board[BOARD_SIZE][BOARD_SIZE]);
void colocar_navios_exemplo(int board[BOARD_SIZE][BOARD_SIZE]);
void criar_cone(int cone[][7], int size);       // ex.: 7x7
void criar_cruz(int cruz[][5], int size);       // ex.: 5x5
void criar_octaedro(int octa[][5], int size);   // ex.: 5x5 (losango)
void sobrepor_habilidade(int board[BOARD_SIZE][BOARD_SIZE],
                         int skill[][7], int skill_rows, int skill_cols,
                         int origin_row, int origin_col, int mark_value);
void sobrepor_habilidade_small(int board[BOARD_SIZE][BOARD_SIZE],
                         int skill[][5], int skill_rows, int skill_cols,
                         int origin_row, int origin_col, int mark_value);
void exibir_tabuleiro(int board[BOARD_SIZE][BOARD_SIZE]);

int main() {
    int board[BOARD_SIZE][BOARD_SIZE];

    /* 1) Inicializa tabuleiro com água */
    inicializar_tabuleiro(board);

    /* 2) Colocar alguns navios de exemplo (valor 3) */
    colocar_navios_exemplo(board);

    /* 3) Criar matrizes de habilidade dinamicamente usando loops e condicionais */
    int cone[7][7];  // 7x7 para o cone (topo no centro da linha 0 da matriz)
    int cruz[5][5];  // 5x5 para a cruz
    int octa[5][5];  // 5x5 para o octaedro (losango)

    criar_cone(cone, 7);
    criar_cruz(cruz, 5);
    criar_octaedro(octa, 5);

    /* 4) Definir pontos de origem no tabuleiro (linha, coluna)
       NOTA: coordenadas fixas no código conforme simplificação do enunciado */
    int origin_cone_row = 1, origin_cone_col = 4;   // posiciona o "topo" do cone centrado em (1,4)
    int origin_cruz_row = 5, origin_cruz_col = 2;   // centro da cruz em (5,2)
    int origin_octa_row = 7, origin_octa_col = 7;   // centro do octaedro em (7,7)

    /* 5) Sobrepor habilidades no tabuleiro. Usamos 5 para área afetada. */
    sobrepor_habilidade(board, cone, 7, 7, origin_cone_row, origin_cone_col, 5);
    sobrepor_habilidade_small(board, cruz, 5, 5, origin_cruz_row, origin_cruz_col, 5);
    sobrepor_habilidade_small(board, octa, 5, 5, origin_octa_row, origin_octa_col, 5);

    /* 6) Exibir resultado */
    printf("Legenda: 0 = agua, 3 = navio, 5 = area afetada\n\n");
    exibir_tabuleiro(board);

    return 0;
}

/* Inicializa todo o tabuleiro com 0 (água) */
void inicializar_tabuleiro(int board[BOARD_SIZE][BOARD_SIZE]) {
    int i, j;
    for (i = 0; i < BOARD_SIZE; i++) {
        for (j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = 0;
        }
    }
}

/* Coloca alguns navios apenas como exemplo (valores 3) */
void colocar_navios_exemplo(int board[BOARD_SIZE][BOARD_SIZE]) {
    /* Exemplo 1: navio horizontal de tamanho 4 em (2,2) a (2,5) */
    int c;
    for (c = 2; c <= 5; c++) board[2][c] = 3;

    /* Exemplo 2: navio vertical de tamanho 3 em (6,0) a (8,0) */
    int r;
    for (r = 6; r <= 8; r++) board[r][0] = 3;

    /* Exemplo 3: navio único (submarino) em (4,7) */
    board[4][7] = 3;
}

/* Cria uma matriz 7x7 que representa um cone apontando para baixo.
   Lógica: o topo fica na linha 0, coluna central; a cada linha descida, a largura aumenta em 2.
   Usamos loops e condicionais para preencher com 1/0. */
void criar_cone(int cone[][7], int size) {
    int center = size / 2; // para 7 -> center = 3
    int r, c;

    for (r = 0; r < size; r++) {
        for (c = 0; c < size; c++) {
            /* distância horizontal ao centro */
            int dist = c - center;
            if (dist < 0) dist = -dist;

            /* largura desejada na linha r: 1 + 2*r (mas limitada a size) */
            int half_width = r; // r passos para cada lado
            if (dist <= half_width) {
                cone[r][c] = 1; // dentro do cone
            } else {
                cone[r][c] = 0; // fora do cone
            }
        }
    }

    /* Observação/Comentário:
       A linha 0 terá apenas o centro = 1.
       A linha 1 terá center-1..center+1 = 3 células = 1.
       A linha 2 terá center-2..center+2 = 5 células = 1, etc. */
}

/* Cria uma matriz 5x5 em forma de cruz (linha central e coluna central = 1) */
void criar_cruz(int cruz[][5], int size) {
    int center = size / 2; // para 5 -> center = 2
    int r, c;
    for (r = 0; r < size; r++) {
        for (c = 0; c < size; c++) {
            /* Condicional: é 1 se estiver na linha central OU na coluna central */
            if (r == center || c == center) {
                cruz[r][c] = 1;
            } else {
                cruz[r][c] = 0;
            }
        }
    }
}

/* Cria uma matriz 5x5 que representa a vista frontal de um octaedro (losango).
   Critério: abs(r-center) + abs(c-center) <= center => dentro do losango (valor 1) */
void criar_octaedro(int octa[][5], int size) {
    int center = size / 2; // 2
    int r, c;
    for (r = 0; r < size; r++) {
        for (c = 0; c < size; c++) {
            int dr = r - center; if (dr < 0) dr = -dr;
            int dc = c - center; if (dc < 0) dc = -dc;
            if (dr + dc <= center) {
                octa[r][c] = 1;
            } else {
                octa[r][c] = 0;
            }
        }
    }
}

/* Sobrepõe uma habilidade cujo array máximo é 7x7 (usado para cone).
   A matriz skill contém 1/0; sobrepomos centrando a skill no ponto origin_row, origin_col.
   Se o ponto calculado estiver dentro dos limites do tabuleiro e skill == 1,
   marcamos com mark_value (ex.: 5). */
void sobrepor_habilidade(int board[BOARD_SIZE][BOARD_SIZE],
                         int skill[][7], int skill_rows, int skill_cols,
                         int origin_row, int origin_col, int mark_value) {
    int center_r = skill_rows / 2;
    int center_c = skill_cols / 2;
    int i, j;
    for (i = 0; i < skill_rows; i++) {
        for (j = 0; j < skill_cols; j++) {
            if (skill[i][j] == 1) {
                /* mapa a posição da skill sobre o tabuleiro */
                int board_r = origin_row + (i - center_r);
                int board_c = origin_col + (j - center_c);

                /* validação para permanecer dentro dos limites do tabuleiro */
                if (board_r >= 0 && board_r < BOARD_SIZE && board_c >= 0 && board_c < BOARD_SIZE) {
                    board[board_r][board_c] = mark_value;
                }
            }
        }
    }
}

/* Versão para matrizes 5x5 (assinatura separada por facilidade) */
void sobrepor_habilidade_small(int board[BOARD_SIZE][BOARD_SIZE],
                         int skill[][5], int skill_rows, int skill_cols,
                         int origin_row, int origin_col, int mark_value) {
    int center_r = skill_rows / 2;
    int center_c = skill_cols / 2;
    int i, j;
    for (i = 0; i < skill_rows; i++) {
        for (j = 0; j < skill_cols; j++) {
            if (skill[i][j] == 1) {
                int board_r = origin_row + (i - center_r);
                int board_c = origin_col + (j - center_c);
                if (board_r >= 0 && board_r < BOARD_SIZE && board_c >= 0 && board_c < BOARD_SIZE) {
                    board[board_r][board_c] = mark_value;
                }
            }
        }
    }
}

/* Exibe o tabuleiro no console usando printf e loops aninhados.
   Imprime números separados por espaço para representar cada célula. */
void exibir_tabuleiro(int board[BOARD_SIZE][BOARD_SIZE]) {
    int i, j;
    for (i = 0; i < BOARD_SIZE; i++) {
        for (j = 0; j < BOARD_SIZE; j++) {
            printf("%d ", board[i][j]);
        }
        printf("\n");
    }
}
