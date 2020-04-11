#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

int R, P;

struct Player{
    int x, y;
    int hp, stamina;
    char name[300];
    int status;
    int gloves;
    int killed;
};

int FIGHT(struct Player [P], int, int, FILE *);
int cmp(struct Player *, struct Player *);
void Sort(struct Player [P], int);
void init_by_radius(int [2 * R + 2][2 * R + 2], int);
void check_glacier(struct Player [P], int, int [2 * R + 2][2 * R + 2], int [2 * R + 2][2 * R + 2], int, FILE *);
void reCheck_glacier(struct Player [P], int, int [2 * R + 2][2 * R + 2], int, FILE *);
void Move(struct Player [P], int, int [2 * R + 2][2 * R + 2], int [2 * R + 2][2 * R + 2], int [2 * R + 2][2 * R + 2], int, FILE *, FILE *);
void SnowStorm(int, struct Player [P], int, FILE *);

void dbg_matrix(int [2 * R + 2][2 * R + 2], int);
void dbg_Players(struct Player [P], int);


int main(int argc, char *argv[]) {
    int i, j;
    //strcpy(argv[1], "snowfight");
    int nfile = strlen(argv[1]);
    char infile[nfile + 5];
    strcpy(infile, argv[1]);
    strcat(infile, ".in");

    char outfile[nfile + 5];
    strcpy(outfile, argv[1]);
    strcat(outfile, ".out");

    FILE *f = fopen(infile, "r");
    FILE *g = fopen(outfile, "w");
    fscanf(f, "%d %d\n", &R, &P);
    int B[2*R+2][2*R+2];
    int A[2*R+2][2*R+2];
    int v[2*R+2][2*R+2];
    struct Player p[P];
    struct Player score_board[P];

    for(i = 0 ; i < 2 * R + 1 ; ++i)
        for(j = 0 ; j < 2 * R + 1 ; ++j)
            fscanf(f, "%d %d",&A[i][j], &B[i][j]);
    init_by_radius(v, R);
    //dbg_matrix(v, 2 * R + 1);
    int r = R;
    for(i = 0 ; i < P ; ++i){
        fscanf(f, "%s %d %d %d %d\n", p[i].name, &p[i].x, &p[i].y, &p[i].hp, &p[i].stamina);
        p[i].status = 1;
    }
    char name[300];
    check_glacier(p, P, v, B, 2 * r + 1, g);
    int nrac = 0;
    for(i = 0 ; i < P ; ++i)
        if(p[i].status == 1){++nrac;strcpy(name, p[i].name);}
    if(nrac == 1){
        fprintf(g, "%s has won!\n", name);
        return 0;
    }
    //dbg_Players(p, P);
    char cmd[20];

    int id, stamina;
    while(!feof(f)){
        fscanf(f, "%s", cmd);
        if(strcmp(cmd, "MOVE") == 0){
            fscanf(f, "%d ", &id);
            if(p[id].status)
            Move(p, id, v, A, B, 2 * r + 1, f, g);
        }
        else if(strcmp(cmd, "MELTDOWN") == 0){
            fscanf(f, "%d\n", &stamina);
            r--;

            init_by_radius(v, r);
            //dbg_matrix(v, 2 * R + 1);
            reCheck_glacier(p, P, v, stamina, g);
        }
        else if(strcmp(cmd, "PRINT_SCOREBOARD") == 0){
            memcpy(score_board, p, sizeof(p));
            Sort(score_board, P);
            fprintf(g, "SCOREBOARD:\n");
            for(i = 0 ; i < P ; ++i){
                fprintf(g, "%s\t", score_board[i].name);
                if(score_board[i].status == 0)fprintf(g, "WET");
                else fprintf(g, "DRY");
                fprintf(g, "\t%d\n",score_board[i].killed);
            }
        }
        else if(strcmp(cmd, "SNOWSTORM") == 0){
            int storm;
            fscanf(f, "%d\n", &storm);
            SnowStorm(storm, p, P, g);
        }
        nrac = 0;
        for(i = 0 ; i < P ; ++i)
            if(p[i].status == 1){++nrac;strcpy(name, p[i].name);}
        if(nrac == 1){
            fprintf(g, "%s has won.\n", name);
            break;
        }
    }
    return 0;
}

void init_by_radius(int v[2 * R + 2][2 * R + 2], int r){
    int i;
    int j;
    for(i = 0 ; i < 2 * R + 1 ; ++i)
        for(j = 0 ; j < 2 * R + 1 ; ++j)
            v[i][j] = 0;
    //memset(v, 0, sizeof(v));
    //dbg_matrix(v, 2 * R + 1);
    for(i = 0 ; i < 2 * R + 1 ; ++i)
        for(j = 0 ; j < 2 * R + 1 ; ++j){
            double d = sqrt((R - i) * (R - i) + (R - j) * (R - j));
            if(d <= r)
                v[i][j] = 1;
        }

}

void dbg_matrix(int v[2 * R + 2][2 * R + 2], int n){
    int i, j;
    for(i = 0 ; i < n ; ++i) {
        for (j = 0; j < n ; ++j) {
            printf("%d ", v[i][j]);
        }
        printf("\n");
    }
}

