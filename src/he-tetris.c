#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/select.h>

#define WIDTH 10
#define HEIGHT 20

int board[HEIGHT][WIDTH] = {0};
int score = 0;
int game_over = 0;

// Tetromino shapes 4x4 matrix
int pieces[7][4][4] = {
    // I (Cyan, 1)
    {{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}},
    // J (Blue, 2)
    {{2,0,0,0}, {2,2,2,0}, {0,0,0,0}, {0,0,0,0}},
    // L (Orange, 3)
    {{0,0,3,0}, {3,3,3,0}, {0,0,0,0}, {0,0,0,0}},
    // O (Yellow, 4)
    {{0,4,4,0}, {0,4,4,0}, {0,0,0,0}, {0,0,0,0}},
    // S (Green, 5)
    {{0,5,5,0}, {5,5,0,0}, {0,0,0,0}, {0,0,0,0}},
    // T (Purple, 6)
    {{0,6,0,0}, {6,6,6,0}, {0,0,0,0}, {0,0,0,0}},
    // Z (Red, 7)
    {{7,7,0,0}, {0,7,7,0}, {0,0,0,0}, {0,0,0,0}}
};

int colors[] = {0, 36, 34, 33, 31, 32, 35, 31}; // ANSI codes

struct Piece {
    int shape_idx;
    int matrix[4][4];
    int x, y;
} current, next;

// Terminal setting restoration
struct termios orig_termios;
void reset_terminal_mode() {
    tcsetattr(0, TCSANOW, &orig_termios);
    printf("\033[?25h"); // Show cursor
}

void set_conio_terminal_mode() {
    struct termios new_termios;
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));
    atexit(reset_terminal_mode);
    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 1;
    new_termios.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &new_termios);
    printf("\033[?25l"); // Hide cursor
}

int kbhit() {
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv) > 0;
}

int getch() {
    if (kbhit()) return getchar();
    return 0;
}

void new_piece(struct Piece *p) {
    p->shape_idx = rand() % 7;
    memcpy(p->matrix, pieces[p->shape_idx], sizeof(p->matrix));
    p->x = WIDTH / 2 - 2;
    p->y = 0;
}

int check_collision(struct Piece *p, int nx, int ny) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (p->matrix[r][c] != 0) {
                int bx = nx + c;
                int by = ny + r;
                if (bx < 0 || bx >= WIDTH || by >= HEIGHT || (by >= 0 && board[by][bx] != 0)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void lock_piece() {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (current.matrix[r][c] != 0 && current.y + r >= 0) {
                board[current.y + r][current.x + c] = current.matrix[r][c];
            }
        }
    }
    
    // Clear lines
    int lines_cleared = 0;
    for (int r = HEIGHT - 1; r >= 0; r--) {
        int full = 1;
        for (int c = 0; c < WIDTH; c++) {
            if (board[r][c] == 0) { full = 0; break; }
        }
        if (full) {
            lines_cleared++;
            for (int k = r; k > 0; k--) {
                for (int c = 0; c < WIDTH; c++) board[k][c] = board[k-1][c];
            }
            for (int c = 0; c < WIDTH; c++) board[0][c] = 0;
            r++; // Recheck same row
        }
    }
    score += lines_cleared * 100;
    
    current = next;
    new_piece(&next);
    if (check_collision(&current, current.x, current.y)) {
        game_over = 1;
    }
}

void rotate_piece() {
    struct Piece temp = current;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            current.matrix[c][3-r] = temp.matrix[r][c];
        }
    }
    // Wall kick
    if (check_collision(&current, current.x, current.y)) {
        if (!check_collision(&current, current.x - 1, current.y)) current.x--;
        else if (!check_collision(&current, current.x + 1, current.y)) current.x++;
        else current = temp; // Revert
    }
}

void draw() {
    printf("\033[H"); // Move cursor to top left
    printf("   Helium Tetris\n");
    printf(" Score: %d\n", score);
    printf("<!");
    for(int i=0; i<WIDTH; i++) printf("==");
    printf("!>\n");
    
    for (int r = 0; r < HEIGHT; r++) {
        printf("<!");
        for (int c = 0; c < WIDTH; c++) {
            int draw_val = board[r][c];
            if (draw_val == 0) {
                // Check if current piece occupies here
                for (int pr = 0; pr < 4; pr++) {
                    for (int pc = 0; pc < 4; pc++) {
                        if (current.matrix[pr][pc] != 0 && current.y + pr == r && current.x + pc == c) {
                            draw_val = current.matrix[pr][pc];
                        }
                    }
                }
            }
            if (draw_val != 0) {
                printf("\033[%dm[]\033[0m", colors[draw_val]);
            } else {
                printf(" .");
            }
        }
        
        // Print Next Piece Box
        if (r == 1) printf("!>  Next:");
        else if (r >= 2 && r <= 5) {
            printf("!>  ");
            int pr = r - 2;
            for (int pc = 0; pc < 4; pc++) {
                if (next.matrix[pr][pc] != 0) printf("\033[%dm[]\033[0m", colors[next.matrix[pr][pc]]);
                else printf("  ");
            }
        }
        else printf("!>");
        printf("\n");
    }
    printf("<!");
    for(int i=0; i<WIDTH; i++) printf("==");
    printf("!>\n");
    printf(" Controls: A/D/S = Move, W = Rotate, Q = Quit\n");
}

void handle_input() {
    int key = getch();
    if (key == 0) return;
    
    // Handle escape sequences or raw chars
    if (key == '\033') {
        if (getch() == '[') {
            switch(getch()) {
                case 'A': key = 'w'; break; // Up
                case 'B': key = 's'; break; // Down
                case 'C': key = 'd'; break; // Right
                case 'D': key = 'a'; break; // Left
            }
        }
    }
    
    if (key == 'A' || key == 'a') {
        if (!check_collision(&current, current.x - 1, current.y)) current.x--;
    } else if (key == 'D' || key == 'd') {
        if (!check_collision(&current, current.x + 1, current.y)) current.x++;
    } else if (key == 'S' || key == 's') {
        if (!check_collision(&current, current.x, current.y + 1)) current.y++;
    } else if (key == 'W' || key == 'w') {
        rotate_piece();
    } else if (key == 'Q' || key == 'q') {
        game_over = 1;
    }
}

int main() {
    srand(time(NULL));
    set_conio_terminal_mode();
    printf("\033[2J"); // Clear screen
    
    new_piece(&current);
    new_piece(&next);
    
    unsigned long timer = 0;
    while (!game_over) {
        draw();
        handle_input();
        
        // Gravity roughly every 500ms
        if (++timer % 50 == 0) {
            if (!check_collision(&current, current.x, current.y + 1)) {
                current.y++;
            } else {
                lock_piece();
            }
        }
        
        usleep(10000); // 10ms tick
    }
    
    printf("\nGame Over! Final Score: %d\n", score);
    return 0;
}