void check_glacier(struct Player v[P], int p, int a[2 * R + 2][2 * R + 2], int G[2 * R + 2][2 * R + 2], int n, FILE *printer){
    int i;
    for(i = 0 ; i < p ; ++i) {
        if (v[i].x >= n || v[i].x < 0 || v[i].y < 0 || v[i].y >= n || a[v[i].x][v[i].y] == 0) {
            v[i].status = 0;
            fprintf(printer, "%s has missed the glacier.\n", v[i].name);
        } else v[i].gloves = G[v[i].x][v[i].y];
        v[i].killed = 0;
    }
}
void dbg_Players(struct Player v[P], int n){
    int i;
    for(i = 0 ; i < n ; ++i){
        printf("%s %d %d %d %d %d %d\n", v[i].name, v[i].x, v[i].y, v[i].hp, v[i].stamina, v[i].status, v[i].gloves);
        v[i].status = 1;
    }
}

void Move(struct Player p[P], int id, int v[2 * R + 2][2 * R + 2], int A[2 * R + 2][2 * R + 2], int G[2 * R + 2][2 * R + 2], int n, FILE *reader, FILE *printer){
    char m;
    int i;

    while(fscanf(reader, "%c", &m) != EOF && m != '\n'){
        int x = p[id].x;
        int y = p[id].y;
        int nextx = x;
        int nexty = y;
        switch (m){
            case 'U': nextx--;break;
            case 'R': nexty++;break;
            case 'L': nexty--;break;
            case 'D': nextx++;break;
        }
        int cost = abs(A[x][y] - A[nextx][nexty]);
        if(cost <= p[id].stamina){
            p[id].stamina -= cost;
            p[id].x = x = nextx;
            p[id].y = y = nexty;
            if(x < 0 || y < 0 || x >= n || y >= n || v[x][y] == 0){
                fprintf(printer, "%s fell off the glacier.\n", p[id].name);
                p[id].status = 0;
                return;
            }
            if(G[x][y] > p[id].gloves){
                int aux = G[x][y];
                G[x][y] = p[id].gloves;
                p[id].gloves = aux;
            }
            for(i = 0 ; i < P ; ++i)
                if(i != id && p[i].status && x == p[i].x && y == p[i].y)
                    if(FIGHT(p, id, i, printer) == 0)return;
                    else break;

        }
    }
}

int FIGHT(struct Player p[P], int id1, int id2, FILE *printer){
    int win;
    if(p[id2].gloves == 0)win = 1;
    else if(p[id1].gloves == 0)win = 2;
    else{
        int h1 = p[id1].hp / p[id2].gloves + (p[id1].hp % p[id2].gloves != 0);
        int h2 = p[id2].hp / p[id1].gloves + (p[id2].hp % p[id1].gloves != 0);
        if(h1 > h2)
            win = 1;
        else if(h2 > h1)
            win = 2;
        else if(p[id1].stamina > p[id2].stamina)win = 1;
        else if(p[id2].stamina > p[id1].stamina)win = 2;
        else win = 1;
    }
    if(win == 1){
        p[id2].status = 0;
        p[id1].stamina += p[id2].stamina;
        p[id1].killed++;
        fprintf(printer, "%s sent %s back home.\n", p[id1].name, p[id2].name);
        return 1;
    }
    else{
        p[id1].status = 0;
        p[id2].stamina += p[id1].stamina;
        p[id2].killed++;
        fprintf(printer, "%s sent %s back home.\n", p[id2].name, p[id1].name);
        return 0;
    }
}

void reCheck_glacier(struct Player v[P], int p, int a[2 * R + 2][2 * R + 2], int stamina, FILE *printer){
    int i;
    for(i = 0 ; i < p ; ++i) {
        if (v[i].status == 1 && a[v[i].x][v[i].y] == 0) {
            v[i].status = 0;
            fprintf(printer, "%s got wet because of global warming.\n", v[i].name);
        }
        else v[i].stamina += stamina;
    }
}

void SnowStorm(int storm, struct Player v[P], int P, FILE *printer){
    unsigned char DMG, R, Y, X;
    int i;
    DMG = (storm >> 24);
    R = ((storm << 8) >> 24);
    Y = ((storm << 16) >> 24);
    X = ((storm << 24) >> 24);
    /*X = reverse(X);
    Y = reverse(Y);
    R = reverse(R);
    DMG = reverse(DMG);*/
    for(i = 0 ; i < P ; ++i){
        int dx, dy;
        if(X >= v[i].x)dx = X - v[i].x;
        else dx = v[i].x - X;
        if(Y >= v[i].y)dy = Y - v[i].y;
        else dy = v[i].y - Y;
        double d = sqrt(dx * dx + dy * dy);
        if((d <= R || (X == v[i].x && Y == v[i].y)) && v[i].status == 1) {
            v[i].hp -= DMG;
            if(v[i].hp <= 0){
                v[i].status = 0;
                fprintf(printer, "%s was hit by snowstorm.\n", v[i].name);
            }
        }
    }
}
int cmp(struct Player * a, struct Player * b){
    if(a->status == 1 && b->status == 0)
        return -1;
    if(a->status == 0 && b->status == 1)
        return 1;
    if(a->killed < b->killed)
        return 1;
    if(b->killed < a->killed)
        return -1;
    return strcmp(a->name, b->name);
}
void Sort(struct Player v[P], int n){
    int i, j;
    for(i = 0 ; i < n ; ++i)
        for(j = i + 1 ; j < n ; ++j){
            int comp = cmp(v + i, v + j);
            if(comp > 0){
                struct Player aux = v[i];
                v[i] = v[j];
                v[j] = aux;
            }
        }
}